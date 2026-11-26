/*!
 * \file mainboard-logger-api.h
 * \date 2026-11-25
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Timestamped logger and command driven serial console
 *
 * \details Thin wrapper around liblog: every message is prefixed with the
 *          time elapsed since boot, laid out in columns and sent through the
 *          write callback given at initialization (e.g. the non-blocking USART)
 *
 *          Output format:
 *          \code
 *               1.234  INFO   pcu-api.c:42                        precharge started
 *          \endcode
 *
 *          Usage:
 *          \code
 *          MAINBOARD_LOGGER_INFO("precharge started, Vbus=%u", vbus);
 *          \endcode
 *
 *          The same serial line is also a console: every key typed is a
 *          command that prints a report (voltages, temperatures, relays, ...)
 *          or acts on the car (TS on/off, balancing), press 'h' for the list.
 *          The reports are printed once per key press, paced line by line by
 *          mainboard_logger_api_routine so they never fill the output and the
 *          live logs keep flowing
 *
 * \note The source file name is taken from __FILE_NAME__ (GCC >= 12, clang),
 *       so the logs of a function defined in a header show the header name
 *
 * \attention liblog formats every message in a single static buffer, so the
 *            macros must never be called from an interrupt
 * \attention A message longer than LOGGER_EMIT_BUFFER_SIZE (header included)
 *            is dropped and LOGGER_RC_TRANSMISSION_ERROR is returned
 */

#ifndef MAINBOARD_LOGGER_API_H
#define MAINBOARD_LOGGER_API_H

#include <stdint.h>

#include "logger-api.h"
#include "mainboard-logger.h"

/*!
 * \defgroup mainboard_logger_macros Log a message with the given level
 *
 * \details The message is tagged with the name and line of the calling source file
 *
 * \param fmt The printf-like format string, followed by its arguments
 *
 * \returns The liblog return code, it can be ignored
 * \{
 */
#define MAINBOARD_LOGGER_INFO(fmt, ...) LOGGER_API_INFO(NULL, __FILE_NAME__, fmt, ##__VA_ARGS__)
#define MAINBOARD_LOGGER_WARNING(fmt, ...) LOGGER_API_WARNING(NULL, __FILE_NAME__, fmt, ##__VA_ARGS__)
#define MAINBOARD_LOGGER_ERROR(fmt, ...) LOGGER_API_ERROR(NULL, __FILE_NAME__, fmt, ##__VA_ARGS__)
#define MAINBOARD_LOGGER_DEBUG(fmt, ...) LOGGER_API_DEBUG(NULL, __FILE_NAME__, fmt, ##__VA_ARGS__)
/*! \} */

/*!
 * \brief Initialize the logger and print the welcome banner
 *
 * \details Must be called before any other module so that their initialization
 *          can be logged; messages emitted before this call are discarded
 *
 * \param data The callbacks and the verbosity, see \ref MainboardLoggerInitData
 *
 * \retval LOGGER_RC_NULL_POINTER if data or one of its callbacks is NULL
 * \retval LOGGER_RC_OK otherwise
 */
enum LoggerReturnCode mainboard_logger_api_init(const struct MainboardLoggerInitData *data);

/*!
 * \brief Change the enabled log levels
 *
 * \details Also leaves the quiet mode of the console
 *
 * \param verbosity The enabled levels as a bit flag of LoggerLevel values
 */
void mainboard_logger_api_set_verbosity(uint8_t verbosity);

/*!
 * \brief Run the console: read the typed commands and send the next lines of the report being printed
 *
 * \details Call it periodically from the main loop, never from an interrupt
 */
void mainboard_logger_api_routine(void);

/*!
 * \brief Execute a console command as if its key was typed
 *
 * \param key The key of the command, case insensitive; unknown keys print a hint
 */
void mainboard_logger_api_handle_command(char key);

#endif // MAINBOARD_LOGGER_API_H
