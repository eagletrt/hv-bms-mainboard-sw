/*!
 * \file mainboard-watchdogs-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Single pool of watchdogs shared by the whole firmware
 */

#include "mainboard-watchdogs-api.h"

#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-status-api.h"
#include "watchdogs-api.h"

EAGLETRT_STATIC struct WatchdogHandler mainboard_watchdogs_handler;

/*!
 * \brief Latch the watchdogs fault if a return code is an error
 *
 * \details BUSY, NOT_RUNNING and TIMED_OUT describe the state of the watchdog
 *          and are left to the caller, every other code that is not OK is an
 *          error of the pool or of its usage
 *
 * \param[in] return_code The return code of a watchdog function
 *
 * \return The same return code, unchanged
 */
EAGLETRT_STATIC enum WatchdogReturnCode prv_check(const enum WatchdogReturnCode return_code) {
    switch (return_code) {
        case WATCHDOG_RC_OK:
        case WATCHDOG_RC_BUSY:
        case WATCHDOG_RC_NOT_RUNNING:
        case WATCHDOG_RC_TIMED_OUT:
            break;

        case WATCHDOG_RC_NULL_POINTER:
        case WATCHDOG_RC_UNINITIALIZED:
        case WATCHDOG_RC_TEMPORAL_DISCONTINUITY:
        case WATCHDOG_RC_ERROR:
        default:
            EAGLETRT_API_UNUSED(mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_WATCHDOGS));
            break;
    }

    return return_code;
}

enum WatchdogReturnCode mainboard_watchdogs_api_init(const mainboard_watchdogs_tick_callback get_tick) {
    return prv_check(watchdogs_api_init_pool(&mainboard_watchdogs_handler, get_tick));
}

enum WatchdogReturnCode mainboard_watchdogs_api_routine(void) {
    return prv_check(watchdogs_api_routine(&mainboard_watchdogs_handler));
}

enum WatchdogReturnCode mainboard_watchdogs_api_init_watchdog(struct Watchdog *const watchdog,
                                                              const milliseconds timeout,
                                                              const watchdog_timeout_callback callback,
                                                              void *const context) {
    return prv_check(watchdogs_api_init_watchdog(watchdog, timeout, callback, context));
}

enum WatchdogReturnCode mainboard_watchdogs_api_start(struct Watchdog *const watchdog) {
    return prv_check(watchdogs_api_watchdog_start(&mainboard_watchdogs_handler, watchdog));
}

enum WatchdogReturnCode mainboard_watchdogs_api_stop(struct Watchdog *const watchdog) {
    return prv_check(watchdogs_api_watchdog_stop(&mainboard_watchdogs_handler, watchdog));
}

enum WatchdogReturnCode mainboard_watchdogs_api_restart(struct Watchdog *const watchdog) {
    return prv_check(watchdogs_api_watchdog_restart(&mainboard_watchdogs_handler, watchdog));
}

enum WatchdogReturnCode mainboard_watchdogs_api_reset(struct Watchdog *const watchdog) {
    return prv_check(watchdogs_api_watchdog_reset(&mainboard_watchdogs_handler, watchdog));
}

enum WatchdogReturnCode mainboard_watchdogs_api_pet(struct Watchdog *const watchdog) {
    return prv_check(watchdogs_api_watchdog_pet(&mainboard_watchdogs_handler, watchdog));
}

enum WatchdogReturnCode mainboard_watchdogs_api_feed(struct Watchdog *const watchdog) {
    return prv_check(watchdogs_api_watchdog_feed(&mainboard_watchdogs_handler, watchdog));
}

bool mainboard_watchdogs_api_is_running(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_is_running(watchdog);
}

bool mainboard_watchdogs_api_is_timed_out(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_is_timed_out(watchdog);
}
