/*!
 * \file mainboard-watchdogs.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the mainboard watchdogs
 */

#ifndef MAINBOARD_WATCHDOGS_H
#define MAINBOARD_WATCHDOGS_H

#include "mainboard-def.h"

/*!
 * \brief Callback used to get the current time
 *
 * \returns The time elapsed since boot in ms, it must be monotonic
 */
typedef milliseconds (*mainboard_watchdogs_tick_callback)(void);

#endif // MAINBOARD_WATCHDOGS_H
