/*!
 * \file phc-api.h
 * \date 2026-11-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Health Controller (PHC): checks the values against their limits and
 *        sets the faults and conditions in the mainboard status
 */

#ifndef PHC_API_H
#define PHC_API_H

#include "phc.h"

/*!
 * \brief Initialize the PHC handler and its watchdogs
 *
 * \details Starts the startup watchdog: every check must run once within
 *          PHC_MAX_STARTUP_INTERVAL_MS, otherwise the PHC fault is set
 *
 * \attention Must be called once, after \ref mainboard_watchdogs_api_init
 *
 * \retval PHC_RC_ERROR if the handler is already initialized or a watchdog fails to initialize or start
 * \retval PHC_RC_OK otherwise
 */
enum PHCReturnCode phc_api_init(void);

/*!
 * \brief Run a specific PHC control and set the corresponding fault or update
 *        the corresponding condition in the mainboard status
 *
 * \details Every run starts or pets the watchdog of the control: once a check has
 *          run, it must run again within PHC_MAX_CHECK_INTERVAL_MS, otherwise the
 *          PHC fault is set. When the data needed by the check has not been received
 *          yet nothing is written and the watchdog is not touched, so a check that
 *          never gets its data sets the PHC fault
 *
 * \attention Must be called from the same context as \ref mainboard_watchdogs_api_routine
 *
 * \param control The PHC control to check
 *
 * \retval PHC_RC_OK if the check ran
 * \retval PHC_RC_NOT_READY if the data needed by the check has not been received yet
 * \retval PHC_RC_ERROR if the control does not exist
 */
enum PHCReturnCode phc_api_check(enum PHCControl control);

#endif // PHC_API_H
