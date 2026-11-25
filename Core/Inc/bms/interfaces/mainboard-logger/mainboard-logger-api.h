/*!
 * \file mainboard-logger-api.h
 * \date 2026-11-25
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Timestamped logger shared by the whole firmware
 *
 * \details Thin wrapper around liblog: every message is prefixed with the
 *          time elapsed since boot and sent through the write callback
 *          given at initialization (e.g. the non-blocking USART)
 *
 *          Output format:
 *          \code
 *          [      1.234] [INFO ] @POST:42 >> message
 *          \endcode
 *
 *          Usage:
 *          \code
 *          #define MODULE_TAG "PCU"
 *          MAINBOARD_LOGGER_INFO(MODULE_TAG, "precharge started, Vbus=%u", vbus);
 *          \endcode
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
 * \param tag A non-empty string identifying the module
 * \param fmt The printf-like format string, followed by its arguments
 *
 * \returns The liblog return code, it can be ignored
 * \{
 */
#define MAINBOARD_LOGGER_INFO(tag, fmt, ...) LOGGER_API_INFO(NULL, (tag), fmt, ##__VA_ARGS__)
#define MAINBOARD_LOGGER_WARNING(tag, fmt, ...) LOGGER_API_WARNING(NULL, (tag), fmt, ##__VA_ARGS__)
#define MAINBOARD_LOGGER_ERROR(tag, fmt, ...) LOGGER_API_ERROR(NULL, (tag), fmt, ##__VA_ARGS__)
#define MAINBOARD_LOGGER_DEBUG(tag, fmt, ...) LOGGER_API_DEBUG(NULL, (tag), fmt, ##__VA_ARGS__)
/*! \} */

/*!
 * \brief Initialize the logger
 *
 * \details Must be called before any other module so that their initialization
 *          can be logged; messages emitted before this call are discarded
 *
 * \param write The callback used to send a full message, it must copy the data
 *              since the buffer is reused for the next message
 * \param get_tick The callback used to get the current time in ms
 * \param verbosity The enabled levels as a bit flag of LoggerLevel values
 *                  (e.g. LOGGER_ENABLE_ALL)
 *
 * \retval LOGGER_RC_NULL_POINTER if write or get_tick are NULL
 * \retval LOGGER_RC_OK otherwise
 */
enum LoggerReturnCode mainboard_logger_api_init(logger_write_callback write,
                                                mainboard_logger_tick_callback get_tick,
                                                uint8_t verbosity);

/*!
 * \brief Change the enabled log levels
 *
 * \param verbosity The enabled levels as a bit flag of LoggerLevel values
 */
void mainboard_logger_api_set_verbosity(uint8_t verbosity);

#endif // MAINBOARD_LOGGER_API_H
