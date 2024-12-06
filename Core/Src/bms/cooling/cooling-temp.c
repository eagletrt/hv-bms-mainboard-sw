/**
 * @file cooling-temp.h
 * @date 2024-09-24
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Cooling temperature measurment and control
 */

#include "cooling-temp.h"

#include <string.h>

#include "error.h"

#ifdef CONF_COOLING_TEMPERATURE_MODULE_ENABLE

_STATIC _CoolingTempHandler hcoolingtemp;

/**
 * @brief Convert a voltage into a temperature using a polynomial conversion
 *
 * @param value The voltage value in V
 *
 * @return celsius_t The converted value in °C
 */
celsius_t _cooling_temp_volt_to_celsius(volt_t value) {
    // Value is converted in V and limited to fit the polynomial range
    value = MAINBOARD_CLAMP(value, COOLING_TEMP_MIN_LIMIT_V, COOLING_TEMP_MAX_LIMIT_V);
    const double v = value;
    const double v2 = v * v;
    const double v3 = v2 * v;
    const double v4 = v2 * v2;
    const double v5 = v4 * v;
    const double v6 = v3 * v3;
    return COOLING_TEMP_COEFF_0 + 
        COOLING_TEMP_COEFF_1 * v + 
        COOLING_TEMP_COEFF_2 * v2 + 
        COOLING_TEMP_COEFF_3 * v3 + 
        COOLING_TEMP_COEFF_4 * v4 + 
        COOLING_TEMP_COEFF_5 * v5 +
        COOLING_TEMP_COEFF_6 * v6;
}

/**
 * @brief Check if the cells temperature values are in range otherwise set an error
 *
 * @param value The temperature value to check in °C
 */
_STATIC_INLINE void _cooling_temp_check_value(const size_t index, const celsius_t value) {
    if (value <= COOLING_TEMP_MIN_C)
        (void)error_set(ERROR_GROUP_COOLING_UNDER_TEMPERATURE, index);
    else
        (void)error_reset(ERROR_GROUP_COOLING_UNDER_TEMPERATURE, index);
    if (value >= COOLING_TEMP_MAX_C)
        (void)error_set(ERROR_GROUP_COOLING_OVER_TEMPERATURE, index);
    else
        (void)error_reset(ERROR_GROUP_COOLING_OVER_TEMPERATURE, index);
}

CoolingTempReturnCode cooling_temp_init(void) {
    memset(&hcoolingtemp, 0U, sizeof(hcoolingtemp));
    return COOLING_TEMP_OK;
}

CoolingTempReturnCode cooling_temp_notify_conversion_complete(size_t index, const volt_t value) {
    // Convert the raw value to celsius
    const celsius_t temp = _cooling_temp_volt_to_celsius(value);
    cooling_temp_update_value(index, temp);
    return COOLING_TEMP_OK;
}

CoolingTempReturnCode cooling_temp_update_value(const size_t index, const celsius_t value) {
    if (index > COOLING_TEMP_COUNT)
        return COOLING_TEMP_OUT_OF_BOUNDS;
    hcoolingtemp.temperatures[index] = value;
    // _cooling_temp_check_value(index, value);
    return COOLING_TEMP_OK;
}

const cooling_temp_t * cooling_temp_get_values(void) {
    return &hcoolingtemp.temperatures;
}


celsius_t cooling_temp_get_min(void) {
    celsius_t min = COOLING_TEMP_MAX_C;
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        min = MAINBOARD_MIN(min, hcoolingtemp.temperatures[i]);
    }
    return min;
}

celsius_t cooling_temp_get_max(void) {
    celsius_t max = 0U;
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        max = MAINBOARD_MAX(max, hcoolingtemp.temperatures[i]);
    }
    return max;
}

celsius_t cooling_temp_get_sum(void) {
    celsius_t sum = 0U;
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        sum += hcoolingtemp.temperatures[i];
    }
    return sum;
}

celsius_t cooling_temp_get_avg(void) {
    return cooling_temp_get_sum() / COOLING_TEMP_COUNT;
}

primary_hv_cells_temp_converted_t * cooling_temp_get_temperatures_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hcoolingtemp.cooling_temp_can_payload);

    static size_t index = 0U;
    const celsius_t * temperatures = hcoolingtemp.temperatures;
    hcoolingtemp.cooling_temp_can_payload.start_index = index;
    switch (index) {
        case 0U:
            hcoolingtemp.cooling_temp_can_payload.temp_0 = temperatures[COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE];
            hcoolingtemp.cooling_temp_can_payload.temp_1 = temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_1];
            hcoolingtemp.cooling_temp_can_payload.temp_2 = temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_2];
            hcoolingtemp.cooling_temp_can_payload.temp_3 = temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3];
            break;
        case 1U:
            hcoolingtemp.cooling_temp_can_payload.temp_0 = temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_4];
            hcoolingtemp.cooling_temp_can_payload.temp_1 = temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_5];
            hcoolingtemp.cooling_temp_can_payload.temp_2 = temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_6];
            hcoolingtemp.cooling_temp_can_payload.temp_3 = 0U;
            break;
    }
    if (++index >= 2U)
        index = 0U;
    return &hcoolingtemp.cooling_temp_can_payload;
}

#ifdef CONF_COOLING_TEMPERATURE_STRINGS_ENABLE

_STATIC char * cooling_temp_module_name = "cooling temperature";

_STATIC char * cooling_temp_return_code_name[] = {
    [COOLING_TEMP_OK] = "ok",
    [COOLING_TEMP_NULL_POINTER] = "null pointer",
    [COOLING_TEMP_OUT_OF_BOUNDS] = "out of bounds"
};

_STATIC char * cooling_temp_return_code_description[] = {
    [COOLING_TEMP_OK] = "executed successfully",
    [COOLING_TEMP_NULL_POINTER] = "attempt to dereference a null pointer"
    [COOLING_TEMP_OUT_OF_BOUNDS] = "attempt to access an invalid memory region"
};

#endif  //  CONF_COOLING_TEMPERATURE_STRINGS_ENABLE

#endif  //  CONF_COOLING_TEMPERATURE_MODULE_ENABLE
