/*!
 * \file imd.h
 * \date 2024-07-04
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Insulation Monitoring Device (IMD) handing functions
 */

#ifndef IMD_H
#define IMD_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "primary_network.h"

#include "ir1553204.h"

/*! \brief Type definition for the callback function that should start the PWM measurements */
typedef void (*imd_pwm_start_callback)(void);

/*!
 * \brief Return code for the IMD module functions
 */
enum ImdReturnCode {
    IMD_RC_OK = IR1553204_OK,                       /*!< Executed successfully */
    IMD_RC_NULL_POINTER = IR1553204_NULL_POINTER,   /*!< A NULL pointer was given to a function */
    IMD_RC_INVALID_DATA = IR1553204_RC_INVALID_DATA /*!< The given data is not valid */
};

/*!
 * \brief Type definition for the IMD status
 */
enum ImdStatus {
    IMD_STATUS_UNKNOWN = IR1553204_STATUS_UNKNOWN,             /*!< Unknown status */
    IMD_STATUS_SHORT_CIRCUIT = IR1553204_STATUS_SHORT_CIRCUIT, /*!< Short circuit detected */
    IMD_STATUS_NORMAL = IR1553204_STATUS_NORMAL,               /*!< Normal operation */
    IMD_STATUS_UNDER_VOLTAGE = IR1553204_STATUS_UNDER_VOLTAGE, /*!< Undervoltage detected */
    IMD_STATUS_START_MEASURE = IR1553204_STATUS_START_MEASURE, /*!< Measurement started */
    IMD_STATUS_DEVICE_ERROR = IR1553204_STATUS_DEVICE_ERROR,   /*!< Device error */
    IMD_STATUS_EARTH_FAULT = IR1553204_STATUS_EARTH_FAULT,     /*!< Earth fault detected */
    IMD_STATUS_COUNT = IR1553204_STATUS_COUNT
};

/*!
 * \brief Type definition for the IMD handler structure
 *
 * \attention Do not use this struct outside this module
 */
struct ImdHandler {
    imd_pwm_start_callback start; /*< A pointer to the callback used to start the PWM to read from the IMD */

    Ir1553204Handler ir1153204; /*< Handler structure of the IMD driver */

    primary_hv_imd_status_converted_t status_can_payload; /*< The canlib payload used to send the IMD status */
};

#endif // IMD_H
