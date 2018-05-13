// Common definition files.
//
#include "GPIO/Relay.hpp"
#include "HTTP/Connection.hpp"
#include "HTTP/HTML.hpp"
#include "HTTP/HTTP.hpp"

// Local definition files.
//
#include "Servus/WWW/Home.hpp"

/**
 * @brief   Generate HTML page for the 'Relay' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
void
WWW::Site::generateRelay(HTTP::Connection &connection, HTML::Instance &instance)
{
    HTML::Division division(instance, NULL, "workspace");

    { // HTML.Division
        HTML::Division division(instance, "full", "slice");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("Relaisstation");
        } // HTML.HeadingText

        {
            HTML::Table table(instance);

            {
                HTML::TableHeader tableHeader(instance);

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Relais");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Stand");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("");
                    }
                }
            }

            GPIO::RelayStation &relayStation = GPIO::RelayStation::SharedInstance();

            {
                HTML::TableBody tableBody(instance);

                for (unsigned int relayIndex = 0;
                     relayIndex < relayStation.size();
                     relayIndex++)
                {
                    GPIO::Relay *relay = relayStation[relayIndex];

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "label");

                        tableDataCell.plain(relay->name);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL,
                                (relay->isOff()) ? "red" : "green");

                        tableDataCell.plain((relay->isOff()) ? "Aus" : "Ein");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%u&%s=%s",
                                    connection.pageName(),
                                    WWW::SwitchRelay,
                                    relayIndex,
                                    WWW::RelayState,
                                    WWW::RelayStateDown);

                            HTML::URL url(instance,
                                    urlString,
                                    NULL,
                                    "Schalte Relais aus.");

                            url.plain("[Aus]");
                        } // HTML.URL
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "action");

                        { // HTML.URL
                            char urlString[200];

                            snprintf(urlString, sizeof(urlString),
                                    "%s?%s=%u&%s=%s",
                                    connection.pageName(),
                                    WWW::SwitchRelay,
                                    relayIndex,
                                    WWW::RelayState,
                                    WWW::RelayStateUp);

                            HTML::URL url(instance,
                                    urlString,
                                    NULL,
                                    "Schalte Relais ein.");

                            url.plain("[Ein]");
                        } // HTML.URL
                    }
                }
            }
        }
    } // HTML.Division
}
