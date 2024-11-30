/**
 * @file temp.h
 * @date 2024-08-30
 * @author Riccardo Segala [riccardo.segala@icloud.com]
 *
 * @brief Temperature measurment and control
 */

#ifndef TEMP_H
#define TEMP_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "primary_network.h"
#include "bms_network.h"

/** @brief Minimum and maximum allowed cell temperature in celsius */
#define TEMP_MIN_C (-10.f)
#define TEMP_MAX_C (60.f)

/** @brief Number of temperatures sent in a single temp can message */
#define TEMP_NUM_TEMP_CAN_MESSAGE (4U)

/**
 * @brief Return code for the temperature module functions
 *
 * @details
 *     - TEMP_OK the function executed successfully
 *     - TEMP_BUSY the module is busy and cannot execute the requested function
 *     - TEMP_NULL_POINTER a NULL pointer is given as parameter or used inside the function
 *     - TEMP_OUT_OF_BOUNDS an index (or pointer) value is greater/lower than the maximum/minimum allowed value
 */
typedef enum {
    TEMP_OK,
    TEMP_NULL_POINTER,
    TEMP_BUSY,
    TEMP_OUT_OF_BOUNDS
} TempReturnCode;

/**
 * @brief Type definition for a matrix of cells temperatures in °C
 *
 * @details The matrix contains a row for each cellboard and every column contains
 * the i-th voltage of each segment
 */
typedef celsius_t cells_temp_t[CELLBOARD_COUNT][CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT];

/**
 * @brief Type definition for the temperature module handler structure
 *
 * @param temperatures The array of temperatures in °C
 * @param cellboard_id The cellboard identifier used when the canlib payload is sent
 * @param offset An offset used when the canlib payload is sent
 * @param temp_can_payload The canlib message payload for the cells temperatures
 */
typedef struct {
    cells_temp_t temperatures;

    CellboardId cellboard_id;
    size_t offset; 
    primary_hv_cells_temperature_converted_t temp_can_payload;
    primary_hv_cells_temp_stats_converted_t temp_stats_can_payload;
} _TempHandler;

#ifdef CONF_TEMPERATURE_MODULE_ENABLE

/**
 * @brief Initialize the temperature module
 *
 * @return TempReturnCode
 *     - TEMP_OK
 */
TempReturnCode temp_init(void);

/**
 * @brief Get a pointer to the array where the temperature values are stored
 *
 * @return cells_temp_t* The pointer to the array
 */
const cells_temp_t * temp_get_values(void);

/**
 * @brief Get the minimum cell temperature in the pack
 *
 * @return celsius_t The minimum temperature value in °C
 */
celsius_t temp_get_min(void);

/**
 * @brief Get the maximum cell temperature in the pack
 *
 * @return celsius_t The maximum temperature value in °C
 */
celsius_t temp_get_max(void);

/**
 * @brief Get the sum of the cells temperatures of the pack
 *
 * @return celsius_t The sum of the temperatures in °C
 */
celsius_t temp_get_sum(void);

/**
 * @brief Get the average cell temperature of the pack
 *
 * @return celsius_t The average temperature in °C
 */
celsius_t temp_get_avg(void);

/**
 * @brief Handle the received cellboard cells temperature
 *
 * @param payload A pointer to the canlib payload
 */
void temp_cells_temperature_handle(bms_cellboard_cells_temperature_converted_t * const payload);

/**
 * @brief Get a pointer to the CAN payload of the cells temperature
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_cellboard_cells_temperature_converted_t* A pointer to the payload
 */
primary_hv_cells_temperature_converted_t * temp_get_cells_temperature_canlib_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload of the cells temperature stats
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_cellboard_cells_temp_stats_converted_t* A pointer to the payload
 */
primary_hv_cells_temp_stats_converted_t * temp_get_cells_temperature_stats_canlib_payload(size_t * const byte_size);

#else

#define temp_init() (TEMP_OK)
#define temp_get_values() (NULL)
#define temp_get_min() (NULL)
#define temp_get_max() (NULL)
#define temp_get_avg() (NULL)
#define temp_cells_temperature_handle(payload) MAINBOARD_NOPE()
#define temp_get_cells_temperature_canlib_payload(byte_size) (NULL)
#define temp_get_cells_temperature_stats_canlib_payload(byte_size) (NULL)

#endif  // CONF_TEMPERATURE_MODULE_ENABLE

#endif  // TEMP_H
