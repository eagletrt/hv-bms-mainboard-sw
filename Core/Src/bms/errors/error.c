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
    [ERROR_GROUP_OVER_CURRENT] = ERROR_OVER_CURRENT_INSTANCE_COUNT
};

/**
 * @brief Error thresholds for each group
 * 
 * @details The values are arbitrary and shuold not be too much high 
 */
const size_t thresholds[] = {
    [ERROR_GROUP_POST] = 1U,
    [ERROR_GROUP_OVER_CURRENT] = 3U
};

int32_t error_post_instances[ERROR_POST_INSTANCE_COUNT];
int32_t error_over_current_instances[ERROR_OVER_CURRENT_INSTANCE_COUNT];
int32_t * errors[] = {
    [ERROR_GROUP_POST] = error_post_instances,
    [ERROR_GROUP_OVER_CURRENT] = error_over_current_instances
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

ErrorReturnCode error_set(ErrorGroup group, error_instance_t instance) {
    if (errorlib_error_set(&herror, (errorlib_error_group_t)group, instance) != ERRORLIB_OK)
        return ERROR_UNKNOWN;
    return ERROR_OK;
}

ErrorReturnCode error_reset(ErrorGroup group, error_instance_t instance) {
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
