/**
 * @file current.h
 * @date 2024-06-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Current measurement and control
 */

#ifndef CURRENT_H
#define CURRENT_H

#include "mainboard-conf.h"
#include "mainboard-def.h"

#include "bms_network.h"
#include "primary_network.h"

#include "watchdog.h"

/** @brief Minimum and maximum allowed current value in A */
#define CURRENT_MIN_A (-22.f)
#define CURRENT_MAX_A (130.f)

/**
 * @brief Maximum allowed power in kW
 *
 * @details FS-Rules 2024 v1.1
 *     Rule EV 2.2.1    The TS power at the outlet of the TSAC must not exceed 80 kW.
 */
#define CURRENT_MAX_POWER_KW (80.f)

/**
 * @brief Maximum allowed time delta between two messages of the current sensor
 *
 * @details FS-Rules 2024 v1.1
 *     Rule EV 5.8.7    The AMS must switch off the TS via the SDC, if a critical
 *                      voltage, temperature, or current value according to the
 *                      cell manufacturer’s datasheet or these rules persistently
 *                      occurs for more than:
 *                          - 500 ms for voltage and current values
 *                          - 1 s for temperature values
 */
#define CURRENT_SENSOR_COMMUNICATION_TIMEOUT_MS (200U)

/**
 * @brief Startup time of the current sensor in ms
 */
#define CURRENT_SENSOR_STARTUP_TIME_MS (400U)

/**
 * @brief Return code for the current module functions
 *
 * @details
 *     - CURRENT_OK the function executed succefully
 */
typedef enum {
    CURRENT_OK
} CurrentReturnCode;

/**
 * @brief Current handler structure
 *
 * @attention This structure should not be used outside of this module
 *
 * @param current The current value in A
 * @param sensor_wdg Watchdog used to check if the current sensor is connected
 * @param current_can_payload The canlib payload used to send the current value via CAN
 * @param power_can_payload The canlib payload used to send the power value via CAN
 */
typedef struct {
    ampere_t current;    

    Watchdog sensor_wdg;

    primary_hv_current_converted_t current_can_payload;
    primary_hv_power_converted_t power_can_payload;
} _CurrentHandler;

#ifdef CONF_CURRENT_MODULE_ENABLE

/**
 * @brief Initialize the internal current structure handler
 *
 * @return CurrentReturnCode
 *     - CURRENT_OK
 */
CurrentReturnCode current_init(void);

/**
 * @brief Get the supplied current in A
 *
 * @return ampere_t The current in A
 */
ampere_t current_get_current(void);

/**
 * @brief Get the calculated power value in kW
 *
 * @return kilowatt_t The power in kW
 */
kilowatt_t current_get_power(void);

/**
 * @brief Start the watchdog used to check for the current sensor communication
 *
 * @return WatchdogReturnCode
 *     - WATCHDOG_NULL_POINTER if the watchdog is NULL
 *     - WATCHDOG_BUSY if the watchdog is already running
 *     - WATCHDOG_TIMED_OUT if the watchdog has already timed out
 *     - WATCHDOG_UNAVAILABLE if the watchdog can't be registered
 *     - WATCHDOG_OK otherwise
 */
WatchdogReturnCode current_start_sensor_communication_watchdog(void);

/**
 * @brief Handle the received response from the current sensor
 *
 * @param payload A pointer to the canlib payload of the response
 */
void current_handle(bms_ivt_msg_result_i_t * const payload);

/**
 * @brief Get a pointer to the CAN payload of the current
 * 
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_current_converted_t* A pointer to the payload
 */
primary_hv_current_converted_t * current_get_current_canlib_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload of the power
 * 
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_power_converted_t* A pointer to the payload
 */
primary_hv_power_converted_t * current_get_power_canlib_payload(size_t * const byte_size);

#else  // CONF_CURRENT_MODULE_ENABLE

#define current_init() (CURRENT_OK)
#define current_get_current() (0.f)
#define current_get_power() (0.f)
#define current_start_sensor_communication_watchdog() (WATCHDOG_OK)
#define current_handle(payload) CELLBOARD_NOPE()
#define current_get_current_canlib_payload(byte_size) (NULL)
#define current_get_power_canlib_payload(byte_size) (NULL)

#endif // CONF_CURRENT_MODULE_ENABLE

#endif  // CURRENT_H
