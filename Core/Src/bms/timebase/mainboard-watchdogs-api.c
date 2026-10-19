/*!
 * \file mainboard-watchdogs-api.c
 * \date 2026-10-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Implementation of the mainboard watchdogs API
 */

#include "mainboard-watchdogs-api.h"

#include "watchdogs-api.h"

#ifdef CONF_WATCHDOG_MODULE_ENABLE

static struct WatchdogHandler watchdogs_handler;

/*!
 * \brief Convert a return code of the watchdogs library to the mainboard one
 * \param rc The return code of the watchdogs library
 * \return The corresponding mainboard return code
 */
static enum MainboardWatchdogsReturnCode prv_watchdogs_convert_rc(const enum WatchdogReturnCode rc) {
    switch (rc) {
        case WATCHDOG_RC_OK:
            return MAINBOARD_WATCHDOGS_RC_OK;
        case WATCHDOG_RC_NULL_POINTER:
            return MAINBOARD_WATCHDOGS_RC_NULL_POINTER;
        case WATCHDOG_RC_TIMED_OUT:
            return MAINBOARD_WATCHDOGS_RC_TIMED_OUT;
        case WATCHDOG_RC_TEMPORAL_DISCONTINUITY:
            return MAINBOARD_WATCHDOGS_RC_TEMPORAL_DISCONTINUITY;
        case WATCHDOG_RC_BUSY:
            return MAINBOARD_WATCHDOGS_RC_BUSY;
        case WATCHDOG_RC_NOT_RUNNING:
            return MAINBOARD_WATCHDOGS_RC_NOT_RUNNING;
        case WATCHDOG_RC_UNINITIALIZED:
            return MAINBOARD_WATCHDOGS_RC_UNINITIALIZED;
        case WATCHDOG_RC_ERROR:
        default:
            return MAINBOARD_WATCHDOGS_RC_ERROR;
    }
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_init(const watchdog_tick_callback tick_callback) {
    return prv_watchdogs_convert_rc(watchdogs_api_init_pool(&watchdogs_handler, tick_callback));
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_routine(void) {
    return prv_watchdogs_convert_rc(watchdogs_api_routine(&watchdogs_handler));
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_init_watchdog(
    struct Watchdog *const watchdog,
    const uint32_t timeout,
    const watchdog_timeout_callback callback) {
    return prv_watchdogs_convert_rc(watchdogs_api_init_watchdog(watchdog, timeout, callback));
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_start(struct Watchdog *const watchdog) {
    return prv_watchdogs_convert_rc(watchdogs_api_watchdog_start(&watchdogs_handler, watchdog));
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_stop(struct Watchdog *const watchdog) {
    return prv_watchdogs_convert_rc(watchdogs_api_watchdog_stop(&watchdogs_handler, watchdog));
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_restart(struct Watchdog *const watchdog) {
    return prv_watchdogs_convert_rc(watchdogs_api_watchdog_restart(&watchdogs_handler, watchdog));
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_reset(struct Watchdog *const watchdog) {
    return prv_watchdogs_convert_rc(watchdogs_api_watchdog_reset(&watchdogs_handler, watchdog));
}

enum MainboardWatchdogsReturnCode mainboard_watchdogs_pet(struct Watchdog *const watchdog) {
    return prv_watchdogs_convert_rc(watchdogs_api_watchdog_pet(&watchdogs_handler, watchdog));
}

bool mainboard_watchdogs_is_running(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_is_running(watchdog);
}

bool mainboard_watchdogs_is_timed_out(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_is_timed_out(watchdog);
}

#endif // CONF_WATCHDOG_MODULE_ENABLE
