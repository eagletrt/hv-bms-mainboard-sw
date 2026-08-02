/*!
 * \file watchdog.c
 * \date 2024-04-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Implementation of generic watchdogs that times-out a certain interval of time
 */

#include "watchdog.h"

#include <stddef.h>
#include <string.h>

#include "eagletrt-api.h"
#include "timebase.h"

#ifdef CONF_WATCHDOG_MODULE_ENABLE

/*! \brief Dummy function to avoid NULL dereferencing */
void prv_watchdog_timeout_dummy(void) {
}

enum WatchdogReturnCode watchdog_init(
    struct Watchdog *const watchdog,
    const ticks_t timeout,
    const watchdog_timeout_callback_t expire) {
    if (watchdog == NULL || expire == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    if (watchdog->running) {
        return WATCHDOG_RC_BUSY;
    }
    memset(watchdog, 0U, sizeof(struct Watchdog));
    // Init watchdog
    watchdog->timeout = timeout;
    watchdog->expire = expire;
    return WATCHDOG_RC_OK;
}

enum WatchdogReturnCode watchdog_deinit(struct Watchdog *const watchdog) {
    if (watchdog == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    (void)timebase_unregister_watchdog(watchdog);
    memset(watchdog, 0U, sizeof(struct Watchdog));
    watchdog->expire = prv_watchdog_timeout_dummy;
    return WATCHDOG_RC_OK;
}

enum WatchdogReturnCode watchdog_start(struct Watchdog *const watchdog) {
    if (watchdog == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    if (watchdog->running) {
        return WATCHDOG_RC_BUSY;
    }
    if (watchdog->timed_out) {
        return WATCHDOG_RC_TIMED_OUT;
    }
    if (timebase_register_watchdog(watchdog) == TIMEBASE_RC_WATCHDOG_UNAVAILABLE) {
        return WATCHDOG_RC_UNAVAILABLE;
    }
    watchdog->running = true;
    return WATCHDOG_RC_OK;
}

enum WatchdogReturnCode watchdog_stop(struct Watchdog *const watchdog) {
    if (watchdog == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    if (!watchdog->running) {
        return WATCHDOG_RC_NOT_RUNNING;
    }
    if (watchdog->timed_out) {
        return WATCHDOG_RC_TIMED_OUT;
    }
    EAGLETRT_API_UNUSED(timebase_unregister_watchdog(watchdog));
    watchdog->running = false;
    return WATCHDOG_RC_OK;
}

enum WatchdogReturnCode watchdog_restart(struct Watchdog *const watchdog) {
    if (watchdog == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    // Stop watchdog if its running
    if (watchdog->running) {
        (void)timebase_unregister_watchdog(watchdog);
        watchdog->running = false;
    }
    // Start the watchdog
    if (timebase_register_watchdog(watchdog) == TIMEBASE_RC_WATCHDOG_UNAVAILABLE) {
        return WATCHDOG_RC_UNAVAILABLE;
    }
    watchdog->timed_out = false;
    watchdog->running = true;
    return WATCHDOG_RC_OK;
}

enum WatchdogReturnCode watchdog_reset(struct Watchdog *const watchdog) {
    if (watchdog == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    if (watchdog->timed_out) {
        return WATCHDOG_RC_TIMED_OUT;
    }
    if (!watchdog->running) {
        return WATCHDOG_RC_NOT_RUNNING;
    }
    if (timebase_update_watchdog(watchdog) == TIMEBASE_RC_WATCHDOG_UNAVAILABLE) {
        return WATCHDOG_RC_UNAVAILABLE;
    }
    return WATCHDOG_RC_OK;
}

enum WatchdogReturnCode watchdog_timeout(struct Watchdog *const watchdog) {
    if (watchdog == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    if (!watchdog->running) {
        return WATCHDOG_RC_NOT_RUNNING;
    }
    if (watchdog->timed_out) {
        return WATCHDOG_RC_TIMED_OUT;
    }
    watchdog->timed_out = true;
    watchdog->running = false;
    watchdog->expire();
    return WATCHDOG_RC_OK;
}

bool watchdog_is_timed_out(struct Watchdog *const watchdog) {
    if (watchdog == NULL) {
        return false;
    }
    return watchdog->timed_out;
}

#else // CONF_WATCHDOG_MODULE_ENABLE

#ifdef CONF_WATCHDOG_STRINGS_ENABLE

EAGLETRT_STATIC char *watchdog_module_name = "watchdog";

EAGLETRT_STATIC char *watchdog_return_code_name[] = {
    [WATCHDOG_RC_OK] = "ok",
    [WATCHDOG_RC_NULL_POINTER] = "null pointer",
    [WATCHDOG_RC_BUSY] = "busy",
    [WATCHDOG_RC_TIMED_OUT] = "timeout",
    [WATCHDOG_RC_NOT_RUNNING] = "not running",
    [WATCHDOG_RC_UNAVAILABLE] = "unavailable"
};

EAGLETRT_STATIC char *watchdog_return_code_name[] = {
    [WATCHDOG_RC_OK] = "executed sucessfully",
    [WATCHDOG_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [WATCHDOG_RC_BUSY] = "the watchdog is already running",
    [WATCHDOG_RC_TIMED_OUT] = "the watchdog has timed-out",
    [WATCHDOG_RC_NOT_RUNNING] = "the watchdog is not running",
    [WATCHDOG_RC_UNAVAILABLE] = "the watchdog can't be registered"
};

#endif // CONF_WATCHDOG_STRINGS_ENABLE

#endif // CONF_WATCHDOG_MODULE_ENABLE
