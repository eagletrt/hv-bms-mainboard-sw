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
 * \brief Initialize the PHC handler
 *
 * \returns PHC_RC_OK if the initialization was successful, PHC_RC_ERROR if an error occurred
 */
enum PHCReturnCode phc_api_init(void);

/*!
 * \brief Run a specific PHC control and set the corresponding fault or update
 *        the corresponding condition in the mainboard status
 *
 * \details When the data needed by the check has not been received yet nothing is
 *          written and the time of the last run is not updated, so a check that never
 *          gets its data makes \ref phc_api_check_phc set the PHC fault
 *
 * \param control The PHC control to check
 *
 * \retval PHC_RC_OK if the check ran
 * \retval PHC_RC_NOT_READY if the data needed by the check has not been received yet
 * \retval PHC_RC_ERROR if the control does not exist
 */
enum PHCReturnCode phc_api_check(enum PHCControl control);

/*!
 * \brief Check that every PHC check has run in time
 *
 * \details Sets MAINBOARD_STATUS_FAULT_PHC_FAULT if a check has not run within
 *          PHC_MAX_CHECK_INTERVAL_MS since its last run, or has never run after
 *          PHC_MAX_STARTUP_INTERVAL_MS from boot (for example because its data never arrived)
 *
 * \returns PHC_RC_OK
 */
enum PHCReturnCode phc_api_check_phc(void);

#endif // PHC_API_H
