/**
 * @file error.c
 * @date 2024-07-12
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Simple wrapper for the error handler generated code
 */

#include "error.h"

#include <string.h>

#ifdef CONF_ERROR_MODULE_ENABLE

_STATIC ErrorLibHandler herror;

/** @brief Total number of instances for each group */
const size_t instances[] = {
    [ERROR_GROUP_POST] = ERROR_POST_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_CURRENT] = ERROR_OVER_CURRENT_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_POWER] = ERROR_OVER_POWER_INSTANCE_COUNT,
    [ERROR_GROUP_UNDER_VOLTAGE] = ERROR_UNDER_VOLTAGE_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_VOLTAGE] = ERROR_OVER_VOLTAGE_INSTANCE_COUNT,
    [ERROR_GROUP_UNDER_TEMPERATURE] = ERROR_UNDER_TEMPERATURE_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_TEMPERATURE] = ERROR_OVER_TEMPERATURE_INSTANCE_COUNT,
    [ERROR_GROUP_CAN_COMMUNICATION] = ERROR_CAN_COMMUNICATION_INSTANCE_COUNT
};

/**
 * @brief Error thresholds for each group
 * 
 * @details The values are arbitrary and shuold not be too much high 
 */
const size_t thresholds[] = {
    [ERROR_GROUP_POST] = 1U,
    [ERROR_GROUP_OVER_CURRENT] = 2U,
    [ERROR_GROUP_OVER_POWER] = 2U,
    [ERROR_GROUP_UNDER_VOLTAGE] = 3U,
    [ERROR_GROUP_OVER_VOLTAGE] = 3U,
    [ERROR_GROUP_UNDER_TEMPERATURE] = 5U,
    [ERROR_GROUP_OVER_TEMPERATURE] = 5U,
    [ERROR_GROUP_CAN_COMMUNICATION] = 5U
};

int32_t error_post_instances[ERROR_POST_INSTANCE_COUNT];
int32_t error_over_current_instances[ERROR_OVER_CURRENT_INSTANCE_COUNT];
int32_t error_over_power_instances[ERROR_OVER_POWER_INSTANCE_COUNT];
int32_t error_under_voltage_instances[ERROR_UNDER_VOLTAGE_INSTANCE_COUNT];
int32_t error_over_voltage_instances[ERROR_OVER_VOLTAGE_INSTANCE_COUNT];
int32_t error_under_temperature_instances[ERROR_UNDER_TEMPERATURE_INSTANCE_COUNT];
int32_t error_over_temperature_instances[ERROR_OVER_TEMPERATURE_INSTANCE_COUNT];
int32_t error_can_communication_instances[ERROR_CAN_COMMUNICATION_INSTANCE_COUNT];
int32_t * errors[] = {
    [ERROR_GROUP_POST] = error_post_instances,
    [ERROR_GROUP_OVER_CURRENT] = error_over_current_instances,
    [ERROR_GROUP_OVER_POWER] = error_over_power_instances,
    [ERROR_GROUP_UNDER_VOLTAGE] = error_under_voltage_instances,
    [ERROR_GROUP_OVER_VOLTAGE] = error_over_voltage_instances,
    [ERROR_GROUP_UNDER_TEMPERATURE] = error_under_temperature_instances,
    [ERROR_GROUP_OVER_TEMPERATURE] = error_over_temperature_instances,
    [ERROR_GROUP_CAN_COMMUNICATION] = error_can_communication_instances
};

ErrorReturnCode error_init(void) {
    if (errorlib_init(&herror,
        errors,
        instances,
        thresholds,
        ERROR_GROUP_COUNT
    ) != ERRORLIB_OK)
        return ERROR_UNKNOWN;
    return ERROR_OK;
}

ErrorReturnCode error_set(const ErrorGroup group, const error_instance_t instance) {
    if (errorlib_error_set(&herror, (errorlib_error_group_t)group, instance) != ERRORLIB_OK)
        return ERROR_UNKNOWN;
    return ERROR_OK;
}

ErrorReturnCode error_reset(const ErrorGroup group, const error_instance_t instance) {
    if (errorlib_error_reset(&herror, (errorlib_error_group_t)group, instance) != ERRORLIB_OK)
        return ERROR_UNKNOWN;
    return ERROR_OK;
}

size_t error_get_expired(void) {
    return errorlib_get_expired(&herror);
}

#ifdef CONF_ERROR_STRINGS_ENABLE

_STATIC char * error_module_name = "error";

_STATIC char * error_return_code_name[] = {
    [ERROR_OK] = "ok",
    [ERROR_NULL_POINTER] = "null pointer"
}

_STATIC char * error_return_code_description[] = {
    [ERROR_OK] = "executed succesfully",
    [ERROR_NULL_POINTER] = "attempt to dereference a null pointer"
}

#endif // CONF_ERROR_STRINGS_ENABLE

#endif // CONF_ERROR_MODULE_ENABLE
