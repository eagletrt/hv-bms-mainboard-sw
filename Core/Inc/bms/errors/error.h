/*!
 * \file error.h
 * \date 2024-07-12
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \brief Simple wrapper for the error handler generated code
 */

#ifndef ERROR_H
#define ERROR_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "errorlib.h"
#include "eagletrt-api.h"

#include "bms_network.h"

#include "primary_network.h"

/*! \brief Error instances count for each group */
#define ERROR_POST_INSTANCE_COUNT (1U)
#define ERROR_OVER_CURRENT_INSTANCE_COUNT (1U)
#define ERROR_OVER_POWER_INSTANCE_COUNT (1U)
#define ERROR_UNDER_VOLTAGE_INSTANCE_COUNT (CELLBOARD_SERIES_COUNT)
#define ERROR_OVER_VOLTAGE_INSTANCE_COUNT (CELLBOARD_SERIES_COUNT)
#define ERROR_UNDER_TEMPERATURE_INSTANCE_COUNT (CELLBOARD_TEMP_SENSOR_COUNT)
#define ERROR_OVER_TEMPERATURE_INSTANCE_COUNT (CELLBOARD_TEMP_SENSOR_COUNT)
#define ERROR_CAN_COMMUNICATION_INSTANCE_COUNT (CAN_NETWORK_COUNT)
#define ERROR_CURRENT_SENSOR_COMMUNICATION_INSTANCE_COUNT (1U)
#define ERROR_COOLING_UNDER_TEMPERATURE_INSTANCE_COUNT (COOLING_TEMP_SENSOR_COUNT)
#define ERROR_COOLING_OVER_TEMPERATURE_INSTANCE_COUNT (COOLING_TEMP_SENSOR_COUNT)
#define ERROR_CELLBOARD_ERROR_INSTANCE_COUNT (CELLBOARD_COUNT)

/*! \brief Type redefinition for an error instance */
typedef errorlib_error_instance_t error_instance;

/*!
 * \brief Return code for the error module functions
 */
enum ErrorReturnCode {
    ERROR_RC_OK,           /*!< The function executed succesfully */
    ERROR_RC_NULL_POINTER, /*!< A NULL pointer was given to a function */
    ERROR_RC_UNKNOWN       /*!< Unknown error */
};

/*!
 * \brief Type of the error that categorize a group of instances
 */
enum ErrorGroup {
    ERROR_GROUP_POST,                         /*!< The Power On Self Test procedure failed */
    ERROR_GROUP_OVER_CURRENT,                 /*!< no description */
    ERROR_GROUP_OVER_POWER,                   /*!< The power must not exceed 80 kW */
    ERROR_GROUP_UNDER_VOLTAGE,                /*!< no description */
    ERROR_GROUP_OVER_VOLTAGE,                 /*!< no description */
    ERROR_GROUP_UNDER_TEMPERATURE,            /*!< no description */
    ERROR_GROUP_OVER_TEMPERATURE,             /*!< no description */
    ERROR_GROUP_CAN_COMMUNICATION,            /*!< no description */
    ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION, /*!< no description */
    ERROR_GROUP_COOLING_UNDER_TEMPERATURE,    /*!< no description */
    ERROR_GROUP_COOLING_OVER_TEMPERATURE,     /*!< no description */
    ERROR_GROUP_CELLBOARD_ERROR,              /*!< no description */
    ERROR_GROUP_COUNT                         /*!< The number of error groups */
};

/*!
 * \brief Type of the CAN communication error instances
 */
enum ErrorCanCommunicationInstance {
    ERROR_CAN_COMMUNICATION_INSTANCE_BMS,      /*!< an error occurred in the BMS CAN communication */
    ERROR_CAN_COMMUNICATION_INSTANCE_PRIMARY,  /*!< an error occurred in the primary CAN communication */
    ERROR_CAN_COMMUNICATION_INSTANCE_SECONDARY /*!< an error occurred in the secondary CAN communication */
};

#endif // ERROR_H
