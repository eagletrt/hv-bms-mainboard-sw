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

#include "can-primary.h"
#include "error.h"
#include "mainboard-conf.h"
#include "mainboard-def.h"

#ifdef CONF_ERROR_MODULE_ENABLE

/*!
 * \brief Initialization of the internal error handler structure
 */
enum ErrorReturnCode error_api_init(void);

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
enum ErrorReturnCode error_api_set(enum ErrorGroup group, error_instance instance);

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
enum ErrorReturnCode error_api_reset(enum ErrorGroup group, error_instance instance);

/*!
 * \brief Get the number of expired errors
 *
 * \return size_t The number of expired errors
 */
size_t error_api_get_expired(void);

/*!
 * \brief Get the information of the first expired error
 *
 * \return ErrorInfo The information of the first expired error
 */
ErrorInfo error_api_get_expired_info(void);

/*!
 * \brief Get a pointer to the CAN payload of the mainboard errors
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \return Pointer of the payload
 */
union CanPrimaryMessages *error_api_get_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the cellboard errors
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \return Pointer of the payload
 */
union CanPrimaryMessages *error_api_get_cellboard_a_canlib_payload(size_t *byte_size);
union CanPrimaryMessages *error_api_get_cellboard_b_canlib_payload(size_t *byte_size);
union CanPrimaryMessages *error_api_get_cellboard_c_canlib_payload(size_t *byte_size);

/*!
 * \brief Handler can error messages sent from the cellboards
 *
 * \param payload the payload of the error message
 */
void error_api_cellboard_handle(
    CellboardId cellboard,
    uint8_t post,
    uint8_t undervoltage,
    uint8_t overvoltage,
    uint8_t undertemperature,
    uint8_t overtemperature,
    uint8_t discharge_undertemperature,
    uint8_t discharge_overtemperature,
    uint8_t can_communication,
    uint8_t flash,
    uint8_t bms_monitor_communication,
    uint8_t openwire1,
    uint8_t openwire2,
    uint8_t openwire3,
    uint8_t openwire4,
    uint8_t openwire5,
    uint8_t openwire6,
    uint8_t openwire7,
    uint8_t openwire8,
    uint8_t openwire9,
    uint8_t openwire10,
    uint8_t openwire11,
    uint8_t openwire12,
    uint8_t openwire13);

#ifdef CONF_ERROR_STRINGS_ENABLE

char *error_api_get_group_name_string(const enum ErrorGroup group);

#else // CONF_ERROR_STRINGS_ENABLE

#define error_api_get_group_name_string(group) ("")

#endif // CONF_ERROR_STRINGS_ENABLE

#else // CONF_ERROR_MODULE_ENABLE

#define error_api_init() (ERROR_RC_OK)
#define error_api_set(group, instance) (ERROR_RC_OK)
#define error_api_reset(group, instance) (ERROR_RC_OK)
#define error_api_get_expired() (0U)
#define error_api_get_expired_info() ((ErrorInfo){ 0U })
#define error_api_cellboard_handle(payload) (NULL)
#define error_api_get_error_canlib_payload(byte_size) (NULL);

#endif // CONF_ERROR_MODULE_ENABLE

#endif // ERROR_API_H
