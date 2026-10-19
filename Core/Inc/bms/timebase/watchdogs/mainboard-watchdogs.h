/*!
 * \file mainboard-watchdogs.h
 * \date 2026-10-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Types used by the mainboard watchdogs module
 */
#ifndef MAINBOARD_WATCHDOGS_H
#define MAINBOARD_WATCHDOGS_H

#include "mainboard-conf.h"

#ifdef CONF_WATCHDOG_MODULE_ENABLE

/*!
 * \brief Return code for the mainboard watchdogs functions
 */
enum MainboardWatchdogsReturnCode {
    MAINBOARD_WATCHDOGS_RC_OK = 0,
    MAINBOARD_WATCHDOGS_RC_NULL_POINTER,
    MAINBOARD_WATCHDOGS_RC_TIMED_OUT,
    MAINBOARD_WATCHDOGS_RC_TEMPORAL_DISCONTINUITY,
    MAINBOARD_WATCHDOGS_RC_BUSY,
    MAINBOARD_WATCHDOGS_RC_NOT_RUNNING,
    MAINBOARD_WATCHDOGS_RC_UNINITIALIZED,
    MAINBOARD_WATCHDOGS_RC_ERROR
};

#endif // CONF_WATCHDOG_MODULE_ENABLE
#endif // MAINBOARD_WATCHDOGS_H
