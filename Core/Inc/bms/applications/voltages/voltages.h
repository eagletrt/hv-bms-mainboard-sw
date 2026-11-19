/*!
 * \file voltages.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the cells voltage measurement and control
 */

#ifndef VOLTAGES_H
#define VOLTAGES_H

#include <stdbool.h>

#include "mainboard-def.h"

/*!
 * \brief Return code for the voltages module functions
 */
enum VoltagesReturnCode {
    VOLTAGES_RC_OK,                /*!< The function executed succesfully */
    VOLTAGES_RC_INVALID_CELLBOARD, /*!< The given cellboard does not exist */
    VOLTAGES_RC_INVALID_INDEX      /*!< The given voltage index is out of range */
};

/*!
 * \brief Structure to hold the voltage information of a group of cells
 */
struct VoltagesInfo {
    volt min;     /*!< The minimum voltage */
    volt max;     /*!< The maximum voltage */
    volt average; /*!< The average voltage */
    volt sum;     /*!< The sum of the cells voltages */
};

/*!
 * \brief Structure to hold the voltage data for each cellboard
 */
struct VoltagesHandler {
    volt voltages[CELLBOARD_COUNT][CELLBOARD_SEGMENT_SERIES_COUNT]; /*!< The voltage values for each cellboard */
    volatile bool is_updated_voltages[CELLBOARD_COUNT];             /*!< Indicates if the voltages for a specific cellboard have been updated */

    struct VoltagesInfo cellboard_info[CELLBOARD_COUNT]; /*!< The voltage information for each cellboard */
    volatile bool is_updated_info[CELLBOARD_COUNT];      /*!< Indicates if the voltage information for a specific cellboard has been updated */
    volatile bool is_received_info[CELLBOARD_COUNT];     /*!< Indicates if the voltage information of a specific cellboard has been received at least once, never reset */
};

#endif // VOLTAGES_H
