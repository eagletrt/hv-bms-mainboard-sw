/*!
 * \file mainboard-watchdogs-api.h
 * \date 2026-10-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief API for the mainboard watchdogs
 */

#ifndef MAINBOARD_WATCHDOGS_API_H
#define MAINBOARD_WATCHDOGS_API_H

#include <stdbool.h>
#include <stdint.h>

#include "mainboard-conf.h"
#include "watchdogs.h"

#ifdef CONF_WATCHDOG_MODULE_ENABLE

/*!
 * \brief Initialize the mainboard watchdogs
 * \param tick_callback The function that returns the current tick
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_init(watchdog_tick_callback tick_callback);

/*!
 * \brief Run the mainboard watchdogs routine, executes the callback of the timed out watchdogs
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_routine(void);

/*!
 * \brief Initialize a watchdog
 * \param watchdog A pointer to the watchdog
 * \param timeout The number of ticks that should elapse for the watchdog to time-out
 * \param callback The function that is called when the watchdog times-out
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_init_watchdog(struct Watchdog *watchdog, uint32_t timeout, watchdog_timeout_callback callback);

/*!
 * \brief Start a watchdog, a timed out watchdog cannot be started
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_start(struct Watchdog *watchdog);

/*!
 * \brief Stop a watchdog, a timed out watchdog cannot be stopped
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_stop(struct Watchdog *watchdog);

/*!
 * \brief Restart a watchdog no matter its state
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_restart(struct Watchdog *watchdog);

/*!
 * \brief Reset a watchdog to the not running state without executing its callback
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_reset(struct Watchdog *watchdog);

/*!
 * \brief Replenish the internal time of a running watchdog
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_pet(struct Watchdog *watchdog);

/*!
 * \brief Replenish a watchdog that is used as a liveness check of an external source
 *
 * \details A running watchdog is petted, a timed out one is restarted so that it can detect the next
 *          loss of the source, a stopped one is left stopped
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum WatchdogReturnCode mainboard_watchdogs_api_feed(struct Watchdog *watchdog);

/*!
 * \brief Check if a watchdog is running
 * \param watchdog A pointer to the watchdog
 * \return True if the watchdog is running, false otherwise
 */
bool mainboard_watchdogs_api_is_running(struct Watchdog *watchdog);

/*!
 * \brief Check if a watchdog has timed out
 * \param watchdog A pointer to the watchdog
 * \return True if the watchdog has timed out, false otherwise
 */
bool mainboard_watchdogs_api_is_timed_out(struct Watchdog *watchdog);

#else

#define mainboard_watchdogs_api_init(tick_callback) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_routine() (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_init_watchdog(watchdog, timeout, callback) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_start(watchdog) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_stop(watchdog) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_restart(watchdog) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_reset(watchdog) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_pet(watchdog) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_feed(watchdog) (WATCHDOG_RC_OK)
#define mainboard_watchdogs_api_is_running(watchdog) (false)
#define mainboard_watchdogs_api_is_timed_out(watchdog) (false)

#endif // CONF_WATCHDOG_MODULE_ENABLE
#endif // MAINBOARD_WATCHDOGS_API_H
