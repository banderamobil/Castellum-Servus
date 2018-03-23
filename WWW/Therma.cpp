// Commmon definition files.
//
#include "GPIO/Therma.h"
#include "HTTP/Connection.h"
#include "HTTP/HTML.h"
#include "HTTP/HTTP.h"

// Local definition files.
//
#include "Servus/WWW/Home.h"

/**
 * @brief   Generate HTML page for the 'Therma' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void
WWW::Site::generateTherma(HTTP::Connection &connection, HTML::Instance &instance)
{
    HTML::Division division(instance, NULL, "workspace");

    division.meta("refresh", "10");

    { // HTML.Division
        HTML::Division division(instance, "full", "slice");

        { // HTML.HeadingText
            HTML::HeadingText headingText(instance, HTML::H2, HTML::Left);

            headingText.plain("Therma");
        } // HTML.HeadingText

        {
            HTML::Table table(instance);

            {
                HTML::TableHeader tableHeader(instance);

                {
                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance);

                        tableDataCell.plain("Sensor");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "centered");

                        tableDataCell.plain("Tief");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "centered");

                        tableDataCell.plain("Delta");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "centered");

                        tableDataCell.plain("Aktuell");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "centered");

                        tableDataCell.plain("Delta");
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "centered");

                        tableDataCell.plain("Hoch");
                    }
                }
            }

            Therma::Service &thermaService = Therma::Service::SharedInstance();

            {
                HTML::TableBody tableBody(instance);

                for (unsigned int sensorIndex = 0;
                     sensorIndex < thermaService.size();
                     sensorIndex++)
                {
                    Therma::Sensor *sensor = thermaService[sensorIndex];

                    HTML::TableRow tableRow(instance);

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "label");

                        tableDataCell.plain(sensor->name);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "blue");

                        tableDataCell.plain("%4.2f &#x2103;",
                                sensor->temperature.lowest);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "blue");

                        tableDataCell.plain("[-%4.2f &#x2103;]",
                                sensor->temperature.current - sensor->temperature.lowest);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "green");

                        tableDataCell.plain("%4.2f &#x2103;",
                                sensor->temperature.current);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "red");

                        tableDataCell.plain("[+%4.2f &#x2103;]",
                                sensor->temperature.highest - sensor->temperature.current);
                    }

                    {
                        HTML::TableDataCell tableDataCell(instance, NULL, "red");

                        tableDataCell.plain("%4.2f &#x2103;",
                                sensor->temperature.highest);
                    }
                }
            }
        }
    } // HTML.Division
}
#pragma GCC diagnostic pop
