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

#include "mainboard-def.h"
#include "volt.h"
#include "mainboard-conf.h"

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
 * \brief Set a single voltage value
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the voltage to update
 * \param[in] voltage The voltage value
 */
void volt_api_set_value(CellboardId cellboard, uint8_t index, volt_t voltage);

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
 * \brief Get a pointer to the CAN payload of the Cellboard1 cells voltages
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *volt_api_get_cellboard1_voltage_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard2 cells voltages
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *volt_api_get_cellboard2_voltage_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard3 cells voltages
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *volt_api_get_cellboard3_voltage_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard4 cells voltages
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *volt_api_get_cellboard4_voltage_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard5 cells voltages
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *volt_api_get_cellboard5_voltage_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard6 cells voltages
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *volt_api_get_cellboard6_voltage_canlib_payload(size_t *byte_size);

/*!
 * \brief Handle the cellboard voltage info can payload
 *
 * \param[in] min The minimum voltage value
 * \param[in] max The maximum voltage value
 * \param[in] average The average voltage value
 * \param[in] sum The voltage sum
 */
void volt_api_cellboard_voltage_info_handle(
    CellboardId cellboard,
    volt_t min,
    volt_t max,
    volt_t average,
    volt_t sum);

#else // CONF_VOLTAGE_MODULE_ENABLE

#define volt_api_init() (VOLT_RC_OK)
#define volt_api_get_values() (NULL)
#define volt_api_set_value(cellboard, index, voltage) EAGLETRT_API_NOP()
#define volt_api_get_min() (VOLT_NOMINAL_V)
#define volt_api_get_max() (VOLT_NOMINAL_V)
#define volt_api_get_avg() (VOLT_NOMINAL_V)
#define volt_api_get_sum() (VOLT_NOMINAL_V * CELLBOARD_COUNT * CELLBOARD_SEGMENT_SERIES_COUNT)
#define volt_api_cellboard_voltage_info_handle(cellboard, min, max, average, sum) EAGLETRT_API_NOP()
#define volt_api_voltage_handle(payload) EAGLETRT_API_NOP()
#define volt_api_get_cellboard1_voltage_canlib_payload(byte_size) (NULL)
#define volt_api_get_cellboard2_voltage_canlib_payload(byte_size) (NULL)
#define volt_api_get_cellboard3_voltage_canlib_payload(byte_size) (NULL)
#define volt_api_get_cellboard4_voltage_canlib_payload(byte_size) (NULL)
#define volt_api_get_cellboard5_voltage_canlib_payload(byte_size) (NULL)
#define volt_api_get_cellboard6_voltage_canlib_payload(byte_size) (NULL)

#endif // CONF_VOLTAGE_MODULE_ENABLE

#endif // VOLT_API_H
