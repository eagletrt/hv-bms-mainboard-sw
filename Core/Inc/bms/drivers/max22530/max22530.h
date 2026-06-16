/*!
 * \file max22530.h
 * \date 2024-07-01
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * 
 * \brief MAX22530AWE+ external ADC driver
 */

#ifndef MAX22530_H
#define MAX22530_H

#include <stdbool.h>
#include <stdint.h>

#include "mainboard-def.h"

/*! \brief Offset from the base address of the non-filtered ADC values of the filtered values */
#define MAX22530_FILTERED_OFFSET (0x04U)

/*! \brief Identifier value for the MAX22530 */
#define MAX22530_ID (0x00U)

/*! \brief Reference voltage of the ADC */
#define MAX22530_VREF (1.8F)

/*! \brief ADC resolution in bits */
#define MAX22530_RESOLUTION (12U)

/*!
 * \brief Convert the 12 bit ADC value to a voltage in V
 *
 * \param val The value to convert
 *
 * \returns volt_t The converted value in V
 */
#define MAX22530_RAW_VALUE_TO_VOLT(value) MAINBOARD_ADC_RAW_VALUE_TO_VOLT(value, MAX22530_VREF, MAX22530_RESOLUTION)

/*! \brief Total number of bytes of a read/write command */
#define MAX22530_COMMAND_BYTE_SIZE (3U)
#define MAX22530_COMMAND_CRC_BYTE_SIZE (4U)

/*! \brief Total number of bytes of a burst read command */
#define MAX22530_BURST_BYTE_SIZE (11U)
#define MAX22530_BURST_CRC_BYTE_SIZE (12U)

/*! \brief Value treated as unused and that is ignored if received by the chip */
#define MAX22530_BYTE_UNUSED (0xFF)

/*! \brief Type definition for an ADC register address */
typedef uint8_t max22530_address;

/*! \brief Type definition for the ADC id and revision code */
typedef int8_t max22530_id;
typedef int8_t max22530_revision;

/*!
 * \brief Return code for the MAX22530 module functions
 */
enum Max22530ReturnCode {
    MAX22530_RC_OK,          /*!< Function executed successfully */
    MAX22530_RC_NULL_POINTER /*!< NULL pointer was given to a function */
};

/*!
 * \brief Memory section adresses of the chip
 */
enum Max22530Section {
    MAX22530_SECTION_ID = 0x00U,             /*!< ID section */
    MAX22530_SECTION_ADC_STATUS = 0x01U,     /*!< ADC status section */
    MAX22530_SECTION_COMPARATOR_OUT = 0x09U, /*!< Comparator output section */
    MAX22530_SECTION_CONTROL_STATUS = 0x11U, /*!< Control and status section */
    MAX22530_SECTION_COUNT = 4U              /*!< Total number of sections */
};

/*!
 * \brief Subsection of the ADC sections
 */
enum Max22530Register {
    MAX22530_REGISTER_ID = MAX22530_SECTION_ID,                                                     /*!< ID register */
    MAX22530_REGISTER_ADC = MAX22530_SECTION_ADC_STATUS,                                            /*!< ADC register */
    MAX22530_REGISTER_FILTERED_ADC = MAX22530_REGISTER_ADC + MAX22530_FILTERED_OFFSET,              /*!< Filtered ADC register */
    MAX22530_REGISTER_COMPARATOR_OUTPUT_HIGHER = MAX22530_SECTION_COMPARATOR_OUT,                   /*!< Comparator output higher register */
    MAX22530_REGISTER_COMPARATOR_OUTPUT_LOWER = MAX22530_REGISTER_COMPARATOR_OUTPUT_HIGHER + 0x04U, /*!< Comparator output lower register */
    MAX22530_REGISTER_COMPARATOR_OUTPUT_STATUS = MAX22530_SECTION_CONTROL_STATUS,                   /*!< Comparator output status register */
    MAX22530_REGISTER_INTERRUPT_STATUS = MAX22530_REGISTER_COMPARATOR_OUTPUT_STATUS + 0x01U,        /*!< Interrupt status register */
    MAX22530_REGISTER_INTERRUPT_ENABLE = MAX22530_REGISTER_INTERRUPT_STATUS + 0x01U,                /*!< Interrupt enable register */
    MAX22530_REGISTER_CONTROL = MAX22530_REGISTER_INTERRUPT_ENABLE + 0x01U,                         /*!< Control register */
    MAX22530_REGISTER_COUNT = 9U
};

/*!
 * \brief Control fields for the MAX22530 module
 */
enum Max22530ControlField {
    MAX22530_CONTROL_FIELD_HW_RESET = 0U,                 /*!< Hardware reset field */
    MAX22530_CONTROL_FIELD_SW_RESET = 1U,                 /*!< Software reset field */
    MAX22530_CONTROL_FIELD_CLEAR_POR = 2U,                /*!< Clear power-on reset field */
    MAX22530_CONTROL_FIELD_DISABLE_FIELD_POWER_VDDF = 3U, /*!< Disable field power VDDF field */
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_1 = 4U,           /*!< Clear filter 1 field */
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_2 = 5U,           /*!< Clear filter 2 field */
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_3 = 6U,           /*!< Clear filter 3 field */
    MAX22530_CONTROL_FIELD_CLEAR_FILTER_4 = 7U,           /*!< Clear filter 4 field */
    MAX22530_CONTROL_FIELD_ENABLE_COMMON_THRESHOLD = 14U, /*!< Enable common threshold field */
    MAX22530_CONTROL_FIELD_ENABLE_CRC = 15U               /*!< Enable CRC field */
};

/*!
 * \brief Type definition for the command type
 */
enum Max22530Command {
    MAX22530_COMMAND_READ = 0U, /*!< READ command */
    MAX22530_COMMAND_WRITE      /*!< WRITE command */
};

/*!
 * \brief Type definition for the burst option
 */
enum Max22530Burst {
    MAX22530_BURST_OFF = 0U, /*!< Burst disabled */
    MAX22530_BURST_ON        /*!< Burst enabled */
};

/*!
 * \brief Type definition for the available ADC channels
 */
enum Max22530Channel {
    MAX22530_CHANNEL_1 = 0U, /*!< First channel */
    MAX22530_CHANNEL_2,      /*!< Second channel */
    MAX22530_CHANNEL_3,      /*!< Third channel */
    MAX22530_CHANNEL_4,      /*!< Fourth channel */
    MAX22530_CHANNEL_COUNT   /*!< Total number of channels */
};

/*!
 * \brief Power-on reset flag
 *
 * \details This flag is set to one only after a power-on or an hardware reset
 */
enum Max22530PowerOnReset : int8_t {
    MAX22530_POWER_ON_RESET_OFF = 0, /*!< Power-on reset is off */
    MAX22530_POWER_ON_RESET_ON       /*!< Power-on reset is on */
};

/*!
 * \brief Handler structure for the MAX22530 external ADC
 */
struct Max22530Handler {
    spi_send_callback_t send;                 /*!< Callback for Spi send */
    spi_send_receive_callback_t send_receive; /*!< Callback for Spi send and receive */
};

#endif // MAX22530_H
