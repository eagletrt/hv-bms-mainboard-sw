/**
 * @file mainboard-def.h
 * @date 2024-06-19
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Custom definitions used inside the project
 */

#ifndef MAINBOARD_DEF_H
#define MAINBOARD_DEF_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief No operation
 *
 * @details Does nothing
 */
#define MAINBOARD_NOPE() ((void)(0U))

/**
 * @brief Ignore the return value
 *
 * @details Used to avoid compiler warnings
 *
 * @param _ Anything that can return a value (even variables)
 */
#define MAINBOARD_UNUSED(_) ((void)(_))

/**
 * @brief Type definition for the static attribute 
 *
 * @details Can be used to test if pisseria is doing fetenderi
 * @details Can be used to disable static functions for unit testing
 */
#ifndef _STATIC
#define _STATIC static
#endif  // _STATIC

/**
 * @brief Type definition for the inline attribute 
 *
 * @details Can be used to disable inlining functions for unit testing
 */
#ifndef _STATIC_INLINE
#define _STATIC_INLINE static inline
#endif  // _STATIC_INLINE

/**
 * @brief Type definition for the volatile attribute 
 *
 * @details Can be used to disable volatile variables for unit testing
 */
#ifndef _VOLATILE
#define _VOLATILE volatile
#endif  // _VOLATILE


/*** ######################### CONSTANTS ################################# ***/

/**
 * @defgroups constants
 * @brief Constant used throughout the project
 * {@
 */

/** @brief Total number of cellboards */
#define CELLBOARD_COUNT (CELLBOARD_ID_COUNT)

/** @} */

/*** ######################### MACROS #################################### ***/

/**
 * @defgroups macros
 * @brief Macros used throughout the project
 * {@
 */

/**
 * @brief Get the minimum value between two numbers
 *
 * @param A The first number
 * @param B the second number
 *
 * @return A if it's less or equal than B, B otherwise
 */
#define MAINBOARD_MIN(A, B) (((A) <= (B)) ? (A) : (B))

/**
 * @brief Get the maximum value between two numbers
 *
 * @param A The first number
 * @param B The second number
 *
 * @return A if it's greater or equal than B, B otherwise
 */
#define MAINBOARD_MAX(A, B) (((A) >= (B)) ? (A) : (B))

/**
 * @brief Clamp a value between a certain range
 *
 * @param VAL The value to clamp
 * @param LOW The lower value of the range
 * @param UP The upper value of the range
 *
 * @return LOW if the value is lower or equal to the smallest range value, 
 *     UP if the value is greater or equal to the largest range value
 *     VAL otherwise
 */
#define MAINBOARD_CLAMP(VAL, LOW, UP) (((VAL) <= (LOW)) ? (LOW) : (((VAL) >= (UP)) ? (UP) : (VAL)))

/**
 * @brief Get the value of a specific bit of a variable
 *
 * @param VAR The variable that should be changed
 * @param BIT The position of the bit to get
 *
 * @return The value of the chosen bit
 */
#define MAINBOARD_BIT_GET(VAR, BIT) (((VAR) & (1U << (BIT))) != 0U)

/**
 * @brief Set a specific bit of a variable to 1
 *
 * @attention This macro do not modifiy the variable but returns a new value
 *
 * @param VAR The variable that should be changed
 * @param BIT The position of the bit to set
 *
 * @return The value of the modified variable
 */
#define MAINBOARD_BIT_SET(VAR, BIT) ((VAR) | (1U << (BIT)))

/**
 * @brief Set a specific bit of a variable to 0
 *
 * @attention This macro do not modifiy the variable but returns a new value
 *
 * @param VAR The variable that should be changed
 * @param BIT The position of the bit to reset
 *
 * @return The value of the modified variable
 */
#define MAINBOARD_BIT_RESET(VAR, BIT) (~((~(VAR)) | (1U << (BIT))))

/**
 * @brief Negate a specific bit of a variable
 *
 * @attention This macro do not modifiy the variable but returns a new value
 *
 * @param VAR The variable that should be changed
 * @param BIT The position of the bit to set
 *
 * @return The value of the modified variable
 */
#define MAINBOARD_BIT_TOGGLE(VAR, BIT) ((VAR) ^ (1U << (BIT)))

/**
 * @brief Set or reset a specific bit of a variable based on a boolean condition
 *
 * @attention This macro do not modify the variable but returns a new value
 *
 * @param VAR The variable that should be changed
 * @param CONDITION The boolean condition to check
 * @param BIT The position of the bit to set
 *
 * @return The value of the modified variable
 */
#define MAINBOARD_BIT_TOGGLE_IF(VAR, CONDITION, BIT) ( \
        (CONDITION) ? \
        MAINBOARD_BIT_SET(VAR, BIT) : \
        MAINBOARD_BIT_RESET(VAR, BIT) \
    )

/** @} */

/*** ######################### ENUMS DEFINITIONS ######################### ***/

/**
 * @defgroups enums
 * @brief Definition of different enumerations used to give meaning to integer values
 * {@
 */

/**
 * @brief Definition of the cellboard indices
 * 
 * @details Each cellboard is numbered from 0 to n where n is the last cellboard
 * The real cellboard order is not guaranteed to match this order
 *
 * @details A mainboard identifier is added for utility purposes but it is not included
 * in the cellboard id count
 */
typedef enum {
    CELLBOARD_ID_0 = 0U,
    CELLBOARD_ID_1,
    CELLBOARD_ID_2,
    CELLBOARD_ID_3,
    CELLBOARD_ID_4,
    CELLBOARD_ID_5,
    CELLBOARD_ID_COUNT,
    MAINBOARD_ID
} CellboardId;

/**
 * @brief Type definition for a SPI network
 *
 * @details
 *     - SPI_NETWORK_EEPROM the network connected to an EEPROM
 *     - SPI_NETWORK_ADC the network connected to an external ADC
 */
typedef enum {
    SPI_NETWORK_EEPROM = 0U,
    SPI_NETWORK_ADC,
    SPI_NETWORK_COUNT
} SpiNetwork;

/**
 * @brief Definition of different CAN networks
 *
 * @details
 *     - CAN_NETWORK_BMS the internal network between mainboard and cellboards
 *     - CAN_NETWORK_PRIMARY the main network where all the important message are sent
 *     - CAN_NETWORK_SECONDARY network dedicated to sensor and other measuring devices
 */
typedef enum {
    CAN_NETWORK_BMS,
    CAN_NETWORK_PRIMARY,
    CAN_NETWORK_SECONDARY,
    CAN_NETWORK_COUNT
} CanNetwork;

/**
 * @brief Definition of possible CAN frame types
 *
 * @details
 *     - CAN_FRAME_TYPE_DATA the CAN frame that contains data
 *     - CAN_FRAME_TYPE_REMOTE the CAN frame used to request a data transmission from another node in the network
 */
typedef enum {
    CAN_FRAME_TYPE_DATA,
    CAN_FRAME_TYPE_REMOTE,
    CAN_FRAME_TYPE_COUNT
} CanFrameType;

/** @} */

/*** ######################### TYPE DEFINITIONS ########################## ***/

/**
 * @defgroups types
 * @brief Type definition for various unit of measurements
 * {@
 */

/** @brief Type definition for a percentage (from 0 to 100) */
typedef int8_t percentage_t;
typedef float precise_percentage_t;

/** @brief Type definition for bit flags */
typedef uint8_t bit_flag8_t;
typedef uint16_t bit_flag16_t;
typedef uint32_t bit_flag32_t;

/** @brief Type definition for the standard CAN 2.0a and CAN 2.0b (extended) identifiers */
typedef uint16_t can_id_t;
typedef uint32_t can_ext_id_t;

/**
 * @brief Type definition for a CAN index
 * 
 * @details Used to map the can identifiers
 */
typedef int32_t can_index_t;

/** @brief Type definition for a custom amount of elapsed time */
typedef uint32_t ticks_t;

/** @brief Type definition for the time */
typedef uint32_t seconds_t;
typedef uint32_t milliseconds_t;
typedef uint32_t microseconds_t;

/** @brief Type definition for the frequency */
typedef float hertz_t;
typedef float kilohertz_t;
typedef float megahertz_t;

/**
 * @brief Raw temperature value
 * @details This type depends on the mechanism of acquisition of the temperatures
 * @details If and ADC is used the number of bits depends on its resolution
 */
typedef uint16_t raw_temp_t;

/** @brief Temperature value in °C */
typedef float celsius_t;

/**
 * @brief Raw voltage value
 * @details This type depends on the mechanism of acquisition of the voltages
 * @details If and ADC is used the number of bits depends on its resolution
 */
typedef uint16_t raw_volt_t;

/** @brief Actual voltages */
typedef float volt_t;
typedef float millivolt_t;

/** @brief Function callback that resets the microcontroller */
typedef void (* system_reset_callback_t)(void);

/** @brief Function callback used to enter a critical section */
typedef void (* interrupt_critical_section_enter_t)(void);

/** @brief Function callback used to exit a critical section */
typedef void (* interrupt_critical_section_exit_t)(void);

/**
 * @brief Type definition for the callback used to send data via SPI
 *
 * @param network The SPI network to select
 * @param data A pointer to the data to send
 * @param size The length of data in bytes
 */
typedef void (* spi_send_callback_t)(SpiNetwork network, uint8_t * data, size_t size);

/**
 * @brief Type definition for the callback used to send and receive data via SPI
 *
 * @param network The SPI network to select
 * @param data A pointer to the data to send
 * @param out[out] A pointer to the array where the received data is stored
 * @param size The length of data in bytes
 * @param out_size The number of bytes that should be received
 */
typedef void (* spi_send_receive_callback_t)(
    SpiNetwork network,
    uint8_t * data,
    uint8_t * out,
    size_t size,
    size_t out_size
);

/** @} */

#endif  // MAINBOARD_DEF_H
