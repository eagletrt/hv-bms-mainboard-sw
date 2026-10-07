/*!
 * \file cooling-temp.h
 * \date 2024-09-24
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cooling temperature measurment and control
 */

#ifndef COOLING_TEMP_H
#define COOLING_TEMP_H

#include <stdint.h>
#include "can-primary.h"
#include "mainboard-def.h"

/*! \brief Total number of cooling temperatures handled */
#define COOLING_TEMP_COUNT (COOLING_TEMP_INDEX_COUNT)

/*! \brief Minimum and maximum allowed cooling temperature in celsius */
// TODO: Set the allowed temperature range
#define COOLING_TEMP_MIN_C (-10.f)
#define COOLING_TEMP_MAX_C (60.f)

/*!
 * \brief Minimum and maximum limit for the temperature voltages in V
 *
 * \details This limit is applied to fit into the polynomial conversion
 * to get a plausible temperature value
 */
#define COOLING_TEMP_MIN_LIMIT_V (0.f)
#define COOLING_TEMP_MAX_LIMIT_V (3.f)

/*! \brief Coefficients used for the polynomial conversion of the NTC cooling temperatures values */
#define COOLING_TEMP_COEFF_0 (162.9947f)
#define COOLING_TEMP_COEFF_1 (-289.0887f)
#define COOLING_TEMP_COEFF_2 (353.5436f)
#define COOLING_TEMP_COEFF_3 (-276.9361f)
#define COOLING_TEMP_COEFF_4 (125.8228f)
#define COOLING_TEMP_COEFF_5 (-30.5363f)
#define COOLING_TEMP_COEFF_6 (3.0394f)

/*!
 * \brief Return code for the cooling temperature module functions
 */
enum CoolingTempReturnCode {
    COOLING_TEMP_RC_OK,           /*!< Executed successfully */
    COOLING_TEMP_RC_NULL_POINTER, /*!< A NULL pointer is given as parameter or used inside the function */
    COOLING_TEMP_RC_BUSY,         /*!< The module is busy and cannot execute the requested function */
    COOLING_TEMP_RC_OUT_OF_BOUNDS /*!< An index (or pointer) value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief Indices of the cooling temperatures
 */
enum CoolingTempIndex : uint8_t {
    COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE,    /*<! Inlet liquid temperature */
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_1, /*<! Outlet liquid temperature 1 */
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_2, /*<! Outlet liquid temperature 2 */
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3, /*<! Outlet liquid temperature 3 */
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_4, /*<! Outlet liquid temperature 4 */
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_5, /*<! Outlet liquid temperature 5 */
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_6, /*<! Outlet liquid temperature 6 */
    COOLING_TEMP_INDEX_COUNT,                       /*<! Total number of cooling temperature values */
    COOLING_TEMP_INDEX_INVALID                      /*<! Total number of cooling temperature values */
};

/*!
 * \brief Type definition for an array of cooling temperatures in °C
 */
typedef celsius_t cooling_temps[COOLING_TEMP_COUNT];

/*!
 * \brief Type definition for the cooling temperatures module handler structure
 */
struct CoolingTempHandler {
    cooling_temps temperatures; /*!< Array of cooling temperatures in °C */

    union CanPrimaryMessages libcan_message_cooling1;
    union CanPrimaryMessages libcan_message_cooling2;
};

#endif // COOLING_TEMP_H
