/*!
 * \file temperatures.h
 * \date 2026-11-10
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the cells temperature measurement and control
 */

#ifndef TEMPERATURES_H
#define TEMPERATURES_H

#include <stdbool.h>

#include "mainboard-def.h"

/*!
 * \brief Return code for the temperatures module functions
 */
enum TemperaturesReturnCode {
    TEMPERATURES_RC_OK,                /*!< The function executed succesfully */
    TEMPERATURES_RC_INVALID_CELLBOARD, /*!< The given cellboard does not exist */
    TEMPERATURES_RC_INVALID_INDEX      /*!< The given temperature index is out of range */
};

/*!
 * \brief Structure to hold the temperature information of a group of cells
 */
struct TemperaturesInfo {
    celsius min;     /*!< The minimum temperature */
    celsius max;     /*!< The maximum temperature */
    celsius average; /*!< The average temperature */
};

/*!
 * \brief Structure to hold the temperature data for each cellboard
 */
struct TemperaturesHandler {
    celsius temperatures[CELLBOARD_COUNT][CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT]; /*!< The temperature values for each cellboard */
    volatile bool is_updated_temperatures[CELLBOARD_COUNT];                     /*!< Indicates if the temperatures for a specific cellboard have been updated */

    struct TemperaturesInfo cellboard_info[CELLBOARD_COUNT]; /*!< The temperature information for each cellboard */
    volatile bool is_updated_info[CELLBOARD_COUNT];          /*!< Indicates if the temperature information for a specific cellboard has been updated */
};

#endif // TEMPERATURES_H
