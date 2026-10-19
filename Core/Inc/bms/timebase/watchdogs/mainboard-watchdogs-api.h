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

#include "mainboard-watchdogs.h"
#include "watchdogs.h"

#ifdef CONF_WATCHDOG_MODULE_ENABLE

/*!
 * \brief Initialize the mainboard watchdogs
 * \param tick_callback The function that returns the current tick
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_init(watchdog_tick_callback tick_callback);

/*!
 * \brief Run the mainboard watchdogs routine, executes the callback of the timed out watchdogs
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_routine(void);

/*!
 * \brief Initialize a watchdog
 * \param watchdog A pointer to the watchdog
 * \param timeout The number of ticks that should elapse for the watchdog to time-out
 * \param callback The function that is called when the watchdog times-out
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_init_watchdog(struct Watchdog *watchdog, uint32_t timeout, watchdog_timeout_callback callback);

/*!
 * \brief Start a watchdog, a timed out watchdog cannot be started
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_start(struct Watchdog *watchdog);

/*!
 * \brief Stop a watchdog, a timed out watchdog cannot be stopped
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_stop(struct Watchdog *watchdog);

/*!
 * \brief Restart a watchdog no matter its state
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_restart(struct Watchdog *watchdog);

/*!
 * \brief Reset a watchdog to the not running state without executing its callback
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_reset(struct Watchdog *watchdog);

/*!
 * \brief Replenish the internal time of a running watchdog
 * \param watchdog A pointer to the watchdog
 * \return The return code of the operation
 */
enum MainboardWatchdogsReturnCode mainboard_watchdogs_pet(struct Watchdog *watchdog);

/*!
 * \brief Check if a watchdog is running
 * \param watchdog A pointer to the watchdog
 * \return True if the watchdog is running, false otherwise
 */
bool mainboard_watchdogs_is_running(struct Watchdog *watchdog);

/*!
 * \brief Check if a watchdog has timed out
 * \param watchdog A pointer to the watchdog
 * \return True if the watchdog has timed out, false otherwise
 */
bool mainboard_watchdogs_is_timed_out(struct Watchdog *watchdog);

#else

#define mainboard_watchdogs_init(tick_callback) (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_routine() (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_init_watchdog(watchdog, timeout, callback) (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_start(watchdog) (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_stop(watchdog) (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_restart(watchdog) (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_reset(watchdog) (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_pet(watchdog) (MAINBOARD_WATCHDOGS_RC_OK)
#define mainboard_watchdogs_is_running(watchdog) (false)
#define mainboard_watchdogs_is_timed_out(watchdog) (false)

#endif // CONF_WATCHDOG_MODULE_ENABLE
#endif // MAINBOARD_WATCHDOGS_API_H
