/*!
 * \file internal-temperatures.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the internal temperatures measurement and control
 */

#ifndef INTERNAL_TEMPERATURES_H
#define INTERNAL_TEMPERATURES_H

#include <stdbool.h>

#include "mainboard-def.h"

/*!
 * \brief Return code for the internal temperatures module functions
 */
enum InternalTemperaturesReturnCode {
    INTERNAL_TEMPERATURES_RC_OK,           /*!< The function executed succesfully */
    INTERNAL_TEMPERATURES_RC_INVALID_INDEX /*!< The given temperature index is out of range */
};

/*!
 * \brief Internal temperatures measured by the mainboard
 */
enum InternalTemperaturesIndex {
    INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR, /*!< Temperature of the linear regulator mosfet */
    INTERNAL_TEMPERATURES_INDEX_PRECHARGE,        /*!< Temperature of the precharge resistors heatsink */
    INTERNAL_TEMPERATURES_INDEX_COOLING_INLET,    /*!< Inlet cooling liquid temperature */
    INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_1, /*!< Outlet cooling liquid temperature 1 */
    INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_2, /*!< Outlet cooling liquid temperature 2 */
    INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_3, /*!< Outlet cooling liquid temperature 3 */
    INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_4, /*!< Outlet cooling liquid temperature 4 */
    INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_5, /*!< Outlet cooling liquid temperature 5 */
    INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_6, /*!< Outlet cooling liquid temperature 6 */
    INTERNAL_TEMPERATURES_INDEX_COUNT             /*!< Total number of internal temperatures */
};

/*!
 * \brief Structure to hold the internal temperatures data
 *
 * \attention This structure should not be used outside of this module
 */
struct InternalTemperaturesHandler {
    celsius temperatures[INTERNAL_TEMPERATURES_INDEX_COUNT];                   /*!< The internal temperature values */
    volatile bool is_received_temperatures[INTERNAL_TEMPERATURES_INDEX_COUNT]; /*!< Indicates if each internal temperature has been received at least once, never reset */
};

#endif // INTERNAL_TEMPERATURES_H
