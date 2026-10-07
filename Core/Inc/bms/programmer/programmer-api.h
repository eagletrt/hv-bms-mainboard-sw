/*!
 * \file programmer-api.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions used during the flash procedure where the microcontroller
 * is reset and openblt load the new code inside the flash memory
 */

#ifndef PROGRAMMER_API_H
#define PROGRAMMER_API_H

#include "programmer.h"

/*!
 * \brief Intialize the internal programmer handler structure
 *
 * \param reset A pointer to the function that resets the microcontroller
 *
 * \retval PROGRAMMER_RC_OK if the initialization is successful
 */
enum ProgrammerReturnCode programmer_api_init(system_reset_callback_t reset);

/*!
 * \brief Handle the received flash request
 *
 * \param payload A pointer to the canlib payload of the request
 */
// void programmer_api_flash_request_handle(primary_hv_flash_request_converted_t *payload);

/*!
 * \brief Handle the received flash response from the cellboards
 *
 * \param payload A pointer to the canlib payload of the response
 */
// void programmer_api_cellboard_flash_response_handle(bms_cellboard_flash_response_converted_t *payload);

/*!
 * \brief Handle the received actual flash command
 *
 * \param payload A pointer to the canlib payload of the command
 */
// void programmer_api_flash_handle(primary_hv_flash_converted_t *payload);

/*!
 * \brief Routine that should be called during the flash procedure
 *
 * \attention This function can reset the microcontroller
 *
 * \retval PROGRAMMER_RC_TIMEOUT if the flash procedure has timed out
 * \retval PROGRAMMER_RC_OK if the flash procedure has finished
 * \retval PROGRAMMER_RC_BUSY otherwise
 */
enum ProgrammerReturnCode programmer_api_routine(void);

void programmer_api_reset_mcu(void);

#endif // PROGRAMMER_API_H
