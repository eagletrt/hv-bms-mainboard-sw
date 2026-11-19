/*!
 * \file temperatures.h
 * \date 2026-11-10
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the cells and discharge resistors temperature measurement and control
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
 * \brief Structure to hold the temperature information of a group of sensors
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
    celsius cell_temperatures[CELLBOARD_COUNT][CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT]; /*!< The cell temperature values for each cellboard */
    volatile bool is_updated_cell_temperatures[CELLBOARD_COUNT];                     /*!< Indicates if the cell temperatures for a specific cellboard have been updated */

    struct TemperaturesInfo cell_temperatures_info[CELLBOARD_COUNT];   /*!< The cell temperature information for each cellboard, as reported by the cellboard */
    volatile bool is_updated_cell_temperatures_info[CELLBOARD_COUNT];  /*!< Indicates if the cell temperature information for a specific cellboard has been updated */
    volatile bool is_received_cell_temperatures_info[CELLBOARD_COUNT]; /*!< Indicates if the cell temperature information of a specific cellboard has been received at least once, never reset */

    celsius discharge_temperatures[CELLBOARD_COUNT][CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT];                   /*!< The discharge resistor temperature values for each cellboard */
    volatile bool is_updated_discharge_temperatures[CELLBOARD_COUNT];                                          /*!< Indicates if the discharge resistor temperatures for a specific cellboard have been updated */
    volatile bool is_received_discharge_temperatures[CELLBOARD_COUNT][CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT]; /*!< Indicates if each discharge resistor temperature has been received at least once, never reset */
};

#endif // TEMPERATURES_H
