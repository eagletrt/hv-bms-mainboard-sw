/*!
 * \file temp-api.h
 * \date 2026-05-22
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Temperature measurment and control
 */

#ifndef TEMP_API_H
#define TEMP_API_H

#include "temp.h"
#include "mainboard-conf.h"

#ifdef CONF_TEMPERATURE_MODULE_ENABLE

/*!
 * \brief Initialize the temperature module
 *
 * \retval TEMP_OK The module is initialized successfully
 */
enum TempReturnCode temp_api_init(void);

/*!
 * \brief Get a pointer to the array where the temperature values are stored
 *
 * \returns cells_temp* The pointer to the array
 */
const cells_temp *temp_api_get_values(void);

/*!
 * \brief Get the minimum cell temperature in the pack
 *
 * \returns celsius_t The minimum temperature value in °C
 */
celsius_t temp_api_get_min(void);

/*!
 * \brief Get the maximum cell temperature in the pack
 *
 * \returns celsius_t The maximum temperature value in °C
 */
celsius_t temp_api_get_max(void);

/*!
 * \brief Get the sum of the cells temperatures of the pack
 *
 * \returns celsius_t The sum of the temperatures in °C
 */
celsius_t temp_api_get_sum(void);

/*!
 * \brief Get the average cell temperature of the pack
 *
 * \returns celsius_t The average temperature in °C
 */
celsius_t temp_api_get_avg(void);

/*!
 * \brief Get a pointer to the CAN payload of the cells temperature stats
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer of the payload
 */
union CanPrimaryMessages *temp_api_get_cells_temperature_info_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard1 cells temperatures
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *temp_api_get_cellboard1_temperature_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard2 cells temperatures
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *temp_api_get_cellboard2_temperature_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard3 cells temperatures
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *temp_api_get_cellboard3_temperature_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard4 cells temperatures
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *temp_api_get_cellboard4_temperature_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard5 cells temperatures
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *temp_api_get_cellboard5_temperature_canlib_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload of the Cellboard6 cells temperatures
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *temp_api_get_cellboard6_temperature_canlib_payload(size_t *byte_size);

/*!
 * \brief Handle the received cellboard cells temperature
 *
 * \param payload A pointer to the canlib payload
 */
// void temp_api_cells_temperature_handle(bms_cellboard_cells_temperature_converted_t *payload);

/*!
 * \brief Get a pointer to the CAN payload of the cells temperature
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_cellboard_cells_temperature_converted_t* A pointer to the payload
 */
// primary_hv_cells_temperature_converted_t *temp_api_get_cells_temperature_canlib_payload(size_t *byte_size);

#else

#define temp_api_init() (TEMP_OK)
#define temp_api_get_values() (NULL)
#define temp_api_get_min() (NULL)
#define temp_api_get_max() (NULL)
#define temp_api_get_avg() (NULL)
#define temp_api_cells_temperature_handle(payload) EAGLETRT_API_NOP()
#define temp_api_get_cells_temperature_canlib_payload(byte_size) (NULL)
#define temp_api_get_cells_temperature_stats_canlib_payload(byte_size) (NULL)

#endif // CONF_TEMPERATURE_MODULE_ENABLE

#endif // TEMP_API_H
