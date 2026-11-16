/*!
 * \file internal-voltages.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the internal voltages measurement and control
 */

#ifndef INTERNAL_VOLTAGES_H
#define INTERNAL_VOLTAGES_H

#include <stdbool.h>

#include "mainboard-def.h"

/*! \brief Conversion factor from W to kW */
#define INTERNAL_VOLTAGES_W_TO_KW (0.001F)

/*!
 * \brief Return code for the internal voltages module functions
 */
enum InternalVoltagesReturnCode {
    INTERNAL_VOLTAGES_RC_OK,           /*!< The function executed succesfully */
    INTERNAL_VOLTAGES_RC_INVALID_INDEX /*!< The given voltage index is out of range */
};

/*!
 * \brief Internal voltages measured by the mainboard
 */
enum InternalVoltagesIndex {
    INTERNAL_VOLTAGES_INDEX_TS,   /*!< The voltage of the Tractive System */
    INTERNAL_VOLTAGES_INDEX_PACK, /*!< The voltage of the battery pack */
    INTERNAL_VOLTAGES_INDEX_COUNT /*!< Total number of internal voltages */
};

/*!
 * \brief Structure to hold the internal electrical information of the pack
 */
struct InternalVoltagesInfo {
    volt ts;        /*!< The voltage of the Tractive System */
    volt pack;      /*!< The voltage of the battery pack */
    ampere current; /*!< The current supplied by the pack */
    kilowatt power; /*!< The power supplied at the TSAC outlet (current times TS voltage) */
};

/*!
 * \brief Structure to hold the internal voltages and current data
 *
 * \attention This structure should not be used outside of this module
 */
struct InternalVoltagesHandler {
    volt voltages[INTERNAL_VOLTAGES_INDEX_COUNT]; /*!< The internal voltage values */
    volatile bool is_updated_voltages;            /*!< Indicates if the internal voltages have been updated */

    ampere current;                   /*!< The current value */
    volatile bool is_updated_current; /*!< Indicates if the current has been updated */
};

#endif // INTERNAL_VOLTAGES_H
