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

/** @brief Minimum and maximum allowed cell voltage in V */
#define VOLT_MIN_V (2.8f)
#define VOLT_MAX_V (4.2f)

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
 * @brief Type definition for a the matrix of cells voltages in V
 *
 * @details The matrix contains a row for each cellboard and every column contains
 * the i-th voltage of each segment
 */
typedef volt_t cells_voltage_t[CELLBOARD_COUNT][CELLBOARD_SEGMENT_SERIES_COUNT];

/**
 * @brief Voltages handler structure
 *
 * @warning This structure should never be used outside of this file
 *
 * @param voltages The array of cells voltages in V
 * @param volt_can_payload The canlib payload of the cells voltages
 * @param cellboard_id Cellboard identifier to set inside the payload
 * @param offset Cell offset to set inside the payload
 */
typedef struct {
    cells_voltage_t voltages;

    CellboardId cellboard_id;
    size_t offset;
    primary_hv_cells_voltage_converted_t volt_can_payload;
    primary_hv_cells_voltage_stats_converted_t volt_stats_can_payload;
} _VoltHandler;

#ifdef CONF_VOLTAGE_MODULE_ENABLE

/**
 * @brief Initialize the voltage module
 *
 * @return VoltReturnCode
 *     - VOLT_OK
 */
VoltReturnCode volt_init(void);

/**
 * @brief Get a pointer to the array where the voltages are stored
 *
 * @return cells_voltage_t* The pointer to the array
 */
const cells_voltage_t * volt_get_values(void);

/**
 * @brief Get the minimum cell voltage in the pack
 *
 * @return volt_t The minimum voltage in V
 */
volt_t volt_get_min(void);

/**
 * @brief Get the maximum cell voltage in the pack
 *
 * @return volt_t The maximum voltage in V
 */
volt_t volt_get_max(void);

/**
 * @brief Get the average cell voltage of the pack
 *
 * @return volt_t The average voltage in V
 */
volt_t volt_get_avg(void);

/**
 * @brief Get the sum of the cells voltages of the pack
 *
 * @return volt_t The cells voltages sum
 */
volt_t volt_get_sum(void);

/**
 * @brief Handle the received cellboard cells voltage
 *
 * @param payload A pointer to the canlib payload
 */
void volt_cells_voltage_handle(bms_cellboard_cells_voltage_converted_t * const payload);

/**
 * @brief Get a pointer to the CAN payload of the cells voltages
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_cellboard_cells_voltage_converted_t* A pointer to the payload
 */
primary_hv_cells_voltage_converted_t * volt_get_cells_voltage_canlib_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload of the cells voltage stats
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_cellboard_cells_voltage_stats_converted_t* A pointer to the payload
 */
primary_hv_cells_voltage_stats_converted_t * volt_get_cells_voltage_stats_canlib_payload(size_t * const byte_size);

#else  // CONF_VOLTAGE_MODULE_ENABLE

#define volt_init() (VOLT_OK)
#define volt_get_values() (NULL)
#define volt_get_min() (VOLT_MAX_VALUE)
#define volt_get_max() (VOLT_MAX_VALUE)
#define volt_get_avg() (VOLT_MAX_VALUE)
#define volt_get_sum() (VOLT_VALUE_TO_VOLT(VOLT_MAX_VALUE))
#define volt_cells_voltage_handle(payload) MAINBOARD_NOPE()
#define volt_get_cells_voltage_canlib_payload(byte_size) (NULL)
#define volt_get_cells_voltage_stats_canlib_payload(byte_size) (NULL)

#endif  // CONF_VOLTAGE_MODULE_ENABLE

#endif  // VOLT_H
