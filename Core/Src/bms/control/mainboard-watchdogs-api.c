/*!
 * \file mainboard-watchdogs-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Single pool of watchdogs shared by the whole firmware
 */

#include "mainboard-watchdogs-api.h"

#include "eagletrt.h"
#include "watchdogs-api.h"

EAGLETRT_STATIC struct WatchdogHandler mainboard_watchdogs_handler;

enum WatchdogReturnCode mainboard_watchdogs_api_init(const mainboard_watchdogs_tick_callback get_tick) {
    return watchdogs_api_init_pool(&mainboard_watchdogs_handler, get_tick);
}

enum WatchdogReturnCode mainboard_watchdogs_api_routine(void) {
    return watchdogs_api_routine(&mainboard_watchdogs_handler);
}

enum WatchdogReturnCode mainboard_watchdogs_api_init_watchdog(struct Watchdog *const watchdog,
                                                              const milliseconds timeout,
                                                              const watchdog_timeout_callback callback,
                                                              void *const context) {
    return watchdogs_api_init_watchdog(watchdog, timeout, callback, context);
}

enum WatchdogReturnCode mainboard_watchdogs_api_start(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_start(&mainboard_watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_stop(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_stop(&mainboard_watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_restart(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_restart(&mainboard_watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_reset(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_reset(&mainboard_watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_pet(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_pet(&mainboard_watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_feed(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_feed(&mainboard_watchdogs_handler, watchdog);
}

bool mainboard_watchdogs_api_is_running(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_is_running(watchdog);
}

bool mainboard_watchdogs_api_is_timed_out(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_is_timed_out(watchdog);
}
