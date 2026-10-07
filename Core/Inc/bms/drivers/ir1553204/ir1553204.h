/*!
 * \file ir1553204.h
 * \date 2024-07-03
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Driver for the IR155-3204 Insulation Monitoring Device (IMD)
 */

#ifndef IR1553204_H
#define IR1553204_H

#include <stdint.h>

#include "mainboard-def.h"

/*!
 * \brief Return code for the IR1553204 module functions
 */
enum Ir1553204ReturnCode {
    IR1553204_RC_OK,           /*!< Function executed successfully */
    IR1553204_RC_NULL_POINTER, /*!< A NULL pointer was given to a function */
    IR1553204_RC_INVALID_DATA  /*!< Invalid data was given to a function */
};

/*!
 * \brief Type definition for the IR1553204 status
 */
enum Ir1553204Status {
    IR1553204_STATUS_UNKNOWN = -1,  /*!< Unknown status */
    IR1553204_STATUS_SHORT_CIRCUIT, /*!< Short circuit detected */
    IR1553204_STATUS_NORMAL,        /*!< Normal operation */
    IR1553204_STATUS_UNDER_VOLTAGE, /*!< Undervoltage detected */
    IR1553204_STATUS_START_MEASURE, /*!< Measurement started */
    IR1553204_STATUS_DEVICE_ERROR,  /*!< Device error */
    IR1553204_STATUS_EARTH_FAULT,   /*!< Earth fault detected */
    IR1553204_STATUS_COUNT          /*!< Count of status values */
};

/*!
 * \brief IR155-3204 handler structure definition
 */
struct Ir1553204Handler {
    hertz_t frequency;               /*!< The frequency of the signal in Hz */
    precise_percentage_t duty_cycle; /*!< The duty cycle of the signal in % from 0 to 1 */
};

#endif // IR1553204_H
