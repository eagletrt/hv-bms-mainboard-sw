/*!
 * \file watchdog.h
 * \date 2024-04-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Implementation of generic watchdogs that times-out a certain interval of time
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdbool.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

/*!
 * \brief Return code for the watchdog module functions
 */
enum WatchdogReturnCode {
    WATCHDOG_RC_OK,           /*!< The function exeuted succesfully */
    WATCHDOG_RC_NULL_POINTER, /*!< A NULL pointer was given to a function */
    WATCHDOG_RC_BUSY,         /*!< The watchdog is already running */
    WATCHDOG_RC_TIMED_OUT,    /*!< The watchdog has timed out */
    WATCHDOG_RC_NOT_RUNNING,  /*!< The watchdog is not running */
    WATCHDOG_RC_UNAVAILABLE   /*!< The watchdog is not registered inside the timebase */
};

/*!
 * \brief Type definition for a function that is called when the watchdog times-out
 *
 * \details When the watchdog times-out it unregister itself from the timebase automatically
 */
typedef void (*watchdog_timeout_callback_t)(void);

/*!
 * \brief Definiton of the watchdog structure handler
 */
struct Watchdog {
    bool running;                       /*!< True if the watchdog is running, false otherwise */
    bool timed_out;                     /*!< True if the watchdog is running, false otherwise */
    ticks_t timeout;                    /*!< The number of ticks that should elapse for the watchdog to time-out */
    watchdog_timeout_callback_t expire; /*!< The function that is called when the watchdog times-out */
};

#ifdef CONF_WATCHDOG_MODULE_ENABLE

/*!
 * \brief Initialize the watchdog
 *
 * \param watchdog A pointer to the watchdog handler structure
 * \param timeout The number of ticks that should elapse for the watchdog to time-out
 * \param expire The function that is called when the watchdog times-out
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog or the expire pointers are NULL
 * \retval WATCHDOG_RC_BUSY if the watchdog is already running
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode watchdog_init(
    struct Watchdog *watchdog,
    ticks_t timeout,
    watchdog_timeout_callback_t expire);

/*!
 * \brief De-initialize the watchdog
 *
 * \param watchdog A pointer to the watchdog handler structure
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog or the internal expire pointers are NULL
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode watchdog_deinit(struct Watchdog *watchdog);

/*!
 * \brief Start a watchdog
 *
 * \details A timed out watchdog cannot be started
 *
 * \param watchdog A pointer to the watchdog handler structure
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL
 * \retval WATCHDOG_RC_BUSY if the watchdog is already running
 * \retval WATCHDOG_RC_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_RC_UNAVAILABLE if the watchdog can't be registered
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode watchdog_start(struct Watchdog *watchdog);

/*!
 * \brief Stop a watchdog
 *
 * \details A timed out watchdog cannot be stopped
 *
 * \param watchdog A pointer to the watchdog handler structure
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL
 * \retval WATCHDOG_RC_NOT_RUNNING if the watchdog is not running
 * \retval WATCHDOG_RC_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode watchdog_stop(struct Watchdog *watchdog);

/*!
 * \brief Start a watchdog even if it has timed out
 *
 * \details If the watchdog is not running it is started
 * as the watchdog start function
 *
 * \param A pointer to the watchdog handler structure
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL
 *  retval WATCHDOG_RC_UNAVAILABLE if the watchdog can't be registered
 *  retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode watchdog_restart(struct Watchdog *watchdog);

/*!
 * \brief Reset the watchdog internal time to 0
 *
 * \details The watchdog is not stopped after the reset
 *
 * \param watchdog A pointer to the watchdog
 *
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is NULL
 * \retval WATCHDOG_RC_NOT_RUNNING if the watchdog is not running
 * \retval WATCHDOG_RC_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_RC_UNAVAILABLE if the watchdog can't be registered inside the timebase
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode watchdog_reset(struct Watchdog *watchdog);

/*!
 * \brief Set the watchdog status as timed out
 *
 * \param watchdog A pointer to the watchdog
 *
 * \retval WATCHDOG_RC_NULL_POINTER A NULL pointer was given as parameter
 * \retval WATCHDOG_RC_NOT_RUNNING if the watchdog is not running
 * \retval WATCHDOG_RC_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_RC_OK otherwise
 */
enum WatchdogReturnCode watchdog_timeout(struct Watchdog *watchdog);

/*!
 * \param Check if the watchdog has timed out
 *
 * \param watchdog A pointer to the watchdog
 *
 * \return True if the watchdog has timed out, false otherwise
 */
bool watchdog_is_timed_out(struct Watchdog *watchdog);

#else // CONF_WATCHDOG_MODULE_ENABLE

#define watchdog_init(watchdog, timeout, expire) (WATCHDOG_RC_OK)
#define watchdog_deinit(watchdog) (WATCHDOG_RC_OK)
#define watchdog_start(watchdog) (WATCHDOG_RC_OK)
#define watchdog_stop(watchdog) (WATCHDOG_RC_OK)
#define watchdog_restart(watchdog) (WATCHDOG_RC_OK)
#define watchdog_reset(watchdog) (WATCHDOG_RC_OK)
#define watchdog_timeout(watchdog) (WATCHDOG_RC_OK)
#define watchdog_is_timed_out(watchdog) (false)

#endif // CONF_WATCHDOG_MODULE_ENABLE

#endif // WATCHDOG_H
