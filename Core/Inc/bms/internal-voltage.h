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

#include "primary_network.h"

#include "max22530.h"

/** @brief The period with which internal voltages are updated in ms */
#define INTERNAL_VOLTAGE_CYCLE_TIME_MS (8U)

/** @brief Maximum allowed voltage difference between the pack voltage and the cells voltage sum in V */
#define INTERNAL_VOLTAGE_MAX_DELTA_V (25.f)

/** @brief Divider ratio of the internal voltages */
#define INTERNAL_VOLTAGE_DIVIDER_RATIO (0.002914f)

/**
 * @brief Conversion from the voltage read from the ADC to the real voltage in V
 *
 * @param value The voltage read from the ADC
 *
 * @return volt_t The converted value in V
 */
#define INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(value) (value / INTERNAL_VOLTAGE_DIVIDER_RATIO)

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
 *     - INTERNAL_VOLTAGE_CHANNEL_PACK_VOLTAGE The voltage of the battery pack
 *     - INTERNAL_VOLTAGE_CHANNEL_IMD_TS_CONNECTED Feedback on the TS connection to the IMD
 *     - INTERNAL_VOLTAGE_CHANNEL_PRECHARGE_TEMPERATURE Temperature of the precharge resistors heatsink
 */
typedef enum {
    INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE = MAX22530_CHANNEL_1,
    INTERNAL_VOLTAGE_CHANNEL_PACK_VOLTAGE = MAX22530_CHANNEL_2,
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
 * @param ts The voltage of the Tractive System
 * @param pack The voltage of the battery pack
 * @param ts_voltage_can_payload The TS voltage info canlib payload
 */
typedef struct {
    Max22530Handler max22530;

    volt_t ts;
    volt_t pack;

    primary_hv_ts_voltage_converted_t ts_voltage_can_payload;
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
InternalVoltageReturnCode internal_voltage_init(const spi_send_callback_t send, const spi_send_receive_callback_t send_receive);

/**
 * @brief Read all channels of the external ADC
 *
 * @return InternalVoltageReturnCode
 *     - INTERNAL_VOLTAGE_OK
 */
InternalVoltageReturnCode internal_voltage_read_all(void);

/**
 * @brief Get the voltage of the Tractive System in V
 *
 * @return volt_t The TS voltage in V
 */
volt_t internal_voltage_get_ts(void);

/**
 * @brief Get the voltage of the battery pack in V
 *
 * @return volt_t The battery pack voltage in V
 */
volt_t internal_voltage_get_pack(void);

/**
 * @brief Get a pointer to the CAN payload of the TS voltage info
 * 
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_ts_voltage_converted_t* A pointer to the payload
 */
primary_hv_ts_voltage_converted_t * internal_voltage_get_ts_voltage_canlib_payload(size_t * const byte_size);

#else  // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

#define internal_voltage_init(send, send_receive) (INTERNAL_VOLTAGE_OK)
#define internal_voltage_read_all() (INTERNAL_VOLTAGE_OK)
#define internal_voltage_get_ts() (0U)
#define internal_voltage_get_pack() (0U)
#define internal_voltage_get_ts_voltage_canlib_payload(byte_size) (NULL)

#endif // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

#endif  // INTERNAL_VOLTAGE_H
