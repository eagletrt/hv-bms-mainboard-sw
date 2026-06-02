/*!
 * \file current-api.h
 * \date 2024-06-08
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Current measurement and control public API
 */

#ifndef CURRENT_API_H
#define CURRENT_API_H

#include "current.h"

#ifdef CONF_CURRENT_MODULE_ENABLE

/*!
 * \brief Initialize the internal current structure handler
 *
 * \retval CURRENT_RC_OK if the initialization was successful
 */
enum CurrentReturnCode current_api_init(void);

/*!
 * \brief Get the supplied current in A
 *
 * \returns ampere_t The current in A
 */
ampere_t current_api_get_current(void);

/*!
 * \brief Get the calculated power value in kW
 *
 * \returns kilowatt_t The power in kW
 */
kilowatt_t current_api_get_power(void);

/*!
 * \brief Start the watchdog used to check for the current sensor communication
 *
 * \retval WATCHDOG_NULL_POINTER if the watchdog is NULL
 * \retval WATCHDOG_BUSY if the watchdog is already running
 * \retval WATCHDOG_TIMED_OUT if the watchdog has already timed out
 * \retval WATCHDOG_UNAVAILABLE if the watchdog can't be registered
 * \retval WATCHDOG_OK otherwise
 */
WatchdogReturnCode current_api_start_sensor_communication_watchdog(void);

/*!
 * \brief Handle the received response from the current sensor
 *
 * \param payload A pointer to the canlib payload of the response
 */
void current_api_handle(bms_ivt_msg_result_i_t *payload);

/*!
 * \brief Get a pointer to the CAN payload of the current
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_current_converted_t* A pointer to the payload
 */
primary_hv_current_converted_t *current_api_get_current_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the power
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_power_converted_t* A pointer to the payload
 */
primary_hv_power_converted_t *current_api_get_power_canlib_payload(size_t *byte_size);

#else // CONF_CURRENT_MODULE_ENABLE

#define current_api_init() (CURRENT_OK)
#define current_api_get_current() (0.f)
#define current_api_get_power() (0.f)
#define current_api_start_sensor_communication_watchdog() (WATCHDOG_OK)
#define current_api_handle(payload) EAGLETRT_API_NOP()
#define current_api_get_current_canlib_payload(byte_size) (NULL)
#define current_api_get_power_canlib_payload(byte_size) (NULL)

#endif // CONF_CURRENT_MODULE_ENABLE

#endif // CURRENT_API_H
