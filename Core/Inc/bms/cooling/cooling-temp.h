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
 * @brief Minimum and maximum limit for the temperature voltages in V
 *
 * @details This limit is applied to fit into the polynomial conversion
 * to get a plausible temperature value
 */
#define COOLING_TEMP_MIN_LIMIT_V (0.f/** @todo put correct values */)
#define COOLING_TEMP_MAX_LIMIT_V (0.f/** @todo put correct values */)


/** @brief Coefficients used for the polynomial conversion of the NTC cooling temperatures values */
#define COOLING_TEMP_COEFF_0 (0.f /** @todo put correct values */)
#define COOLING_TEMP_COEFF_1 (0.f /** @todo put correct values */)
#define COOLING_TEMP_COEFF_2 (0.f /** @todo put correct values */)
#define COOLING_TEMP_COEFF_3 (0.f /** @todo put correct values */)
#define COOLING_TEMP_COEFF_4 (0.f /** @todo put correct values */)
#define COOLING_TEMP_COEFF_5 (0.f /** @todo put correct values */)
#define COOLING_TEMP_COEFF_6 (0.f /** @todo put correct values */)

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
} _CoolingTempHandler;

#ifdef CONF_COOLING_TEMPERATURE_MODULE_ENABLE

/**
 * @brief Initialize the cooling temperature module
 *
 * @return CoolingTempReturnCode
 *     - COOLING_TEMP_OK
 */
CoolingTempReturnCode cooling_temp_init(void);

/**
 * @brief Notify the cooling temperature module that the conversion is completed
 *
 * @param index The idx of the value to be upfated
 * @param value The voltage to copy in V
 */
CoolingTempReturnCode cooling_temp_notify_conversion_complete(size_t index, const volt_t value);

/**
 * @brief Update a single temperature value
 *
 * @param index The index of the value to update
 * @param value The new value
 *
 * @return TempReturnCode
 *     - COOLING_TEMP_OUT_OF_BOUNDS if the index is greater than the total number of values
 *     - COOLING_TEMP_OK otherwise
 */
CoolingTempReturnCode cooling_temp_update_value(const size_t index, const celsius_t value);

/**
 * @brief Get a pointer to the array where the temperature values are stored
 *
 * @return cooling_temp_t* The pointer to the array
 */
const cooling_temp_t * cooling_temp_get_values(void);


/**
 * @brief Get the minimum temperature in the cooling loop
 *
 * @return celsius_t The minimum temperature value in °C
 */
celsius_t cooling_temp_get_min(void);

/**
 * @brief Get the maximum temperature in the cooling loop
 *
 * @return celsius_t The maximum temperature value in °C
 */
celsius_t cooling_temp_get_max(void);

/**
 * @brief Get the sum of the temperatures of the cooling loop
 *
 * @return celsius_t The sum of the temperatures in °C
 */
celsius_t cooling_temp_get_sum(void);

/**
 * @brief Get the average temperature of the cooling loop
 *
 * @return celsius_t The average temperature in °C
 */
celsius_t cooling_temp_get_avg(void);

#else

#define cooling_temp_init() (COOLING_TEMP_OK)
#define cooling_temp_get_values() (NULL)
#define cooling_temp_get_min() (NULL)
#define cooling_temp_get_max() (NULL)
#define cooling_temp_get_avg() (NULL)

#endif  // CONF_COOLING_TEMPERATURE_MODULE_ENABLE

#endif  // COOLING_TEMP_H
