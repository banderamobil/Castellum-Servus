// Common definition files.
//
#include "HTTP/Connection.h"
#include "HTTP/HTML.h"
#include "HTTP/HTTP.h"

// Local definition files.
//
#include "Servus/WWW/Home.h"

/**
 * @brief   Generate HTML page for the 'System Information' tab.
 *
 * @param   connection      HTTP connection.
 * @param   instance        HTML instance.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void
WWW::Site::generateSystemInformation(HTTP::Connection &connection, HTML::Instance &instance)
{
    HTML::Division division(instance, "full", "slice");

    {
        HTML::Table table(instance);

        {
            HTML::TableRow tableRow(instance);

            {
                HTML::TableDataCell tableDataCell(instance, NULL, "label");

                tableDataCell.plain("Software version:");
            }

            {
                HTML::TableDataCell tableDataCell(instance, NULL, "value");

                tableDataCell.plain("Value");
            }
        }
    }
}
#pragma GCC diagnostic pop
