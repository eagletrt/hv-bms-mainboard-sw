/*!
 * \file timebase.h
 * \date 2024-04-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Functions to manage periodic tasks at certain intervals
 */

#ifndef TIMEBASE_H
#define TIMEBASE_H

#include <stdint.h>
#include <stdbool.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"
#include "watchdog.h"
#include "tasks.h"
#include "min-heap.h"

/*!
 * \brief Convert the time in ms to ticks
 *
 * \param T The time to convert
 * \param RES The resolution of a tick
 *
 * \return ticks_t The corresponing amount of ticks
 */
#define TIMEBASE_TIME_TO_TICKS(T, RES) ((T) / (RES))

/*!
 * \brief Convert the ticks in ms
 *
 * \param T The ticks to convert
 * \param RES The resolution of a tick
 *
 * \return milliseconds_t The corresponing amount of ms
 */
#define TIMEBASE_TICKS_TO_TIME(T, RES) ((T) * (RES))

/*! \brief Maximum number of watchdogs that can be handled simultaneously */
#define TIMEBASE_RUNNING_WATCHDOG_COUNT (24U)

/*!
 * \brief Return code for the timebase module functions
 */
enum TimebaseReturnCode {
    TIMEBASE_RC_OK,                      /*!< The function executed successfully */
    TIMEBASE_RC_NULL_POINTER,            /*!< A NULL pointer was given to a function */
    TIMEBASE_RC_DISABLED,                /*!< The timebase is not running */
    TIMEBASE_RC_BUSY,                    /*!< The timebase cannot perform the current operation because is busy with other actions */
    TIMEBASE_RC_BUFFER_ERROR,            /*!< An error related to internal buffers of the scheduler */
    TIMEBASE_RC_TASK_ERROR,              /*!< An error occured in the task module */
    TIMEBASE_RC_WATCHDOG_NOT_REGISTERED, /*!< The watchdog is not registered inside the timebase */
    TIMEBASE_RC_WATCHDOG_UNAVAILABLE     /*!< The given watchdog can't be registered inside the timebase */
};

/*!
 * \brief Definition of a scheduled task that has to be executed at a certain time
 */
struct TimebaseScheduledTask {
    ticks_t t;         /*!< The time in which the task should be executed */
    struct Task *task; /*!< A pointer to the task to run */
};

/*!
 * \brief Definition of a scheduled watchdog
 */
struct TimebaseScheduledWatchdog {
    ticks_t t;                 /*!< The time in which the watchdog should timeout */
    struct Watchdog *watchdog; /*!< A pointer to the watchdog handler structure */
};

/*!
 * \brief Timebase handler structure
 *
 * \attention This structure should never be used outside of this file
 */
struct TimebaseHandler {
    bool enabled;                                                                                   /*!< True if the timebase is running, false otherwise */
    milliseconds_t resolution;                                                                      /*!< Number of ms that represent one tick */
    volatile ticks_t t;                                                                             /*!< The current number of ticks */
    MinHeap(struct TimebaseScheduledTask, TASKS_COUNT) scheduled_tasks;                             /*!< The heap of scheduled tasks that has to be executed */
    MinHeap(struct TimebaseScheduledWatchdog, TIMEBASE_RUNNING_WATCHDOG_COUNT) scheduled_watchdogs; /*!<  The heap of scheduled watchdogs that are currently running */
};

#ifdef CONF_TIMEBASE_MODULE_ENABLE

/*!
 * \brief Initialize the timebase handler
 *
 * \param resolution The amount of time that represent one tick (in ms)
 *
 * \retval TIMEBASE_RC_NULL_POINTER if a tasks is not implemented
 * \retval TIMEBASE_RC_BUFFER_ERROR failed internal buffers initialization
 * \retval TIMEBASE_RC_TASK_ERROR failed task initialization
 * \retval TIMEBASE_RC_OK otherwise
 */
enum TimebaseReturnCode timebase_init(milliseconds_t resolution_ms);

/*!
 * \brief Enable or disable the timebase
 *
 * \param enabled True to enable the timebase false to disable it
 */
void timebase_set_enable(bool enabled);

/*!
 * \brief Increment the internal timebase by one tick
 *
 * \retval TIMEBASE_RC_DISABLED if the timebase is disabled
 * \retval TIMEBASE_RC_OK otherwise
 */
enum TimebaseReturnCode timebase_inc_tick(void);

/*!
 * \brief Get the current number of ticks
 *
 * \return The number of ticks
 */
ticks_t timebase_get_tick(void);

/*!
 * \brief Get the current elapsed time in ms
 *
 * \return The current elapsed time
 */
milliseconds_t timebase_get_time(void);

/*!
 * \brief Get the number of ms that represents a single tick
 *
 * \return The timebase resolution
 */
milliseconds_t timebase_get_resolution(void);

/*!
 * \brief Register a watchdog into the timebase
 *
 * \details If the timebase is not enable the watchdog is started but not updated
 *
 * \param Watchdog A pointer to the watchdog handler structure
 *
 * \retval TIMEBASE_RC_NULL_POINTER if the watchdog is NULL
 * \retval TIMEBASE_RC_BUSY if the watchdog is already running
 * \retval TIMEBASE_RC_WATCHDOG_UNAVAILABLE if the timebase can't handle the watchdog
 * \retval TIMEBASE_RC_OK otherwise
 */
enum TimebaseReturnCode timebase_register_watchdog(struct Watchdog *watchdog);

/*!
 * \brief Unregister a watchdog from the timebase
 *
 * \details The watchdog is unregistered even if the timebase is not enabled
 *
 * \param watchdog The watchdog handler structure
 *
 * \retval TIMEBASE_RC_NULL_POINTER if the watchdog pointer is NULL
 * \retval TIMEBASE_RC_WATCHDOG_NOT_REGISTERED the watchdog is not registered
 * \retval TIMEBASE_RC_OK otherwise
 */
enum TimebaseReturnCode timebase_unregister_watchdog(struct Watchdog *watchdog);

/*!
 * \brief Check if the watchdog is registered into the timebase
 *
 * \details A NULL pointer is counted as NOT registered
 *
 * \param watchdog A pointer to the watchdog to check
 *
 * \return True if the watchdog is registered, false otherwise
 */
bool timebase_is_registered_watchdog(struct Watchdog *watchdog);

/*!
 * \brief Update the registered watchdog
 *
 * \attention During the update process the watchdog is unregistered and than registered again
 * if the function returns TIMEBASE_RC_WATCHDOG_UNAVAILABLE that means that the watchdog was
 * unregistered but cannot be registered back (i.e. is not running anymore)
 *
 * \param watchdog A pointer to the watchdog
 *
 * \retval TIMEBASE_RC_NULL_POINTER if the watchdog is NULL
 * \retval TIMEBASE_RC_WATCHDOG_NOT_REGISTERED the watchdog is not registered
 * \retval TIMEBASE_RC_WATCHDOG_UNAVAILABLE if the watchdog can't be registered again
 * \retval TIMEBASE_RC_OK otherwise
 */
enum TimebaseReturnCode timebase_update_watchdog(struct Watchdog *watchdog);

/*!
 * \brief Routine that checks which functions shuold run during this
 *
 * \retval TIMEBASE_RC_DISABLED if the timebase is disabled
 * \retval TIMEBASE_RC_OK otherwise
 */
enum TimebaseReturnCode timebase_routine(void);

#else // CONF_TIMEBASE_MODULE_ENABLE

#define timebase_init(resolution) (TIMEBASE_RC_OK)
#define timebase_set_enable() EAGLETRT_API_NOP()
#define timebase_inc_tick() (TIMEBASE_RC_OK)
#define timebase_get_tick() (0U)
#define timebase_get_time() (0U)
#define timebase_get_resolution() (1U) // The default value of 1 is used to avoid 0 division error
#define timebase_regsiter_watchdog(watchdog) (TIMEBASE_RC_OK)
#define timebase_unregsiter_watchdog(watchdog) (TIMEBASE_RC_OK)
#define timebase_update_watchdog(watchdog) (TIMEBASE_RC_OK)
#define timebase_routine() (TIMEBASE_RC_OK)

#endif // CONF_TIMEBASE_MODULE_ENABLE

#endif // TIMEBASE_H
