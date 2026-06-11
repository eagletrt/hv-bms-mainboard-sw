/*!
 * \file volt-api.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Voltage measurment and control
 */

#ifndef VOLT_API_H
#define VOLT_API_H

#include "volt.h"
#include "eagletrt-api.h"

#ifdef CONF_VOLTAGE_MODULE_ENABLE

/*!
 * \brief Initialize the voltage module
 *
 * \retval VOLT_OK if the initialization is successful
 */
enum VoltReturnCode volt_api_init(void);

/*!
 * \brief Get a pointer to the array where the voltages are stored
 *
 * \returns cells_voltage* The pointer to the array
 */
const cells_voltage *volt_api_get_values(void);

/*!
 * \brief Get the minimum cell voltage in the pack
 *
 * \returns volt_t The minimum voltage in V
 */
volt_t volt_api_get_min(void);

/*!
 * \brief Get the maximum cell voltage in the pack
 *
 * \returns volt_t The maximum voltage in V
 */
volt_t volt_api_get_max(void);

/*!
 * \brief Get the average cell voltage of the pack
 *
 * \returns volt_t The average voltage in V
 */
volt_t volt_api_get_avg(void);

/*!
 * \brief Get the sum of the cells voltages of the pack
 *
 * \returns volt_t The cells voltages sum
 */
volt_t volt_api_get_sum(void);

/*!
 * \brief Handle the received cellboard cells voltage
 *
 * \param payload A pointer to the canlib payload
 */
void volt_api_cells_voltage_handle(bms_cellboard_cells_voltage_converted_t *payload);

/*!
 * \brief Get a pointer to the CAN payload of the cells voltages
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_cellboard_cells_voltage_converted_t* A pointer to the payload
 */
primary_hv_cells_voltage_converted_t *volt_api_get_cells_voltage_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the cells voltage stats
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_cellboard_cells_voltage_stats_converted_t* A pointer to the payload
 */
primary_hv_cells_voltage_stats_converted_t *volt_api_get_cells_voltage_stats_canlib_payload(size_t *byte_size);

#else // CONF_VOLTAGE_MODULE_ENABLE

#define volt_api_init() (VOLT_OK)
#define volt_api_get_values() (NULL)
#define volt_api_get_min() (VOLT_NOMINAL_V)
#define volt_api_get_max() (VOLT_NOMINAL_V)
#define volt_api_get_avg() (VOLT_NOMINAL_V)
#define volt_api_get_sum() (VOLT_NOMINAL_V * CELLBOARD_COUNT * CELLBOARD_SEGMENT_SERIES_COUNT)
#define volt_api_cells_voltage_handle(payload) EAGLETRT_API_NOP()
#define volt_api_get_cells_voltage_canlib_payload(byte_size) (NULL)
#define volt_api_get_cells_voltage_stats_canlib_payload(byte_size) (NULL)

#endif // CONF_VOLTAGE_MODULE_ENABLE

#endif // VOLT_API_H
