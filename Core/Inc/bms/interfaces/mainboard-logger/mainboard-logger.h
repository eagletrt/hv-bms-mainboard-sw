/*!
 * \file mainboard-logger.h
 * \date 2026-11-25
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the mainboard logger
 */

#ifndef MAINBOARD_LOGGER_H
#define MAINBOARD_LOGGER_H

#include "logger.h"
#include "mainboard-def.h"

/*!
 * \brief Maximum length of the timestamp prepended to every message
 *
 * \details The format is "[sssssss.mmm] ", 14 characters at most for a 32 bit
 *          tick, plus the string terminator used by snprintf
 */
#define MAINBOARD_LOGGER_TIMESTAMP_SIZE (16U)

/*!
 * \brief Size of a full message, timestamp included
 */
#define MAINBOARD_LOGGER_MESSAGE_SIZE ((MAINBOARD_LOGGER_TIMESTAMP_SIZE) + (LOGGER_EMIT_BUFFER_SIZE))

/*!
 * \brief Callback used to get the current time
 *
 * \returns The time elapsed since boot in ms, it must be monotonic
 */
typedef milliseconds (*mainboard_logger_tick_callback)(void);

#endif // MAINBOARD_LOGGER_H
