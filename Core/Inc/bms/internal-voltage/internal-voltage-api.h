/*!
 * \file internal-voltage-api.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal voltages measurements and controls
 */

#ifndef INTERNAL_VOLTAGE_API_H
#define INTERNAL_VOLTAGE_API_H

#include "internal-voltage.h"
#include "mainboard-conf.h"

#ifdef CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

/*!
 * \brief Initialization function of the internal voltage module
 *
 * \param send A pointer to the function callback used to send data
 * \param send A pointer to the function callback used to send and receive data
 *
 * \retval INTERNAL_VOLTAGE_NULL_POINTER if any of the given parameters are NULL
 * \retval INTERNAL_VOLTAGE_OK otherwise
 */
enum InternalVoltageReturnCode internal_voltage_api_init(spi_send_callback_t send, spi_send_receive_callback_t send_receive);

/*!
 * \brief Read all channels of the external ADC
 *
 * \retval INTERNAL_VOLTAGE_OK
 */
enum InternalVoltageReturnCode internal_voltage_api_read_all(void);

/*!
 * \brief Get the voltage of the Tractive System in V
 *
 * \returns volt_t The TS voltage in V
 */
volt_t internal_voltage_api_get_ts(void);

/*!
 * \brief Get the voltage of the battery pack in V
 *
 * \returns volt_t The battery pack voltage in V
 */
volt_t internal_voltage_api_get_pack(void);

/*!
 * \brief Get a pointer to the CAN payload of the TS voltage info
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer of the payload
 */
union CanPrimaryMessages *internal_voltage_api_get_canlib_payload(size_t *byte_size);

#else // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

#define internal_voltage_api_init(send, send_receive) (INTERNAL_VOLTAGE_RC_OK)
#define internal_voltage_api_read_all() (INTERNAL_VOLTAGE_RC_OK)
#define internal_voltage_api_get_ts() (0U)
#define internal_voltage_api_get_pack() (0U)
#define internal_voltage_api_get_canlib_payload(byte_size) (NULL)

#endif // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

#endif // INTERNAL_VOLTAGE_API_H
