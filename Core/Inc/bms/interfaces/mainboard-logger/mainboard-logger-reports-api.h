/*!
 * \file mainboard-logger-reports-api.h
 * \date 2026-11-26
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Reports printed by the logger console
 *
 * \note Internal to the mainboard logger, the other modules use mainboard-logger-api.h
 */

#ifndef MAINBOARD_LOGGER_REPORTS_API_H
#define MAINBOARD_LOGGER_REPORTS_API_H

#include <stdbool.h>
#include <stddef.h>

#include "mainboard-logger.h"

/*!
 * \brief Get the description of a report
 *
 * \param report The report
 *
 * \returns The description, NULL if the report does not exist
 */
const struct MainboardLoggerReportInfo *int_mainboard_logger_reports_get(enum MainboardLoggerReport report);

/*!
 * \brief Append formatted text to a line, the text that does not fit is cut
 *
 * \param line The line to append to
 * \param fmt The printf-like format string, followed by its arguments
 */
[[gnu::format(printf, 2, 3)]]
void int_mainboard_logger_line_append(struct MainboardLoggerLine *line, const char *fmt, ...);

/*!
 * \brief Fill a row of the help report, implemented by the console
 *
 * \param row The index of the row
 * \param line[out] The line to fill
 *
 * \returns true if the line was filled, false if the help has no more rows
 */
bool int_mainboard_logger_api_help_row(size_t row, struct MainboardLoggerLine *line);

#endif // MAINBOARD_LOGGER_REPORTS_API_H
