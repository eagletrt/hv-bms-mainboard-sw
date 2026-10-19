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

enum WatchdogReturnCode mainboard_watchdogs_api_init(const watchdog_tick_callback tick_callback) {
    return watchdogs_api_init_pool(&watchdogs_handler, tick_callback);
}

enum WatchdogReturnCode mainboard_watchdogs_api_routine(void) {
    return watchdogs_api_routine(&watchdogs_handler);
}

enum WatchdogReturnCode mainboard_watchdogs_api_init_watchdog(
    struct Watchdog *const watchdog,
    const uint32_t timeout,
    const watchdog_timeout_callback callback) {
    return watchdogs_api_init_watchdog(watchdog, timeout, callback);
}

enum WatchdogReturnCode mainboard_watchdogs_api_start(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_start(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_stop(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_stop(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_restart(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_restart(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_reset(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_reset(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode mainboard_watchdogs_api_pet(struct Watchdog *const watchdog) {
    return watchdogs_api_watchdog_pet(&watchdogs_handler, watchdog);
}
#endif // CONF_WATCHDOG_MODULE_ENABLE
