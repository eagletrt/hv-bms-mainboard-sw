/**
 * @file can-comm.h
 * @date 2024-06-21
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Functions to handle CAN communication with other devices
 */

#ifndef CAN_COMM_H
#define CAN_COMM_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

#include "primary_network.h"
#include "bms_network.h"

#include "ring-buffer.h"

/** @brief Maximum number of bytes of the payload in a CAN message */
#define CAN_COMM_MAX_PAYLOAD_BYTE_SIZE (8U)

/** @brief Mask used to check the bits of a CAN identifier */
#define CAN_COMM_ID_MASK (0x7FFU)
#define CAN_COMM_EXT_ID_MASK (0x1FFFFFFFU)

/** @brief Maximum number of CAN messages that can be saved inside the transmission and reception buffers */
#define CAN_COMM_MESSAGE_COUNT (bms_MESSAGE_COUNT + primary_MESSAGE_COUNT)
#define CAN_COMM_TX_BUFFER_BYTE_SIZE (CAN_COMM_MESSAGE_COUNT)
#define CAN_COMM_RX_BUFFER_BYTE_SIZE (CAN_COMM_MESSAGE_COUNT)

/** @brief Mask for the bits that defines if the CAN module is enabled or not */
#define CAN_COMM_ENABLED_ALL_MASK \
    ( \
        (1U << CAN_COMM_RX_ENABLE_BIT) | \
        (1U << CAN_COMM_TX_ENABLE_BIT) \
    )

/**
 * @brief Enable a single bit of the internal flag
 *
 * @param FLAG The internal flag
 * @param BIT The bit of the flag to set
 */
#define CAN_COMM_ENABLE(FLAG, BIT) ((FLAG) = MAINBOARD_BIT_SET(FLAG, BIT))
/**
 * @brief Disable a single bit of the internal flag
 *
 * @param FLAG The internal flag
 * @param BIT The bit of the flag to reset
 */
#define CAN_COMM_DISABLE(FLAG, BIT) ((FLAG) = MAINBOARD_BIT_RESET(FLAG, BIT))
/**
 * @brief Toggle a single bit of the internal flag
 *
 * @param FLAG The internal flag
 * @param BIT The bit of the flag to flip
 */
#define CAN_COMM_TOGGLE(FLAG, BIT) ((FLAG) = MAINBOARD_BIT_TOGGLE(FLAG, BIT))
/**
 * @brief Check if a specific bit of the internal flag is set
 *
 * @param FLAG The internal flag
 * @param BIT The bit of the flag to check
 *
 * @return bool True if the bit is set, false otherwise
 */
#define CAN_COMM_IS_ENABLED(FLAG, BIT) MAINBOARD_BIT_GET(FLAG, BIT)

/**
 * @brief Enable all the bits of the internal flag
 *
 * @param FLAG The internal flag
 */
#define CAN_COMM_ENABLE_ALL(FLAG) ((FLAG) |= CAN_COMM_ENABLED_ALL_MASK)
/**
 * @brief Disable all the bits of the internal flag
 *
 * @param FLAG The internal flag
 */
#define CAN_COMM_DISABLE_ALL(FLAG) ((FLAG) &= ~CAN_COMM_ENABLED_ALL_MASK)
/**
 * @brief Toggle all the bits of the internal flag
 *
 * @param FLAG The internal flag
 */
#define CAN_COMM_TOGGLE_ALL(FLAG) ((FLAG) ^= CAN_COMM_ENABLED_ALL_MASK)
/**
 * @brief Check if all the bits of the internal flag are set
 *
 * @param FLAG The internal flag
 *
 * @return bool True if all the bits are set, false otherwise
 */
#define CAN_COMM_IS_ENABLED_ALL(FLAG) (((FLAG) & CAN_COMM_ENABLED_ALL_MASK) == CAN_COMM_ENABLED_ALL_MASK)

/**
 * @brief Return code for the CAN communication module functions
 *
 * @details
 *     - CAN_COMM_OK the function executed succesfully
 *     - CAN_COMM_NULL_POINTER a NULL pointer was given to a function
 *     - CAN_COMM_DISABLED the CAN manager is not running
 *     - CAN_COMM_OVERRUN the transmit buffer is full
 *     - CAN_COMM_INVALID_NETWORK the given network does not correspond to any valid canlib network
 *     - CAN_COMM_INVALID_INDEX the given index does not correspond to any CAN message
 *     - CAN_COMM_INVALID_PAYLOAD_SIZE the payload size exceed the maximum possible length
 *     - CAN_COMM_INVALID_FRAME_TYPE the frame type does not correspond to any existing CAN frame type
 *     - CAN_COMM_CONVERSION_ERROR the message could not be converted correctly
 *     - CAN_COMM_TRANSMISSION_ERROR there was an error during the transmission of the message
 */
typedef enum {
    CAN_COMM_OK,
    CAN_COMM_NULL_POINTER,
    CAN_COMM_DISABLED,
    CAN_COMM_OVERRUN,
    CAN_COMM_INVALID_NETWORK,
    CAN_COMM_INVALID_INDEX,
    CAN_COMM_INVALID_PAYLOAD_SIZE,
    CAN_COMM_INVALID_FRAME_TYPE,
    CAN_COMM_CONVERSION_ERROR,
    CAN_COMM_TRANSMISSION_ERROR
} CanCommReturnCode;

/**
 * @brief Enable bit flag positions
 *
 * @details
 *     - CAN_COMM_RX_ENABLE_BIT reception enable bit
 *     - CAN_COMM_TX_ENABLE_BIT transmission enable bit
 */
typedef enum {
    CAN_COMM_RX_ENABLE_BIT = 0U,
    CAN_COMM_TX_ENABLE_BIT,
    CAN_COMM_ENABLE_BIT_COUNT
} CanCommEnableBit;

/**
 * @brief Union used to choose the CAN payload based on transmission or reception
 *
 * @details This is needed because during transmission the size of the payload is unknown
 * otherwise the received payload cannot exceed the maximum possible size
 *
 * @param tx A pointer to the CAN payload that needs to be transmitted
 * @param rx The received CAN payload
 */
typedef union {
    uint8_t tx[bms_MAX_STRUCT_SIZE_CONVERSION];
    uint8_t rx[bms_MAX_STRUCT_SIZE_RAW];
} CanPayload;

/**
 * @brief Structure definition for the content of a CAN bus message
 *
 * @param network The CAN network used to communicate
 * @param index Index mapped to the CAN identifier
 * @param frame_type The frame type
 * @param payload A pointer to the actual content of the message
 */
typedef struct {
    CanNetwork network;
    can_index_t index;
    CanFrameType frame_type;
    CanPayload payload;
} CanMessage;


/**
 * @brief Function used to send CAN message via a network
 *
 * @oaram network The CAN network used to transmit the message
 * @param id The CAN identifier 
 * @param frame_type The CAN frame type
 * @param data The actual payload of the message
 * @param size The size of the payload
 *
 * @return CanCommReturnCode The return code value
 */
typedef CanCommReturnCode (* can_comm_transmit_callback_t)(
    const CanNetwork network,
    const can_id_t id,
    const CanFrameType frame_type,
    const uint8_t * const data,
    const size_t size
);

/**
 * @brief CAN manager handler structure
 *
 * @warning This structure should never be used outside of this file
 *
 * @details The enabled bit flag 
 *
 * @param enabled Flag used to enable or disable the CAN communication
 * @param tx_busy Transmission messages flags to check if the message has not already been sent
 * @param rx_busy Reception messages flags to check if the message has not already been handled
 * @param tx_buf Transmission messages circular buffer
 * @param rx_buf Reception messages circular buffer
 * @param send A pointer to the callback used to send the data via CAN
 * @param rx_device The reception canlib message handler
 * @param rx_raw The reception raw data of the message
 * @param rx_conv The reception converted data of the message
 */
typedef struct {
    bit_flag8_t enabled;
    bool tx_busy[CAN_NETWORK_COUNT][CAN_COMM_MESSAGE_COUNT];
    bool rx_busy[CAN_NETWORK_COUNT][CAN_COMM_MESSAGE_COUNT];
    RingBuffer(CanMessage, CAN_COMM_TX_BUFFER_BYTE_SIZE) tx_buf;
    RingBuffer(CanMessage, CAN_COMM_RX_BUFFER_BYTE_SIZE) rx_buf;

    can_comm_transmit_callback_t send;

    // Canlib devices
    device_t rx_device;
    uint8_t rx_raw[bms_MAX_STRUCT_SIZE_RAW];
    uint8_t rx_conv[bms_MAX_STRUCT_SIZE_CONVERSION];
} _CanCommHandler;

/**
 * @brief Handle the received CAN payload data
 *
 * @details The payload parameter should be converted to the correct structure pointer
 *
 * @param payload A pointer to the converted canlib structure data
 */
typedef void (* can_comm_canlib_payload_handle_callback_t)(void * const payload);

/** @brief Type definitions for the canlib device functions */
typedef int (* id_from_index_t)(int);
typedef int (* serialize_from_id_t)(void *, uint16_t, uint8_t *);
typedef void (* deserialize_from_id_t)(device_t *, uint16_t, uint8_t *);

#ifdef CONF_CAN_COMM_MODULE_ENABLE

/**
 * @brief Initialize the CAN communication handler structure
 *
 * @param send The callback of a function that should send the data via a CAN network
 *
 * @return CanCommReturnCode
 *     - CAN_COMM_NULL_POINTER a NULL pointer was given as parameter
 *     - CAN_COMM_OK otherwise
 */
CanCommReturnCode can_comm_init(const can_comm_transmit_callback_t send);

/** @brief Enable the CAN manager */
void can_comm_enable_all(void);

/** @brief Disable the CAN manager */
void can_comm_disable_all(void);

/**
 * @brief Check if the CAN manager is enabled
 *
 * @return bool True if the manager is enabled, false otherwise
 */
bool can_comm_is_enabled_all(void);

/**
 * @brief Enable a single bit of the internal handler flag
 *
 * @param bit The bit to enable
 */
void can_comm_enable(const CanCommEnableBit bit);

/**
 * @brief Disable a single bit of the internal handler flag
 *
 * @param bit The bit to disable
 */
void can_comm_disable(const CanCommEnableBit bit);

/**
 * @brief Check if a single bit of the internal handler flag is enabled
 *
 * @param bit The bit to check
 *
 * @return bool True if the manager is enabled, false otherwise
 */
bool can_comm_is_enabled(const CanCommEnableBit bit);

/**
 * @brief Immediately send the message via the CAN bus
 *
 * @attention This function should be used carefully because it can run the
 * routine multiple times internally
 *
 * @param network The canlib network to select
 * @param index The CAN index mapped to its identifier
 * @param frame_type The frame type
 * @param data The payload of the message
 * @param size The payload size in bytes
 *
 * @return CanCommReturnCode
 *     - CAN_COMM_DISABLED the CAN manager is disabled
 *     - CAN_COMM_INVALID_NETWORK if the given network is not a valid canlib network
 *     - CAN_COMM_INVALID_INDEX if the given index does not match any valid CAN identifier
 *     - CAN_COMM_INVALID_PAYLOAD_SIZE the given payload size exceed the maximum possible length
 *     - CAN_COMM_INVALID_FRAME_TYPE the given frame type is not a valid CAN frame type
 *     - CAN_COMM_OVERRUN the transmission buffer is already full
 *     - CAN_COMM_CONVERSION_ERROR there was an error during the conversion of the message
 *     - CAN_COMM_OK otherwise
 */
CanCommReturnCode can_comm_send_immediate(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t * const data,
    const size_t size
);

/**
 * @brief Add a message to the transmission buffer
 *
 * @details The message will be sent afterwards inside the routine
 *
 * @param network The canlib network to select
 * @param index The CAN index mapped to its identifier
 * @param frame_type The frame type
 * @param data The payload of the message
 * @param size The payload size in bytes
 *
 * @return CanCommReturnCode
 *     - CAN_COMM_DISABLED the CAN manager is disabled
 *     - CAN_COMM_INVALID_NETWORK if the given network is not a valid canlib network
 *     - CAN_COMM_INVALID_INDEX if the given index does not match any valid CAN identifier
 *     - CAN_COMM_INVALID_PAYLOAD_SIZE the given payload size exceed the maximum possible length
 *     - CAN_COMM_INVALID_FRAME_TYPE the given frame type is not a valid CAN frame type
 *     - CAN_COMM_OVERRUN the transmission buffer is already full
 *     - CAN_COMM_OK otherwise
 */
CanCommReturnCode can_comm_tx_add(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t * const data,
    const size_t size
);

/**
 * @brief Add a message to the reception buffer
 *
 * @details The message will be handled afterwards inside the routine
 *
 * @param network The canlib network to select
 * @param index The CAN index mapped to its identifier
 * @param frame_type The frame type
 * @param data The payload of the message (can be NULL for REMOTE frames)
 * @param size The paylaod size in bytes
 *
 * @return CanCommReturnCode
 *     - CAN_COMM_DISABLED the CAN manager is disabled
 *     - CAN_COMM_INVALID_NETWORK if the given network is not a valid canlib network
 *     - CAN_COMM_INVALID_PAYLOAD_SIZE the given payload size exceed the maximum possible length
 *     - CAN_COMM_INVALID_FRAME_TYPE the given frame type is not a valid CAN frame type
 *     - CAN_COMM_OVERRUN the transmission buffer is already full
 *     - CAN_COMM_OK otherwise
 */
CanCommReturnCode can_comm_rx_add(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t * const data,
    const size_t size
);

/**
 * @brief Routine used to manage the sent or received can data
 *
 * @return CanCommReturnCode
 *     - CAN_COMM_DISABLED the CAN manager is not running
 *     - CAN_COMM_CONVERSION_ERROR there was an error during the conversion of the message
 *     - CAN_COMM_OK otherwise
 */
CanCommReturnCode can_comm_routine(void);

#else  // CONF_CAN_COMM_MODULE_ENABLE

#define can_comm_init(send) (CAN_COMM_OK)
#define can_comm_enable_all() CELLBOARD_NOPE()
#define can_comm_disable_all() CELLBOARD_NOPE()
#define can_comm_is_enabled_all() (false)
#define can_comm_enable(bit) CELLBOARD_NOPE()
#define can_comm_disable(bit) CELLBOARD_NOPE()
#define can_comm_is_enabled(bit) (false)
#define can_comm_send_immidiate(index, frame_type, data, size) (CAN_COMM_OK)
#define can_comm_tx_add(index, frame_type, data, size) (CAN_COMM_OK)
#define can_comm_rx_add(index, frame_type, data, size) (CAN_COMM_OK)
#define can_comm_routine() (CAN_COMM_OK)

#endif // CONF_CAN_COMM_MODULE_ENABLE

#endif  // CAN_COMM_H
