/*!
 * \file flash-control-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Flash procedure control: resets the microcontroller when the mainboard is
 *        the target or enables the CAN bridge when a cellboard is the target
 */

#ifndef FLASH_CONTROL_API_H
#define FLASH_CONTROL_API_H

#include "flash-control.h"

/*!
 * \brief Initializes the flash control module
 *
 * \retval FLASH_CONTROL_RC_OK always
 */
enum FlashControlReturnCode flash_control_api_init(void);

/*!
 * \brief Sets the target of the flash procedure
 *
 * \param target The board targeted by the flash request
 *
 * \retval FLASH_CONTROL_RC_OK if the target was set successfully
 * \retval FLASH_CONTROL_RC_INVALID_PARAMETERS if the target is invalid
 */
enum FlashControlReturnCode flash_control_api_set_target(enum FlashControlRequestState target);

/*!
 * \brief Starts the flash procedure
 *
 * \retval FLASH_CONTROL_RC_OK if the flash procedure was started
 * \retval FLASH_CONTROL_RC_INVALID_PARAMETERS if no valid target was set
 * \retval FLASH_CONTROL_RC_FLASH_ALREADY_STARTED if the flash procedure is already active
 */
enum FlashControlReturnCode flash_control_api_start_flash(void);

/*!
 * \brief Routine of the flash procedure, to be called periodically
 *
 * \retval FLASH_CONTROL_RC_OK if the routine ran successfully
 * \retval FLASH_CONTROL_RC_FLASH_NOT_STARTED if the flash procedure is not active
 */
enum FlashControlReturnCode flash_control_api_routine(void);

/*!
 * \brief Stops the flash procedure
 *
 * \retval FLASH_CONTROL_RC_OK if the flash procedure was stopped
 * \retval FLASH_CONTROL_RC_FLASH_NOT_STARTED if the flash procedure is not active
 */
enum FlashControlReturnCode flash_control_api_stop_flash(void);

/*!
 * \brief Checks if the flash control has valid parameters
 *
 * \retval true if the parameters are valid
 * \retval false if the parameters are invalid
 */
bool flash_control_api_has_valid_parameters(void);

#endif // FLASH_CONTROL_API_H
