/*!
 * \file mainboard-logger-api.c
 * \date 2026-11-25
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Timestamped logger shared by the whole firmware
 */

#include "mainboard-logger-api.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "eagletrt.h"

EAGLETRT_STATIC logger_write_callback mainboard_logger_write;
EAGLETRT_STATIC mainboard_logger_tick_callback mainboard_logger_get_tick;

/*!
 * \brief Write callback given to liblog, prepends the timestamp to the message
 *
 * \details The timestamp and the message are joined in a single buffer so that
 *          they are sent with one write and can not be split by other data
 *
 * \param msg The message formatted by liblog
 * \param size The length of msg in bytes
 *
 * \returns The value returned by the write callback, or -1 if the message
 *          does not fit in the buffer
 */
EAGLETRT_STATIC int prv_mainboard_logger_write(const char *const msg, const size_t size) {
    static char buffer[MAINBOARD_LOGGER_MESSAGE_SIZE];
    constexpr milliseconds ms_per_second = 1000U;

    const milliseconds now = mainboard_logger_get_tick();
    const int offset = snprintf(buffer,
                                MAINBOARD_LOGGER_TIMESTAMP_SIZE,
                                "[%7" PRIu32 ".%03" PRIu32 "] ",
                                (uint32_t)(now / ms_per_second),
                                (uint32_t)(now % ms_per_second));
    if (offset < 0 || (size_t)offset >= MAINBOARD_LOGGER_TIMESTAMP_SIZE || size > sizeof(buffer) - (size_t)offset) {
        return -1;
    }

    memcpy(buffer + offset, msg, size);
    return mainboard_logger_write(buffer, (size_t)offset + size);
}

enum LoggerReturnCode mainboard_logger_api_init(const logger_write_callback write,
                                                const mainboard_logger_tick_callback get_tick,
                                                const uint8_t verbosity) {
    if (write == NULL || get_tick == NULL) {
        return LOGGER_RC_NULL_POINTER;
    }
    mainboard_logger_write = write;
    mainboard_logger_get_tick = get_tick;
    return logger_api_init(NULL, prv_mainboard_logger_write, NULL, verbosity);
}

void mainboard_logger_api_set_verbosity(const uint8_t verbosity) {
    logger_api_set_verbosity(NULL, verbosity);
}
