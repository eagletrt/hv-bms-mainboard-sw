/*!
 * \file cooling-temp-api.h
 * \date 2024-09-24
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cooling temperature measurment and control
 */

#ifndef COOLING_TEMP_API_H
#define COOLING_TEMP_API_H

#include "cooling-temp.h"
#include "mainboard-conf.h"

#ifdef CONF_COOLING_TEMPERATURE_MODULE_ENABLE

/*!
 * \brief Initialize the cooling temperature module
 *
 * \retval COOLING_TEMP_RC_OK the init always succeeds as it just clears the handler.
 */
enum CoolingTempReturnCode cooling_temp_api_init(void);

/*!
 * \brief Callback used in the adc to notify the cooling temperature module that the conversion is completed
 *
 * \param index The idx of the value to be updated
 * \param value The voltage to copy in V
 *
 * \retval COOLING_TEMP_RC_OUT_OF_BOUNDS if the index is greater than the total number of values or the voltage value is out of bounds
 * \retval COOLING_TEMP_RC_OK if the value is updated successfully
 */
enum CoolingTempReturnCode cooling_temp_api_notify_conversion_complete(size_t index, volt_t value);

/*!
 * \brief Update a single temperature value
 *
 * \param index The index of the value to update
 * \param value The new value
 *
 * \retval COOLING_TEMP_RC_OUT_OF_BOUNDS if the index is greater than the total number of values
 * \retval COOLING_TEMP_RC_OK otherwise
 */
enum CoolingTempReturnCode cooling_temp_api_update_value(size_t index, celsius_t value);

/*!
 * \brief Get a pointer to the array where the temperature values are stored
 *
 * \returns cooling_temps* The pointer to the array
 */
const cooling_temps *cooling_temp_api_get_values(void);

/*!
 * \brief Get the minimum temperature in the cooling loop
 *
 * \returns celsius_t The minimum temperature value in °C
 */
celsius_t cooling_temp_api_get_min(void);

/*!
 * \brief Get the maximum temperature in the cooling loop
 *
 * \returns celsius_t The maximum temperature value in °C
 */
celsius_t cooling_temp_api_get_max(void);

/*!
 * \brief Get the sum of the temperatures of the cooling loop
 *
 * \returns celsius_t The sum of the temperatures in °C
 */
celsius_t cooling_temp_api_get_sum(void);

/*!
 * \brief Get the average temperature of the cooling loop
 *
 * \returns celsius_t The average temperature in °C
 */
celsius_t cooling_temp_api_get_avg(void);

/*!
 * \brief Get a pointer to the CAN payload of the cooling temperatures
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_cooling_temperature_converted_t* A pointer to the payload
 */
// primary_hv_cooling_temperature_converted_t *cooling_temp_api_get_temperatures_canlib_payload(size_t *byte_size);

#else

#define cooling_temp_api_init() (COOLING_TEMP_RC_OK)
#define cooling_temp_api_notify_conversion_complete(index, value) (COOLING_TEMP_RC_OK)
#define cooling_temp_api_get_values() (NULL)
#define cooling_temp_api_get_min() (NULL)
#define cooling_temp_api_get_max() (NULL)
#define cooling_temp_api_get_sum() (NULL)
#define cooling_temp_api_get_avg() (NULL)
#define cooling_temp_api_get_temperatures_canlib_payload(byte_size) (NULL)

#endif // CONF_COOLING_TEMPERATURE_MODULE_ENABLE

#endif // COOLING_TEMP_API_H
