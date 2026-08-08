/*!
 * \file cooling-temp-api.h
 * \date 2024-09-24
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cooling temperature measurment and control
 */

#include "cooling-temp-api.h"

#include <string.h>

#include "error-api.h"
#include "eagletrt-api.h"

#ifdef CONF_COOLING_TEMPERATURE_MODULE_ENABLE

EAGLETRT_STATIC struct CoolingTempHandler cooling_temp_handler;

/*!
 * \brief Convert a voltage into a temperature using a polynomial conversion
 *
 * \param value The voltage value in V
 *
 * \return celsius_t The converted value in °C
 */
celsius_t prv_cooling_temp_volt_to_celsius(volt_t value) {
    // Value is converted in V and limited to fit the polynomial range

    constexpr volt_t value_min = COOLING_TEMP_MIN_LIMIT_V;
    constexpr volt_t value_max = COOLING_TEMP_MAX_LIMIT_V;

    value = EAGLETRT_API_CLAMP(value, value_min, value_max);
    const double volt0 = value;
    const double volt2 = volt0 * volt0;
    const double volt3 = volt2 * volt0;
    const double volt4 = volt2 * volt2;
    const double volt5 = volt4 * volt0;
    const double volt6 = volt3 * volt3;
    return COOLING_TEMP_COEFF_0 +
           (COOLING_TEMP_COEFF_1 * volt0) +
           (COOLING_TEMP_COEFF_2 * volt2) +
           (COOLING_TEMP_COEFF_3 * volt3) +
           (COOLING_TEMP_COEFF_4 * volt4) +
           (COOLING_TEMP_COEFF_5 * volt5) +
           (COOLING_TEMP_COEFF_6 * volt6);
}

/*!
 * \brief Check if the cells temperature values are in range otherwise set an error
 *
 * \param value The temperature value to check in °C
 * \param index The index of the error instance to set/reset in case of out of bounds value
 */
EAGLETRT_STATIC_INLINE void prv_cooling_temp_check_value(const size_t index, const celsius_t value) {
    if (value <= COOLING_TEMP_MIN_C) {
        (void)error_api_set(ERROR_GROUP_COOLING_UNDER_TEMPERATURE, (error_instance)index);
    } else {
        (void)error_api_reset(ERROR_GROUP_COOLING_UNDER_TEMPERATURE, (error_instance)index);
    }
    if (value >= COOLING_TEMP_MAX_C) {
        (void)error_api_set(ERROR_GROUP_COOLING_OVER_TEMPERATURE, (error_instance)index);
    } else {
        (void)error_api_reset(ERROR_GROUP_COOLING_OVER_TEMPERATURE, (error_instance)index);
    }
}

enum CoolingTempReturnCode cooling_temp_api_init(void) {
    memset(&cooling_temp_handler, 0U, sizeof(cooling_temp_handler));
    return COOLING_TEMP_RC_OK;
}

enum CoolingTempReturnCode cooling_temp_api_notify_conversion_complete(size_t index, const volt_t value) {
    // Convert the raw value to celsius
    const celsius_t temp = prv_cooling_temp_volt_to_celsius(value);
    return cooling_temp_api_update_value(index, temp);
}

enum CoolingTempReturnCode cooling_temp_api_update_value(const size_t index, const celsius_t value) {
    if (index >= COOLING_TEMP_COUNT) {
        return COOLING_TEMP_RC_OUT_OF_BOUNDS;
    }
    cooling_temp_handler.temperatures[index] = value;
    // prv_cooling_temp_check_value(index, value);
    return COOLING_TEMP_RC_OK;
}

const cooling_temps *cooling_temp_api_get_values(void) {
    return &cooling_temp_handler.temperatures;
}

celsius_t cooling_temp_api_get_min(void) {
    celsius_t min = cooling_temp_handler.temperatures[0U];
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        min = EAGLETRT_API_MIN(min, cooling_temp_handler.temperatures[i]);
    }
    return min;
}

celsius_t cooling_temp_api_get_max(void) {
    celsius_t max = cooling_temp_handler.temperatures[0U];
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        max = EAGLETRT_API_MAX(max, cooling_temp_handler.temperatures[i]);
    }
    return max;
}

celsius_t cooling_temp_api_get_sum(void) {
    celsius_t sum = 0U;
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        sum += cooling_temp_handler.temperatures[i];
    }
    return sum;
}

celsius_t cooling_temp_api_get_avg(void) {
    return cooling_temp_api_get_sum() / COOLING_TEMP_COUNT;
}

// primary_hv_cooling_temperature_converted_t *cooling_temp_api_get_temperatures_canlib_payload(size_t *const byte_size) {
//     if (byte_size != NULL) {
//         *byte_size = sizeof(cooling_temp_handler.cooling_temp_can_payload);
//     }
//
//     const celsius_t *temps = cooling_temp_handler.temperatures;
//     cooling_temp_handler.cooling_temp_can_payload.inlet = temps[COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE];
//     cooling_temp_handler.cooling_temp_can_payload.outlet_0 = temps[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_1];
//     cooling_temp_handler.cooling_temp_can_payload.outlet_1 = temps[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_2];
//     cooling_temp_handler.cooling_temp_can_payload.outlet_2 = temps[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3];
//     cooling_temp_handler.cooling_temp_can_payload.outlet_3 = temps[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_4];
//     cooling_temp_handler.cooling_temp_can_payload.outlet_4 = temps[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_5];
//     cooling_temp_handler.cooling_temp_can_payload.outlet_5 = temps[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_6];
//     return &cooling_temp_handler.cooling_temp_can_payload;
// }

#ifdef CONF_COOLING_TEMPERATURE_STRINGS_ENABLE

EAGLETRT_STATIC char *cooling_temp_module_name = "cooling temperature";

EAGLETRT_STATIC char *cooling_temp_return_code_name[] = {
    [COOLING_TEMP_RC_OK] = "ok",
    [COOLING_TEMP_RC_NULL_POINTER] = "null pointer",
    [COOLING_TEMP_RC_OUT_OF_BOUNDS] = "out of bounds"
};

EAGLETRT_STATIC char *cooling_temp_return_code_description[] = {
    [COOLING_TEMP_RC_OK] = "executed successfully",
    [COOLING_TEMP_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [COOLING_TEMP_RC_OUT_OF_BOUNDS] = "attempt to access an invalid memory region"
};

#endif //  CONF_COOLING_TEMPERATURE_STRINGS_ENABLE

#endif //  CONF_COOLING_TEMPERATURE_MODULE_ENABLE
