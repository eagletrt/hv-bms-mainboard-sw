/**
 * @file max22530.h
 * @date 2024-07-01
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief MAX22530AWE+ external ADC driver
 */

#ifndef MAX22530_H
#define MAX22530_H

#include <stdbool.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

/** @brief Offset from the base address of the non-filtered ADC values of the filtered values */
#define MAX22530_FILTERED_OFFSET (0x04U)

/** @brief Identifier value for the MAX22530 */
#define MAX22530_ID (0x00U)

/** @brief Reference voltage of the chip */
#define MAX22530_VREF (1.8f)

/**
 * @brief Convert the 12 bit ADC value to a voltage in millivolt
 *
 * @param val The value to convert
 *
 * @return millivolt_t The converted value in millivolt
 */
#define MAX22530_VALUE_TO_MILLIVOLT(val) ((millivolt_t)((val) * (MAX22530_VREF / 4095.f)))


/** @brief Total number of bytes of a read/write command */
#define MAX22530_COMMAND_BYTE_SIZE (3U)
#define MAX22530_COMMAND_CRC_BYTE_SIZE (4U)

/** @brief Total number of bytes of a burst read command */
#define MAX22530_BURST_BYTE_SIZE (11U)
#define MAX22530_BURST_CRC_BYTE_SIZE (12U)

/** @brief Type definition for an ADC register address */
typedef uint8_t max22530_address_t;

/** @brief Type definition for the ADC id and revision code */
typedef int8_t max22530_id_t;
typedef int8_t max22530_revision_t;

/**
 * @brief Return code for the MAX22530 module functions
 *
 * @details
 *     - MAX22530_OK the function executed succesfully
 *     - MAX22530_NULL_POINTER a NULL pointer was given to a function
 */
typedef enum {
    MAX22530_OK,
    MAX22530_NULL_POINTER
} Max22530ReturnCode;

/**
 * @brief Memory section adresses of the chip
 *
 * @details
 *     - MAX22530_SECTION_ID info about the chip
 *     - MAX22530_SECTION_ADC_STATUS read ADC values
 *     - MAX22530_SECTION_COMPARATOR_OUT comparator output values
 *     - MAX22530_SECTION_CONTROL_STATUS info about the status of the chip
 */
typedef enum {
    MAX22530_SECTION_ID = 0x00U,
    MAX22530_SECTION_ADC_STATUS = 0x01U,
    MAX22530_SECTION_COMPARATOR_OUT = 0x09U,
    MAX22530_SECTION_CONTROL_STATUS = 0x11U,

    MAX22530_SECTION_COUNT = 4U
} Max22530Section;

/**
 * @brief Subsection of the ADC sections
 *
 * @details
 *     - MAX22530_REGISTER_ID info about the chip (match the entire section)
 *     - MAX22530_REGISTER_ADC read non-filtered ADC values
 *     - MAX22530_REGISTER_FILTERED_ADC read filtered ADC values
 *     - MAX22530_REGISTER_COMPARATOR_OUTPUT_HIGHER comparator high values
 *     - MAX22530_REGISTER_COMPARATOR_OUTPUT_LOWER comparator low values
 *     - MAX22530_REGISTER_COMPARATOR_OUTPUT_STATUS comparator status
 *     - MAX22530_REGISTER_INTERRUPT_STATUS interrupt status
 *     - MAX22530_REGISTER_INTERRUPT_ENABLE interrupt enable
 *     - MAX22530_REGISTER_CONTRL chip control register
 */
typedef enum {
    MAX22530_REGISTER_ID = MAX22530_SECTION_ID,
    MAX22530_REGISTER_ADC = MAX22530_SECTION_ADC_STATUS,
    MAX22530_REGISTER_FILTERED_ADC = MAX22530_REGISTER_ADC + MAX22530_FILTERED_OFFSET,
    MAX22530_REGISTER_COMPARATOR_OUTPUT_HIGHER = MAX22530_SECTION_COMPARATOR_OUT,
    MAX22530_REGISTER_COMPARATOR_OUTPUT_LOWER = MAX22530_REGISTER_COMPARATOR_OUTPUT_HIGHER + 0x04U,
    MAX22530_REGISTER_COMPARATOR_OUTPUT_STATUS = MAX22530_SECTION_CONTROL_STATUS,
    MAX22530_REGISTER_INTERRUPT_STATUS = MAX22530_REGISTER_COMPARATOR_OUTPUT_STATUS + 0x01U,
    MAX22530_REGISTER_INTERRUPT_ENABLE = MAX22530_REGISTER_INTERRUPT_STATUS + 0x01U,
    MAX22530_REGISTER_CONTROL = MAX22530_REGISTER_INTERRUPT_ENABLE + 0x01U,

    MAX22530_REGISTER_COUNT = 9U
} Max22530Register;

typedef enum {
    MAX22530_CONTROL_FIELD_HW_RESET = 0U,
    MAX22530_CONTROL_FIELD_SW_RESET = 1U,
    MAX22530_CONTROL_FIELD_CLEAR_POR = 2U,
    MAX22530_CONTROL_FIELD_DISABLE_FIELD_POWER_VDDF = 3U,
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_1 = 4U,
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_2 = 5U,
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_3 = 6U,
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_4 = 7U,
    MAX22530_CONTROL_FIELD_ENABLE_COMMON_THRESHOLD = 14U,
    MAX22530_CONTROL_FIELD_ENABLE_CRC = 15U
} Max22530ControlField;

/**
 * @brief Type definition for the command type
 *
 * @details
 *     - MAX22530_COMMAND_READ read operation
 *     - MAX22530_COMMAND_WRITE write operation
 */
typedef enum {
    MAX22530_COMMAND_READ = 0U,
    MAX22530_COMMAND_WRITE
} Max22530Command;

/**
 * @brief Type definition for the burst option
 *
 * @details
 *     - MAX22530_BURST_OFF burst disabled
 *     - MAX22530_BURST_ON burst enabled
 */
typedef enum {
    MAX22530_BURST_OFF = 0U,
    MAX22530_BURST_ON
} Max22530Burst;

/**
 * @brief Type definition for the available ADC channels
 *
 * @details
 *     - MAX22530_CHANNEL_1 the first channel
 *     - MAX22530_CHANNEL_2 the second channel
 *     - MAX22530_CHANNEL_3 the third channel
 *     - MAX22530_CHANNEL_4 the fourth channel
 */
typedef enum {
    MAX22530_CHANNEL_1 = 0U,
    MAX22530_CHANNEL_2,
    MAX22530_CHANNEL_3,
    MAX22530_CHANNEL_4,
    MAX22530_CHANNEL_COUNT
} Max22530Channel;

/**
 * @brief Power-on reset flag
 *
 * @details This flag is set to one only after a power-on or an hardware reset
 * @details
 *     - MAX22530_POWER_ON_RESET_OFF normal operation
 *     - MAX22530_POWER_ON_RESET_ON wake-up from power-on reset
 */
typedef enum {
    MAX22530_POWER_ON_RESET_OFF = 0,
    MAX22530_POWER_ON_RESET_ON
} Max22530PowerOnReset;

/**
 * @brief Handler structure for the MAX22530 external ADC
 *
 * @param send A pointer to the callback used to send data via SPI
 * @param send_receive A pointer to the callback used to send data via SPI
 */
typedef struct {
    spi_send_callback_t send;
    spi_send_receive_callback_t send_receive;
} Max22530Handler;

#define CONF_MAX22530_MODULE_ENABLE
#ifdef CONF_MAX22530_MODULE_ENABLE

/**
 * @brief Initialize the MAX22530 internal handler structure
 *
 * @attention By default CRC is disabled so every command is 24 bit long instead of 36 bit,
 * but it can be enabled through spi in the control register of the MAX22530
 *
 * @param handler The ADC handler structure
 * @param send A pointer to the callback used to send data via SPI
 * @param send_receive A pointer to the callback used to send and receive data via SPI
 *
 * @return Max22530ReturnCode
 *     - MAX22530_NULL_POINTER if any of the parameter is NULL
 *     - MAX22530_OK otherwise
 */
Max22530ReturnCode max22530_init(
    Max22530Handler * handler,
    spi_send_callback_t send,
    spi_send_receive_callback_t send_receive
);

/**
 * @brief Get the ADC identifier code
 *
 * @param handler A pointer to the ADC handler structure
 *
 * @return max22530_id_t The ADC identifier or -1 on error
 */
max22530_id_t max22530_get_id(Max22530Handler * handler);

/**
 * @brief Get the power-on reset code
 *
 * @param handler A pointer to the ADC handler structure
 *
 * @return Max22530PowerOnReset The power-on reset code or -1 on error
 */
Max22530PowerOnReset max22530_get_power_on_reset(Max22530Handler * handler);

/**
 * @brief Get the revision code
 *
 * @param handler A pointer to the ADC handler structure
 *
 * @return max22530_revision_t The revision code or -1 on error
 */
max22530_revision_t max22530_get_revision(Max22530Handler * handler);

/**
 * @brief Read a single channel of the ADC
 *
 * @param handler A pointer to the ADC handler structure
 * @param channel The channel to read from
 * @param filtered Set to true to select the filtered channel
 *
 * @return raw_volt_t The raw value of the channel
 */
raw_volt_t max22530_read_channel(Max22530Handler * handler, Max22530Channel channel, bool filtered);

/**
 * @brief Read all the channels of the ADC in a single command
 *
 * @attention The out array has to be big enough to contain all the ADC channels values
 * To declare the array use the MAX22530_CHANNEL_COUNT enum field
 *
 * @param handler A pointer to the ADC handler structure
 * @param filtered Set to true to select the filtered channel
 * @param out[out] A pointer to the array where the data is stored
 * @param interrupt_status[out] A pointer to the variable where the interrupt status is stored (can be NULL)
 *
 * @return Max22530ReturnCode
 *     - MAX22530_NULL_POINTER if any of the parameters are NULL
 *     - MAX22530_OK otherwise
 */
Max22530ReturnCode max22530_read_channels_all(
    Max22530Handler * handler,
    bool filtered,
    raw_volt_t * out,
    uint16_t * interrupt_status
);

#else  // CONF_MAX22530_MODULE_ENABLE

#define max22530_init(handler, send, send_receive) (MAX22530_OK)
#define max22530_get_id(handler) (MAX22530_ID)
#define max22530_get_power_on_reset(handler) (MAX22530_POWER_ON_RESET_OFF)
#define max22530_get_revision(handler) (0x00)
#define max22530_read_channel(handler, channel, filtered) (0U)
#define max22530_read_channels_all(handler, channel, filtered, interrupt_status) (0U)

#endif // CONF_MAX22530_MODULE_ENABLE

#endif // MAX22530_H
