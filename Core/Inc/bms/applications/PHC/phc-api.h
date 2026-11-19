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
 * \brief Check a specific PHC control and set the corresponding fault in the mainboard status
 *
 * \param control The PHC control to check
 *
 * \returns PHC_RC_OK if the check was performed successfully, PHC_RC_ERROR if an error occurred
 */
enum PHCReturnCode phc_api_check(enum PHCControl control);

/*!
 * \brief Check if all the PHC checks are OK and have been called in due time
 *
 * \returns PHC_RC_OK if the check was performed successfully, PHC_RC_ERROR if an error occurred
*/
enum PHCReturnCode phc_api_check_phc(void);

#endif // PHC_API_H
