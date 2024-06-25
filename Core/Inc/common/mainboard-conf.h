/**
 * @file mainboard-conf.h
 * @date 2024-06-19
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Main configuration for the mainboard project
 *
 * @attention Do not touch unless you known whay you are doing
 */

#ifndef MAINBOARD_CONF_H
#define MAINBOARD_CONF_H

#include "mainboard-def.h"

/*** ######################### LIBRARIES MACROS ########################## ***/

/**
 * @defgroup libmacros
 * @brief Macros used by the libraries included inside the project
 */
#define bms_NETWORK_IMPLEMENTATION
#define primary_NETWORK_IMPLEMENTATION

/** @} */

/*** ######################### MODULE SELECTION ########################## ***/

/**
 * @defgroup modules
 * @brief Enable or disable the internal modules of the project
 * {@
 */
#define CONF_CAN_COMM_MODULE_ENABLE
#define CONF_TIMEBASE_MODULE_ENABLE
#define CONF_TASKS_MODULE_ENABLE
#define CONF_WATCHDOG_MODULE_ENABLE

/** @} */

/*** ######################### STRINGS INFORMATION ####################### ***/

/**
 * @defgroup strings
 * @brief Add useful strings that can be printed to get more info
 * {@
 */
// #define CONF_CAN_COMM_STRINGS_ENABLE
// #define CONF_TIMEBASE_STRINGS_ENABLE
// #define CONF_TASKS_STRINGS_ENABLE
// #define CONF_WATCHDOG_STRINGS_ENABLE

/** @} */

#endif  // MAINBOARD_CONF_H
