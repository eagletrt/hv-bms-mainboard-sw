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

#include "mainboard-def.h"

/*!
 * \brief Structure to hold the voltage information for the entire pack
 */
struct VoltagesInfo {
    volt min;     /*!< The minimum voltage*/
    volt max;     /*!< The maximum voltage*/
    volt average; /*!< The average voltage*/
    volt sum;     /*!< The sum of the cells*/
};

/*!
 * \brief Structure to hold the voltage data for each cellboard
 */
struct VoltagesHandler {

    volt voltages[CELLBOARD_COUNT][CELLBOARD_SEGMENT_SERIES_COUNT]; /*!< The voltage values for each cellboard */
    volatile bool is_updated_voltages[CELLBOARD_COUNT];             /*!< Indicates if the voltages for a specific cellboard have been updated */

    struct VoltagesInfo cellboard_info[CELLBOARD_COUNT]; /*!< The voltage information for each cellboard */
    volatile bool is_updated_info[CELLBOARD_COUNT];      /*!< Indicates if the global voltage values have been updated */
};

#endif // VOLTAGES_H
