/**
 * @file cooling-temp.h
 * @date 2024-09-24
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Cooling temperature measurment and control
 */

#ifndef COOLING_TEMP_H
#define COOLING_TEMP_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "primary_network.h"
#include "bms_network.h"

/** @brief Total number of cooling temperatures handled */
#define COOLING_TEMP_COUNT (COOLING_TEMP_INDEX_COUNT)

/** @brief Minimum and maximum allowed cooling temperature in celsius */
// TODO: Set the allowed temperature range
#define COOLING_TEMP_MIN_C (-10.f)
#define COOLING_TEMP_MAX_C (60.f)

/**
 * @brief Return code for the cooling temperature module functions
 *
 * @details
 *     - COOLING_TEMP_OK the function executed successfully
 *     - COOLING_TEMP_BUSY the module is busy and cannot execute the requested function
 *     - COOLING_TEMP_NULL_POINTER a NULL pointer is given as parameter or used inside the function
 *     - COOLING_TEMP_OUT_OF_BOUNDS an index (or pointer) value is greater/lower than the maximum/minimum allowed value
 */
typedef enum {
    COOLING_TEMP_OK,
    COOLING_TEMP_NULL_POINTER,
    COOLING_TEMP_BUSY,
    COOLING_TEMP_OUT_OF_BOUNDS
} CoolingTempReturnCode;

/**
 * Indices of the cooling temperatures
 *
 * @details Temperatures of the inlet and outlet liquid of the cooling
 */
typedef enum {
    COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE,
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_1,
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_2,
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3,
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_4,
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_5,
    COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_6,
    COOLING_TEMP_INDEX_COUNT
} CoolingTempIndex;

/**
 * @brief Type definition for an array of cooling temperatures in °C
 */
typedef celsius_t cooling_temp_t[COOLING_TEMP_COUNT];

/**
 * @brief Type definition for the cooling temperatures module handler structure
 *
 * @param temperatures The array of temperatures in °C
 */
typedef struct {
    cooling_temp_t temperatures;
} _TempHandler;

#ifdef CONF_COOLING_TEMPERATURE_MODULE_ENABLE

/**
 * @brief Initialize the cooling temperature module
 *
 * @return CoolingTempReturnCode
 *     - COOLING_TEMP_OK
 */
CoolingTempReturnCode cooling_temp_init(void);

/**
 * @brief Get a pointer to the array where the temperature values are stored
 *
 * @return cooling_temp_t* The pointer to the array
 */
const cooling_temp_t * cooling_temp_get_values(void);

#else

#define cooling_temp_init() (TEMP_OK)
#define cooling_temp_get_values() (NULL)

#endif  // CONF_COOLING_TEMPERATURE_MODULE_ENABLE

#endif  // COOLING_TEMP_H
