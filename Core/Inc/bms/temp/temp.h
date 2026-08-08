/*!
 * \file temp.h
 * \date 2026-05-22
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Temperature measurment and control
 */

#ifndef TEMP_H
#define TEMP_H

#include "mainboard-def.h"
#include "can-primary.h"

/*! \brief Minimum and maximum allowed cell temperature in celsius */
#define TEMP_MIN_C (-10.f)
#define TEMP_MAX_C (60.f)

/*! \brief Number of temperatures sent in a single temp can message */
#define TEMP_TEMPERATURE_PER_MESSAGE_COUNT (4U)

/*!
 * \brief Return code for the temperature module functions
 */
enum TempReturnCode {
    TEMP_RC_OK,           /*!< The function executed successfully */
    TEMP_RC_NULL_POINTER, /*!< A NULL pointer is given as parameter or used inside the function */
    TEMP_RC_BUSY,         /*!< The module is busy and cannot execute the requested function */
    TEMP_RC_OUT_OF_BOUNDS /*!< An index (or pointer) value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief Type definition for a matrix of cells temperatures in °C
 *
 * \details The matrix contains a row for each cellboard and every column contains
 * the i-th voltage of each segment
 */
typedef celsius_t cells_temp[CELLBOARD_COUNT][CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT];

/*!
 * \brief Type definition for the temperature module handler structure
 */
struct TempHandler {
    cells_temp temperatures; /*!< The array of temperatures in °C */

    union CanPrimaryMessages libcan_message_temperature_info; /*!< The canlib message payload for the cells temperature stats */

    union CanPrimaryMessages libcan_message_cellboard1; /*!< The CAN payload for Cellboard 1 cell temperatures */
    union CanPrimaryMessages libcan_message_cellboard2; /*!< The CAN payload for Cellboard 2 cell temperatures */
    union CanPrimaryMessages libcan_message_cellboard3; /*!< The CAN payload for Cellboard 3 cell temperatures */
    union CanPrimaryMessages libcan_message_cellboard4; /*!< The CAN payload for Cellboard 4 cell temperatures */
    union CanPrimaryMessages libcan_message_cellboard5; /*!< The CAN payload for Cellboard 5 cell temperatures */
    union CanPrimaryMessages libcan_message_cellboard6; /*!< The CAN payload for Cellboard 6 cell temperatures */
};

#endif // TEMP_H
