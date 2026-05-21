/*!
 * \file error-api.h
 * \date 2024-07-12
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Simple wrapper for the error handler generated code
 */

#ifndef ERROR_API_H
#define ERROR_API_H

#include "error.h"

#ifdef CONF_ERROR_MODULE_ENABLE

/*!
 * \brief Initialization of the internal error handler structure
 */
enum ErrorReturnCode error_init(void);

/*!
 * \brief Set an error instance
 *
 * \param group The error group of the instance
 * \param instance The error instance to set
 *
 * \retval ErrorReturnCode
 *     - ERROR_RC_OK the function executed successfully
 *     - ERROR_RC_UNKNOWN an unknown error occurred
 */
enum ErrorReturnCode error_set(enum ErrorGroup group, error_instance instance);

/*!
 * \brief Reset an error instance
 *
 * \param group The error group of the instance
 * \param instance The error instance to reset
 *
 * \retval ErrorReturnCode
 *     - ERROR_RC_OK the function executed successfully
 *     - ERROR_RC_UNKNOWN an unknown error occurred
 */
enum ErrorReturnCode error_reset(enum ErrorGroup group, error_instance instance);

/*!
 * \brief Get the number of expired errors
 *
 * \return size_t The number of expired errors
 */
size_t error_get_expired(void);

/*!
 * \brief Get the information of the first expired error
 *
 * \return ErrorInfo The information of the first expired error
 */
ErrorInfo error_get_expired_info(void);

/*!
 * \brief Handler can error messages sent from the cellboards
 *
 * \param payload the payload of the error message
 */
void error_cellboard_handle(bms_cellboard_error_t *payload);

/*!
 * \brief Get a pointer to the CAN payload of the last mainboard error 
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \return primary_hv_error_converted_t* A pointer to the payload
 */
primary_hv_error_converted_t *error_get_error_canlib_payload(size_t *const byte_size);

#ifdef CONF_ERROR_STRINGS_ENABLE

char *error_get_group_name_string(const enum ErrorGroup group);

#else // CONF_ERROR_STRINGS_ENABLE

#define error_get_group_name_string(group) ("")

#endif // CONF_ERROR_STRINGS_ENABLE

#else // CONF_ERROR_MODULE_ENABLE

#define error_init() (ERROR_OK)
#define error_set(group, instance) (ERROR_OK)
#define error_reset(group, instance) (ERROR_OK)
#define error_get_expired() (0U)
#define error_get_expired_info() ((ErrorInfo){ 0U })
#define error_cellboard_handle(payload) (NULL)
#define error_get_error_canlib_payload(byte_size) (NULL);

#endif // CONF_ERROR_MODULE_ENABLE

#endif // ERROR_API_H
