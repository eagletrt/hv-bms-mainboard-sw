/*!
 * \file mainboard-watchdogs-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Single pool of watchdogs shared by the whole firmware
 *
 * \details This module only owns the pool: the tick source, the scheduling and the routine.
 *          Every watchdog belongs to the module that uses it: that module declares the
 *          \c struct \c Watchdog, writes its timeout callback and decides what a timeout means
 *          (e.g. set one of its faults, count a dropped packet)
 *
 * \attention Every function that changes the pool (start, stop, restart, reset, pet, feed)
 *            must be called from the same context as \ref mainboard_watchdogs_api_routine,
 *            never from an interrupt
 * \attention The pool holds at most WATCHDOGS_MAX_COUNT running watchdogs
 */

#ifndef MAINBOARD_WATCHDOGS_API_H
#define MAINBOARD_WATCHDOGS_API_H

#include <stdbool.h>

#include "mainboard-watchdogs.h"
#include "watchdogs.h"

/*!
 * \brief Initialize the watchdog pool
 *
 * \param get_tick The callback used to get the current time in ms
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the callback is NULL
 * \retval WATCHDOG_RC_ERROR if the pool could not be created
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_init(mainboard_watchdogs_tick_callback get_tick);

/*!
 * \brief Call the callback of every watchdog that has timed out
 *
 * \details Must be called periodically, its period adds to the resolution of every timeout
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the module is not initialized
 * \retval WATCHDOG_RC_TEMPORAL_DISCONTINUITY if the tick went backwards
 * \retval WATCHDOG_RC_ERROR if the pool is corrupted
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_routine(void);

/*!
 * \brief Initialize a watchdog, it is not started
 *
 * \details A watchdog can be initialized only once: to initialize it again, zero it first
 *
 * \param watchdog The watchdog to initialize
 * \param timeout The time after which the watchdog times out, must be greater than 0
 * \param callback The function called when the watchdog times out
 * \param context Passed back unchanged to the callback, may be NULL
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog or the callback is NULL
 * \retval WATCHDOG_RC_ERROR if the watchdog is already initialized or the timeout is 0
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_init_watchdog(struct Watchdog *watchdog,
                                                              milliseconds timeout,
                                                              watchdog_timeout_callback callback,
                                                              void *context);

/*!
 * \brief Start a watchdog that is not running
 *
 * \details A timed out watchdog cannot be started, see \ref mainboard_watchdogs_api_restart
 *
 * \param watchdog The watchdog to start
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL or the module is not initialized
 * \retval WATCHDOG_RC_UNINITIALIZED if the watchdog is not initialized
 * \retval WATCHDOG_RC_BUSY if the watchdog is already running
 * \retval WATCHDOG_RC_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_RC_TEMPORAL_DISCONTINUITY if the tick went backwards
 * \retval WATCHDOG_RC_ERROR if the pool is full
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_start(struct Watchdog *watchdog);

/*!
 * \brief Stop a running watchdog without calling its callback
 *
 * \param watchdog The watchdog to stop
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL
 * \retval WATCHDOG_RC_UNINITIALIZED if the watchdog is not initialized
 * \retval WATCHDOG_RC_NOT_RUNNING if the watchdog is not running
 * \retval WATCHDOG_RC_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_stop(struct Watchdog *watchdog);

/*!
 * \brief Start a watchdog from now, whatever its state
 *
 * \param watchdog The watchdog to restart
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL or the module is not initialized
 * \retval WATCHDOG_RC_UNINITIALIZED if the watchdog is not initialized
 * \retval WATCHDOG_RC_TEMPORAL_DISCONTINUITY if the tick went backwards
 * \retval WATCHDOG_RC_ERROR if the pool is full
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_restart(struct Watchdog *watchdog);

/*!
 * \brief Bring a watchdog back to not running, whatever its state, without calling its callback
 *
 * \param watchdog The watchdog to reset
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL
 * \retval WATCHDOG_RC_UNINITIALIZED if the watchdog is not initialized
 * \retval WATCHDOG_RC_ERROR if the pool is corrupted
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_reset(struct Watchdog *watchdog);

/*!
 * \brief Push the timeout of a running watchdog to now + timeout
 *
 * \details A timed out watchdog stays timed out, use it for timeouts that must latch
 *
 * \param watchdog The watchdog to pet
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL or the module is not initialized
 * \retval WATCHDOG_RC_UNINITIALIZED if the watchdog is not initialized
 * \retval WATCHDOG_RC_NOT_RUNNING if the watchdog is not running
 * \retval WATCHDOG_RC_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_RC_TEMPORAL_DISCONTINUITY if the tick went backwards
 * \retval WATCHDOG_RC_ERROR if the pool is corrupted
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_pet(struct Watchdog *watchdog);

/*!
 * \brief Signal liveness: a running watchdog is petted, a timed out one is restarted
 *
 * \details A stopped watchdog stays stopped, use it for timeouts that recover
 *
 * \param watchdog The watchdog to feed
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL or the module is not initialized
 * \retval WATCHDOG_RC_UNINITIALIZED if the watchdog is not initialized
 * \retval WATCHDOG_RC_NOT_RUNNING if the watchdog is stopped
 * \retval WATCHDOG_RC_TEMPORAL_DISCONTINUITY if the tick went backwards
 * \retval WATCHDOG_RC_ERROR if the pool is full or corrupted
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode mainboard_watchdogs_api_feed(struct Watchdog *watchdog);

/*!
 * \brief Check if a watchdog is running
 *
 * \param watchdog The watchdog to check
 *
 * \returns true if the watchdog is running, false otherwise or if it is NULL
 */
bool mainboard_watchdogs_api_is_running(struct Watchdog *watchdog);

/*!
 * \brief Check if a watchdog has timed out
 *
 * \param watchdog The watchdog to check
 *
 * \returns true if the watchdog has timed out, false otherwise or if it is NULL
 */
bool mainboard_watchdogs_api_is_timed_out(struct Watchdog *watchdog);

#endif // MAINBOARD_WATCHDOGS_API_H
