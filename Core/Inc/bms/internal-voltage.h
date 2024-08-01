/**
 * @file internal-voltage.h
 * @date 2024-07-19
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Internal voltages measurements and controls
 */

#ifndef INTERNAL_VOLTAGE_H
#define INTERNAL_VOLTAGE_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "max22530.h"

/** @brief The period with which internal voltages are updated */
#define INTERNAL_VOLTAGE_CYCLE_TIME_MS ((milliseconds_t)8U)

/**
 * @brief Return code for the internal voltage module functions
 *
 * @details
 *     - INTERNAL_VOLTAGE_OK the function executed succesfully
 *     - INTERNAL_VOLTAGE_NULL_POINTER a NULL pointer was given to a function
 */
typedef enum {
    INTERNAL_VOLTAGE_OK,
    INTERNAL_VOLTAGE_NULL_POINTER
} InternalVoltageReturnCode;

/**
 * @brief Internal voltage channels of the external ADC
 *
 * @details
 *     - INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE The voltage of the TS
 *     - INTERNAL_VOLTAGE_CHANNEL_BATT_VOLTAGE The voltage of the pack
 *     - INTERNAL_VOLTAGE_CHANNEL_IMD_TS_CONNECTED Feedback on the TS connection to the IMD
 *     - INTERNAL_VOLTAGE_CHANNEL_PRECHARGE_TEMPERATURE Temperature of the precharge resistors heatsink
 */
typedef enum {
    INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE = MAX22530_CHANNEL_1,
    INTERNAL_VOLTAGE_CHANNEL_BATT_VOLTAGE = MAX22530_CHANNEL_2,
    INTERNAL_VOLTAGE_CHANNEL_IMD_TS_CONNECTED = MAX22530_CHANNEL_3,
    INTERNAL_VOLTAGE_CHANNEL_PRECHARGE_TEMPERATURE = MAX22530_CHANNEL_4,
    INTERNAL_VOLTAGE_CHANNEL_COUNT = MAX22530_CHANNEL_COUNT
} InternalVoltageChannel;

/**
 * @brief Type definition for the internal voltage handler structure
 *
 * @attention This structure should not be used outside of this module
 *
 * @param max22530 Handler structure of the external ADC
 * @param data The raw data read from the channels of the ADC
 */
typedef struct {
    Max22530Handler max22530;

    raw_volt_t data[INTERNAL_VOLTAGE_CHANNEL_COUNT];
} _InternalVoltageHandler;

#ifdef CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

/**
 * @brief Initialization function of the internal voltage module
 *
 * @param send A pointer to the function callback used to send data
 * @param send A pointer to the function callback used to send and receive data
 *
 * @return InternalVoltageReturnCode
 *     - INTERNAL_VOLTAGE_NULL_POINTER if any of the given parameters are NULL
 *     - INTERNAL_VOLTAGE_OK otherwise
 */
InternalVoltageReturnCode internal_voltage_init(
    spi_send_callback_t send,
    spi_send_receive_callback_t send_receive
);

/**
 * @brief Read all channels of the external ADC
 *
 * @return InternalVoltageReturnCode
 *     - INTERNAL_VOLTAGE_OK
 */
InternalVoltageReturnCode internal_voltage_read_all(void);

/**
 * @brief Get the raw voltage of the TS
 *
 * @return raw_volt_t The raw voltage value
 */
raw_volt_t internal_voltage_get_ts(void);

/**
 * @brief Get the raw voltage of the battery pack
 *
 * @return raw_volt_t The raw voltage value
 */
raw_volt_t internal_voltage_get_batt(void);

#else  // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

#define internal_voltage_init(send, send_receive) (INTERNAL_VOLTAGE_OK)
#define internal_voltage_read_all() (INTERNAL_VOLTAGE_OK)
#define internal_voltage_get_ts() (0U)
#define internal_voltage_get_batt() (0U)

#endif // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

#endif  // INTERNAL_VOLTAGE_H
