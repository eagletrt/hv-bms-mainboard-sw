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

typedef errorlib_error_instance_t error_instance_t;

/**
 * @brief Return code for the error module functions
 *
 * @details
 *     - ERROR_OK the function executed succesfully
 *     - ERROR_NULL_POINTER a NULL pointer was given to a function
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
 */
typedef enum {
    ERROR_GROUP_POST,
    ERROR_GROUP_OVER_CURRENT,
    ERROR_GROUP_COUNT
} ErrorGroup;

#ifdef CONF_ERROR_MODULE_ENABLE

/**
 * @brief Initialization of the internal error handler structure
 */
ErrorReturnCode error_init(void);
ErrorReturnCode error_set(ErrorGroup group, error_instance_t instance);
ErrorReturnCode error_reset(ErrorGroup group, error_instance_t instance);
size_t error_get_expired(void);

#else  // CONF_ERROR_MODULE_ENABLE

#define error_init() (ERROR_OK)
#define error_set(group, instance) (ERROR_OK)
#define error_reset(group, instance) (ERROR_OK)
#define error_get_expired() (0U)

#endif // CONF_ERROR_MODULE_ENABLE

#endif  // ERROR_H
