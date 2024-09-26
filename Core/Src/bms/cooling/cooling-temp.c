/**
 * @file cooling-temp.h
 * @date 2024-09-24
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Cooling temperature measurment and control
 */

#include "cooling-temp.h"

#ifdef CONF_COOLING_TEMPERATURE_MODULE_ENABLE

_STATIC _CoolingTempHandler htemp;

CoolingTempReturnCode cooling_temp_init(void) {
    memset(&htemp, 0U, sizeof(htemp));
    return COOLING_TEMP_OK;
}

const cooling_temp_t * cooling_temp_get_values(void) {
    return &htemp.temperatures;
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
