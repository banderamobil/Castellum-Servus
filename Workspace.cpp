// System definition files.
//
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libconfig.h++>
#include <stdexcept>

// Common definition files.
//
#include "GPIO/GPIO.h"
#include "GPIO/LCD.h"
#include "GPIO/Relay.h"
#include "GPIO/Strip.h"
#include "GPIO/Therma.h"
#include "HTTP/Service.h"
#include "MMPS/MMPS.h"
#include "MODBUS/Service.h"
#include "Signals/Signals.h"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Servus/Configuration.h"
#include "Servus/GKrellM.h"
#include "Servus/Workspace.h"
#include "Servus/WWW/Home.h"

using namespace libconfig;

static void
OwnSignalHandler(int signal_number);

static Workspace::Servus *instance = NULL;

Workspace::Servus &
Workspace::Servus::InitInstance()
{
    if (instance != NULL)
        throw std::runtime_error("Workspace already initialized");

    instance = new Workspace::Servus();

    return *instance;
}

Workspace::Servus &
Workspace::Servus::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("Workspace not initialized");

    return *instance;
}

Workspace::Servus::Servus() :
Inherited()
{
    try
    {
        GPIO::RelayStation::InitInstance();
        GPIO::Strip::InitInstance();
        GPIO::LCD::InitInstance(GPIO::LineLength2004);
        Therma::Service::InitInstance();
    }
    catch (std::exception &exception)
    {
        ReportWarning("[Workspace] Exception: %s", exception.what());
    }
}

/**
 * @brief   Initialize MMPS pools.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Servus::initializeMMPS()
{
    this->http->mmps = MMPS::InitPool(1);
    if (this->http->mmps == NULL)
    {
        ReportError("[Workspace] Cannot create MMPS pool");

        throw std::runtime_error("MMPS");
    }

    int rc;

    rc = MMPS::InitBank(
            this->http->mmps,
            0,
            1024,
            0,
            1000);
    if (rc != 0)
    {
        ReportError("[Workspace] Cannot create MMPS bank: rc=%d", rc);

        throw std::runtime_error("MMPS");
    }

    rc = MMPS::AllocateImmediately(
            this->http->mmps,
            0);
    if (rc != 0)
    {
        ReportError("[Workspace] Cannot allocate memory for MMPS bank: rc=%d", rc);

        throw std::runtime_error("MMPS");
    }
}

/**
 * @brief   1st part of application kernel - initialize all resources.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Servus::kernelInit()
{
    // Start GKrellM service.
    //
    {
        int rc = GKrellM_Start(&this->gkrellm);
        if (rc != 0)
        {
            ReportError("[Workspace] Cannot start GKrellM service - quit");

            throw std::runtime_error("Cannot start GKrellM service");
        }
    }

    this->modbus = new MODBUS::Service(Configuration::MODBUSPortNumber);

    try
    {
        Config config;
        config.readFile("/opt/servus/servus.conf");

        Setting &gpio = config.lookup("GPIO");

        Setting &relays = gpio.lookup("Relays");

        {
            GPIO::RelayStation &relayStation = GPIO::RelayStation::SharedInstance();

            for (int relayIndex = 0;
                 relayIndex < relays.getLength();
                 relayIndex++)
            {
                Setting &relay = relays[relayIndex];

                const char      *relayName = relay.lookup("Name");
                unsigned int    pinNumber = relay.lookup("GPIO");

                relayStation += new GPIO::Relay(pinNumber, relayName);
            }
        }

        Setting &therma = gpio.lookup("Therma");

        {
            Therma::Service &thermaService = Therma::Service::SharedInstance();

            for (int sensorIndex = 0;
                 sensorIndex < therma.getLength();
                 sensorIndex++)
            {
                Setting &sensor = therma[sensorIndex];

                const char      *thermaId = sensor.lookup("Id");
                const char      *thermaName = sensor.lookup("Name");
                unsigned int    modbusUnitId = sensor.lookup("MODBUS");

                thermaService += new Therma::Sensor(thermaId, thermaName, modbusUnitId);
            }
        }
    }
    catch (std::exception &exception)
    {
        ReportWarning("[Workspace] Exception on configuration: %s", exception.what());
    }

    // Start HTTP service.
    //
    {
        this->http = new HTTP::Service(new WWW::Site(),
                "HTTP",
                Communicator::IPv4,
                9000);

        this->initializeMMPS();
    }

    Signals::SetSignalCaptureOn(SIGINT, OwnSignalHandler);
    Signals::SetSignalCaptureOn(SIGTERM, OwnSignalHandler);
}

const unsigned int rows[4] = { 05, 06, 12, 13 }; //{ 29, 31, 32, 33 };
const unsigned int cols[4] = { 16, 17, 18, 19 }; //{ 36, 11, 12, 35 };

/**
 * @brief   2nd part of application kernel - start all subcomponents.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Servus::kernelExec()
{
#if 0
    {
        ReportInfo("ZZZ");

        unsigned int row, col, value;

        for (;;)
        {
            usleep(200000);

            for (col = 0; col < 4; col++)
            {
                GPIO_Unexport(cols[col]);
                GPIO_Export(cols[col]);
                GPIO_Direction(cols[col], GPIO_OUT);
                GPIO_Set(cols[col], GPIO_LOW);
            }

            for (row = 0; row < 4; row++)
            {
                GPIO_Unexport(rows[row]);
                GPIO_Export(rows[row]);
                GPIO_Direction(rows[row], GPIO_IN);
                GPIO_Edge(rows[row], GPIO_EDGE_FALLING);
            }

            for (row = 0; row < 4; row++)
            {
                GPIO_Get(rows[row], &value);
                if (value == GPIO_LOW)
                {
                    ReportInfo("ZZZ row=%u", row);
                    break;
                }
            }

            if (row == 4)
                continue;

            for (col = 0; col < 4; col++)
            {
                GPIO_Direction(cols[col], GPIO_IN);
                //GPIO_Edge(cols[col], GPIO_EDGE_RISING);
            }

            //GPIO_Direction(rows[row], GPIO_OUT);
            GPIO_Set(rows[row], GPIO_HIGH);

            for (col = 0; col < 4; col++)
            {
                GPIO_Get(cols[col], &value);
                if (value == GPIO_HIGH)
                {
                    ReportInfo("ZZZ col=%u", col);
                    break;
                }
            }
        }
    }
#endif

    try
    {
        GPIO::Strip &stripService = GPIO::Strip::SharedInstance();

        stripService.startService();

        Therma::Service &thermaService = Therma::Service::SharedInstance();

        thermaService.startService();

        this->http->startService();
    }
    catch (...)
    {
        ReportError("[Workspace] Error has occurred starting services - quit!");
    }
}

/**
 * @brief   3rd part of application kernel - waiting for completion of services.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Servus::kernelWait()
{
    try
    {
        this->http->waitForService();
    }
    catch (...)
    {
        ReportError("[Workspace] Error has occurred waiting for services - quit!");
    }
}

/**
 * @brief   4th part of application kernel - release all resources.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Servus::kernelDone()
{
    Signals::SetSignalCaptureOff(SIGINT);
    Signals::SetSignalCaptureOff(SIGTERM);

    try
    {
        this->http->stopService();
    }
    catch (...)
    {
        ReportError("[Workspace] Error has occurred trying to stop services");
    }
}

/**
 * @brief   POSIX signal handler.
 *
 * Handle POSIX process signals to let procurator release all resources.
 *
 * @param   signalNumber    POSIX signal number.
 */
static void
OwnSignalHandler(int signalNumber)
{
    ReportNotice("[Workspace] Received signal to quit: signal=%d", signalNumber);

    // Raise signal again to let it be handled by default signal handler.
    //
    raise(SIGHUP);
}
