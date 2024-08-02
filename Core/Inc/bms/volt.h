/**
 * @file volt.h
 * @date 2024-04-20
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Voltage measurment and control
 */

#ifndef VOLT_H
#define VOLT_H

#include <stddef.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

#include "primary_network.h"
#include "bms_network.h"

/** @brief Minimum and maximum allowed cell voltage in millivolt */
#define VOLT_MIN_MILLIVOLT ((millivolt_t)2800.f)
#define VOLT_MAX_MILLIVOLT ((millivolt_t)4200.f)

/** @brief Minimum and maximum allowed cell voltage raw values */
#define VOLT_MIN_VALUE (VOLT_MILLIVOLT_TO_VALUE(VOLT_MIN_MILLIVOLT))
#define VOLT_MAX_VALUE (VOLT_MILLIVOLT_TO_VALUE(VOLT_MAX_MILLIVOLT))

/**
 * @brief Convert a raw voltage value to millivolt
 *
 * @param value The value to convert
 *
 * @return millivolt_t The converted voltage in millivolt
 */
#define VOLT_VALUE_TO_MILLIVOLT(value) ((millivolt_t)(value / 10.f))
/**
 * @brief Convert a voltage in millivolt to the raw voltage value
 *
 * @param value The value in millivolt to convert
 *
 * @return raw_volt_t The raw voltage value
 */
#define VOLT_MILLIVOLT_TO_VALUE(value) ((raw_volt_t)(value * 10.f))

/**
 * @brief Return code for the voltage module functions
 *
 * @details
 *     - VOLT_OK the function executed successfully
 *     - VOLT_NULL_POINTER a NULL pointer is given as parameter or used inside the function
 *     - VOLT_OUT_OF_BOUNDS an index (or pointer) value is greater/lower than the maximum/minimum allowed value
 */
typedef enum {
    VOLT_OK,
    VOLT_NULL_POINTER,
    VOLT_OUT_OF_BOUNDS
} VoltReturnCode;

/**
 * @brief Type definition for a matrix of raw voltage values
 *
 * @details The matrix contains a row for each cellboard and every column contains
 * the i-th voltage of each segment
 */
typedef raw_volt_t volt_matrix_t[CELLBOARD_COUNT][CELLBOARD_SEGMENT_SERIES_COUNT];

#ifdef CONF_VOLTAGE_MODULE_ENABLE

/**
 * @brief Initialize the voltage module
 *
 * @return VoltReturnCode
 *     - VOLT_OK
 */
VoltReturnCode volt_init(void);

/**
 * @brief Get a pointer to the array where the voltage values are stored
 *
 * @return volt_matrix_t* The pointer to the array
 */
const volt_matrix_t * volt_get_values(void);

/**
 * @brief Hanle the received cellboard cells voltage
 *
 * @param payload A pointer to the canlib payload
 */
void volt_cells_voltage_handle(bms_cellboard_cells_voltage_converted_t * payload);

/**
 * @brief Get a pointer to the CAN payload of the cells voltages
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_cellboard_cells_voltage_converted_t* A pointer to the payload
 */
primary_hv_cells_voltage_converted_t * volt_get_canlib_payload(size_t * byte_size);

#else  // CONF_VOLTAGE_MODULE_ENABLE

#define volt_init() (VOLT_OK)
#define volt_get_values() (NULL)
#define volt_cells_voltage_handle(payload) MAINBOARD_NOPE()
#define volt_get_canlib_payload(byte_size) (NULL)

#endif  // CONF_VOLTAGE_MODULE_ENABLE

#endif  // VOLT_H
