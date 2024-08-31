/**
 * @file error.h
 * @date 2024-07-12
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Simple wrapper for the error handler generated code
 */

#ifndef ERROR_H
#define ERROR_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "errorlib.h"

/** @brief Error instances count for each group */
#define ERROR_POST_INSTANCE_COUNT (1U)
#define ERROR_OVER_CURRENT_INSTANCE_COUNT (1U)
#define ERROR_OVER_POWER_INSTANCE_COUNT (1U)
#define ERROR_UNDER_VOLTAGE_INSTANCE_COUNT (CELLBOARD_SERIES_COUNT)
#define ERROR_OVER_VOLTAGE_INSTANCE_COUNT (CELLBOARD_SERIES_COUNT)
#define ERROR_UNDER_TEMPERATURE_INSTANCE_COUNT (CELLBOARD_TEMP_SENSOR_COUNT)
#define ERROR_OVER_TEMPERATURE_INSTANCE_COUNT (CELLBOARD_TEMP_SENSOR_COUNT)
#define ERROR_CAN_COMMUNICATION_INSTANCE_COUNT (CAN_NETWORK_COUNT)

/** @brief Type redefinition for an error instance */
typedef errorlib_error_instance_t error_instance_t;

/**
 * @brief Return code for the error module functions
 *
 * @details
 *     - ERROR_OK the function executed succesfully
 *     - ERROR_NULL_POINTER a NULL pointer was given to a function
 *     - ERROR_UNKNOWN unknown error
 */
typedef enum {
    ERROR_OK,
    ERROR_NULL_POINTER,
    ERROR_UNKNOWN
} ErrorReturnCode;

/**
 * @brief Type of the error that categorize a group of instances
 *
 * @details
 *     - ERROR_GROUP_POST the Power On Self Test procedure failed
 *     - ERROR_GROUP_OVER_CURRENT no description
 *     - ERROR_GROUP_OVER_POWER The power must not exceed 80 kW
 *     - ERROR_GROUP_UNDER_VOLTAGE no description
 *     - ERROR_GROUP_OVER_VOLTAGE no description
 *     - ERROR_GROUP_UNDER_TEMPERATURE no description
 *     - ERROR_GROUP_OVER_TEMPERATURE no description
 *     - ERROR_GROUP_CAN_COMMUNICATION no description
 */
typedef enum {
    ERROR_GROUP_POST,
    ERROR_GROUP_OVER_CURRENT,
    ERROR_GROUP_OVER_POWER,
    ERROR_GROUP_UNDER_VOLTAGE,
    ERROR_GROUP_OVER_VOLTAGE,
    ERROR_GROUP_UNDER_TEMPERATURE,
    ERROR_GROUP_OVER_TEMPERATURE,
    ERROR_GROUP_CAN_COMMUNICATION,
    ERROR_GROUP_COUNT
} ErrorGroup;

typedef enum {
    ERROR_CAN_COMMUNICATION_INSTANCE_BMS,
    ERROR_CAN_COMMUNICATION_INSTANCE_PRIMARY,
    ERROR_CAN_COMMUNICATION_INSTANCE_SECONDARY
} ErrorCanCommunicationInstance;

#ifdef CONF_ERROR_MODULE_ENABLE

/**
 * @brief Initialization of the internal error handler structure
 */
ErrorReturnCode error_init(void);
ErrorReturnCode error_set(const ErrorGroup group, const error_instance_t instance);
ErrorReturnCode error_reset(const ErrorGroup group, const error_instance_t instance);
size_t error_get_expired(void);
ErrorInfo error_get_expired_info(void);

#ifdef CONF_ERROR_STRINGS_ENABLE

char * error_get_group_name_string(const ErrorGroup group);

#else  // CONF_ERROR_STRINGS_ENABLE

#define error_get_group_name_string(group) ("")

#endif // CONF_ERROR_STRINGS_ENABLE

#else  // CONF_ERROR_MODULE_ENABLE

#define error_init() (ERROR_OK)
#define error_set(group, instance) (ERROR_OK)
#define error_reset(group, instance) (ERROR_OK)
#define error_get_expired() (0U)

#endif // CONF_ERROR_MODULE_ENABLE

#endif  // ERROR_H
