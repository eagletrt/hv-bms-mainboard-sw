/*!
 * \file can-comm.c
 * \date 2024-06-21
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions to handle CAN communication with other devices
 */

#include "can-comm-api.h"

#include <string.h>

#include "can-comm.h"
#include "fsm.h"
#include "mainboard-def.h"
#include "programmer-api.h"
#include "watchdog.h"
#include "timebase.h"
#include "current-api.h"
#include "pcu-api.h"
#include "volt-api.h"
#include "identity-api.h"
#include "temp-api.h"
#include "bal-api.h"
#include "error-api.h"

#include "canlib_device.h"

#ifdef CONF_CAN_COMM_MODULE_ENABLE

EAGLETRT_STATIC struct CanCommHandler can_comm_handler;

/*!
 * \brief Get the CAN communication error instance from its network
 *
 * \param network The CAN network
 *
 * \returns error_instance_t The error instance or 0 on error
 */
error_instance prv_can_comm_get_error_instance_from_network(const CanNetwork network) {
    switch (network) {
        case CAN_NETWORK_BMS:
            return ERROR_CAN_COMMUNICATION_INSTANCE_BMS;
        case CAN_NETWORK_PRIMARY:
            return ERROR_CAN_COMMUNICATION_INSTANCE_PRIMARY;
        case CAN_NETWORK_SECONDARY:
            return ERROR_CAN_COMMUNICATION_INSTANCE_SECONDARY;
        default:
            return 0U;
    }
}

/*!
 * \brief Handle the message payload received from the BMS internal CAN network
 *
 * \param index The canlib index of the message
 *
 * \returns can_comm_canlib_payload_handle_callback A pointer to the function callback used to handle the canlib payload
 * or NULL if the index is not valid
 */
can_comm_canlib_payload_handle_callback prv_can_comm_bms_payload_handle(const can_index_t index) {

    constexpr can_index_t cells_voltage_index = BMS_CELLBOARD_CELLS_VOLTAGE_INDEX;
    constexpr can_index_t cells_temperature_index = BMS_CELLBOARD_CELLS_TEMPERATURE_INDEX;
    constexpr can_index_t cellboard_version_index = BMS_CELLBOARD_VERSION_INDEX;
    constexpr can_index_t balancing_status_index = BMS_CELLBOARD_BALANCING_STATUS_INDEX;
    constexpr can_index_t ivt_msg_result_i_index = BMS_IVT_MSG_RESULT_I_INDEX;
    constexpr can_index_t cellboard_error_index = BMS_CELLBOARD_ERROR_INDEX;

    switch (index) {
        case cells_voltage_index:
            return (can_comm_canlib_payload_handle_callback)volt_api_cells_voltage_handle;
        case cells_temperature_index:
            return (can_comm_canlib_payload_handle_callback)temp_api_cells_temperature_handle;
        case cellboard_version_index:
            return (can_comm_canlib_payload_handle_callback)identity_api_cellboard_version_handle;
        case balancing_status_index:
            return (can_comm_canlib_payload_handle_callback)bal_api_cellboard_balancing_status_handle;
        case ivt_msg_result_i_index:
            return (can_comm_canlib_payload_handle_callback)current_api_handle;
        case cellboard_error_index:
            return (can_comm_canlib_payload_handle_callback)error_api_cellboard_handle;
        default:
            return NULL;
    }
}

/*!
 * \brief Handle the message payload received from the primary CAN network of the car
 *
 * \param index The canlib index of the message
 *
 * \returns can_comm_canlib_payload_handle_callback A pointer to the function callback used to handle the canlib payload
 * or NULL if the index is not valid
 */
can_comm_canlib_payload_handle_callback prv_can_comm_primary_payload_handle(const can_index_t index) {

    constexpr can_index_t primary_hv_flash_request_index = PRIMARY_HV_FLASH_REQUEST_INDEX;
    constexpr can_index_t primary_hv_flash_index = PRIMARY_HV_FLASH_INDEX;
    constexpr can_index_t primary_hv_set_status_ecu_index = PRIMARY_HV_SET_STATUS_ECU_INDEX;
    constexpr can_index_t primary_hv_set_status_handcart_index = PRIMARY_HV_SET_STATUS_HANDCART_INDEX;
    constexpr can_index_t primary_hv_set_balancing_status_steering_wheel_index = PRIMARY_HV_SET_BALANCING_STATUS_STEERING_WHEEL_INDEX;
    constexpr can_index_t primary_hv_set_balancing_status_handcart_index = PRIMARY_HV_SET_BALANCING_STATUS_HANDCART_INDEX;

    switch (index) {
        case primary_hv_flash_request_index:
            return (can_comm_canlib_payload_handle_callback)programmer_api_flash_request_handle;
        case primary_hv_flash_index:
            return (can_comm_canlib_payload_handle_callback)programmer_api_flash_handle;
        case primary_hv_set_status_ecu_index:
            return (can_comm_canlib_payload_handle_callback)pcu_api_set_state_from_ecu_handle;
        case primary_hv_set_status_handcart_index:
            return (can_comm_canlib_payload_handle_callback)pcu_api_set_state_from_handcart_handle;
        case primary_hv_set_balancing_status_steering_wheel_index:
            return (can_comm_canlib_payload_handle_callback)bal_api_set_balancing_state_from_steering_wheel_handle;
        case primary_hv_set_balancing_status_handcart_index:
            return (can_comm_canlib_payload_handle_callback)bal_api_set_balancing_state_from_handcart_handle;
        default:
            return NULL;
    }
}

/*!
 * \brief Handle the message payload received from a CAN network
 *
 * \param index The canlib index of the message
 *
 * \returns can_comm_canlib_payload_handle_callback A pointer to the function callback used to handle the canlib payload
 * or NULL if the index is not valid
 */
can_comm_canlib_payload_handle_callback prv_can_comm_payload_handle(const CanNetwork network, const can_index_t index) {
    switch (network) {
        case CAN_NETWORK_BMS:
            return prv_can_comm_bms_payload_handle(index);
        case CAN_NETWORK_PRIMARY:
            return prv_can_comm_primary_payload_handle(index);
        default:
            return NULL;
    }
}

enum CanCommReturnCode can_comm_init(const can_comm_transmit_callback send) {
    if (send == NULL) {
        return CAN_COMM_RC_NULL_POINTER;
    }
    can_comm_disable_all();
    can_comm_handler.send = send;

    // Return values are ignored becuase the buffer addresses are always not NULL
    (void)ring_buffer_api_init(&can_comm_handler.tx_buf, sizeof(struct CanMessage), CAN_COMM_TX_BUFFER_BYTE_SIZE, NULL, NULL, &can_comm_handler.can_arena);
    // TODO: Add callbacks to stop CAN reception interrupt during ring buffer operations?
    (void)ring_buffer_api_init(&can_comm_handler.rx_buf, sizeof(struct CanMessage), CAN_COMM_RX_BUFFER_BYTE_SIZE, NULL, NULL, &can_comm_handler.can_arena);

    // Initialize the canlib device
    device_init(&can_comm_handler.rx_device);
    device_set_address(
        &can_comm_handler.rx_device,
        &can_comm_handler.rx_raw,
        bms_MAX_STRUCT_SIZE_RAW,
        &can_comm_handler.rx_conv,
        bms_MAX_STRUCT_SIZE_CONVERSION);
    return CAN_COMM_RC_OK;
}

void can_comm_enable_all(void) {
    (can_comm_handler.enabled) |= ((1U << CAN_COMM_RX_ENABLE_BIT) | (1U << CAN_COMM_TX_ENABLE_BIT));
}

void can_comm_disable_all(void) {
    (can_comm_handler.enabled) &= (bit_flag8_t) ~((1U << CAN_COMM_RX_ENABLE_BIT) | (1U << CAN_COMM_TX_ENABLE_BIT));
}

bool can_comm_is_enabled_all(void) {
    return CAN_COMM_IS_ENABLED_ALL(can_comm_handler.enabled);
}

void can_comm_enable(const enum CanCommEnableBit bit) {
    if (bit >= CAN_COMM_ENABLE_BIT_COUNT) {
        return;
    }
    CAN_COMM_ENABLE(can_comm_handler.enabled, bit);
}

void can_comm_disable(const enum CanCommEnableBit bit) {
    if (bit >= CAN_COMM_ENABLE_BIT_COUNT) {
        return;
    }
    CAN_COMM_DISABLE(can_comm_handler.enabled, bit);
}

bool can_comm_is_enabled(const enum CanCommEnableBit bit) {
    if (bit >= CAN_COMM_ENABLE_BIT_COUNT) {
        return false;
    }
    return CAN_COMM_IS_ENABLED(can_comm_handler.enabled, bit);
}

enum CanCommReturnCode can_comm_send_immediate(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t *const data,
    const size_t size) {
    if (!CAN_COMM_IS_ENABLED(can_comm_handler.enabled, CAN_COMM_TX_ENABLE_BIT)) {
        return CAN_COMM_RC_DISABLED;
    }

    // Check parameters validity
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMM_RC_INVALID_NETWORK;
    }
    if (network == CAN_NETWORK_BMS && index >= bms_MESSAGE_COUNT) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (network == CAN_NETWORK_PRIMARY && index >= primary_MESSAGE_COUNT) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (frame_type >= CAN_FRAME_TYPE_COUNT) {
        return CAN_COMM_RC_INVALID_FRAME_TYPE;
    }
    // TODO: Change the max payload with the maximum size of the converted struct
    // if (size > CAN_COMM_MAX_PAYLOAD_BYTE_SIZE)
    //     return CAN_COMM_RC_INVALID_PAYLOAD_SIZE;
    if (data == NULL && frame_type != CAN_FRAME_TYPE_REMOTE) {
        return CAN_COMM_RC_NULL_POINTER;
    }

    // Prepare and push message to the buffer
    struct CanMessage msg = {
        .network = network,
        .index = index,
        .frame_type = frame_type
    };
    if (frame_type != CAN_FRAME_TYPE_REMOTE) {
        memcpy(msg.payload.tx, data, size);
    }

    // If the buffer is full run the routine to free space for the new message
    if (ring_buffer_api_is_full(&can_comm_handler.tx_buf)) {
        (void)can_comm_routine();
    }

    // Add and send the new message
    if (ring_buffer_api_push_front(&can_comm_handler.tx_buf, &msg) == RING_BUFFER_RC_OK) {
        return can_comm_routine();
    }
    return CAN_COMM_RC_OVERRUN;
}

enum CanCommReturnCode can_comm_tx_add(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t *const data,
    const size_t size) {
    if (!CAN_COMM_IS_ENABLED(can_comm_handler.enabled, CAN_COMM_TX_ENABLE_BIT)) {
        return CAN_COMM_RC_DISABLED;
    }

    // Check parameters validity
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMM_RC_INVALID_NETWORK;
    }
    if (index < 0) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (network == CAN_NETWORK_BMS && index >= bms_MESSAGE_COUNT) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (network == CAN_NETWORK_PRIMARY && index >= primary_MESSAGE_COUNT) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (frame_type >= CAN_FRAME_TYPE_COUNT) {
        return CAN_COMM_RC_INVALID_FRAME_TYPE;
    }
    // TODO: Change the max payload with the maximum size of the converted struct
    // if (size > CAN_COMM_MAX_PAYLOAD_BYTE_SIZE)
    //     return CAN_COMM_RC_INVALID_PAYLOAD_SIZE;
    if (data == NULL && frame_type != CAN_FRAME_TYPE_REMOTE) {
        return CAN_COMM_RC_NULL_POINTER;
    }

    // Return if a message with the same index is still inside the buffer
    if (can_comm_handler.tx_busy[network][index]) {
        return CAN_COMM_RC_OK;
    }

    // Prepare and push message to the buffer
    struct CanMessage msg = {
        .network = network,
        .index = index,
        .frame_type = frame_type
    };
    if (frame_type != CAN_FRAME_TYPE_REMOTE) {
        memcpy(msg.payload.tx, data, size);
    }

    if (ring_buffer_api_push_back(&can_comm_handler.tx_buf, &msg) == RING_BUFFER_RC_FULL) {
        return CAN_COMM_RC_OVERRUN;
    }
    can_comm_handler.tx_busy[network][index] = true;
    return CAN_COMM_RC_OK;
}

enum CanCommReturnCode can_comm_rx_add(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t *const data,
    const size_t size) {
    if (!CAN_COMM_IS_ENABLED(can_comm_handler.enabled, CAN_COMM_RX_ENABLE_BIT)) {
        return CAN_COMM_RC_DISABLED;
    }

    // Check parameters validity
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMM_RC_INVALID_NETWORK;
    }
    if (index < 0) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (network == CAN_NETWORK_BMS && index >= bms_MESSAGE_COUNT) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (network == CAN_NETWORK_PRIMARY && index >= primary_MESSAGE_COUNT) {
        return CAN_COMM_RC_INVALID_INDEX;
    }
    if (data == NULL && frame_type != CAN_FRAME_TYPE_REMOTE) {
        return CAN_COMM_RC_NULL_POINTER;
    }
    if (size > CAN_COMM_MAX_PAYLOAD_BYTE_SIZE) {
        return CAN_COMM_RC_INVALID_PAYLOAD_SIZE;
    }
    if (frame_type >= CAN_FRAME_TYPE_COUNT) {
        return CAN_COMM_RC_INVALID_FRAME_TYPE;
    }

    // Prepare and push message to the buffer
    struct CanMessage msg = {
        .network = network,
        .index = index,
        .frame_type = frame_type
    };
    if (frame_type != CAN_FRAME_TYPE_REMOTE) {
        memcpy(msg.payload.rx, data, size);
    }

    if (ring_buffer_api_push_back(&can_comm_handler.rx_buf, &msg) == RING_BUFFER_RC_FULL) {
        return CAN_COMM_RC_OVERRUN;
    }
    can_comm_handler.rx_busy[network][index] = true;
    return CAN_COMM_RC_OK;
}

enum CanCommReturnCode can_comm_routine(void) {
    if (!CAN_COMM_IS_ENABLED_ALL(can_comm_handler.enabled)) {
        return CAN_COMM_RC_DISABLED;
    }

    // Handler transmit and receive data
    enum CanCommReturnCode ret = CAN_COMM_RC_OK;
    struct CanMessage tx_msg;
    struct CanMessage rx_msg;

    while (CAN_COMM_IS_ENABLED(can_comm_handler.enabled, CAN_COMM_TX_ENABLE_BIT) &&
           ring_buffer_api_pop_front(&can_comm_handler.tx_buf, &tx_msg) == RING_BUFFER_RC_OK) {
        // Reset the busy flag to notify that the message is not inside the buffer anymore
        can_comm_handler.tx_busy[tx_msg.network][tx_msg.index] = false;

        // Get the right canlib function for the serialization
        id_from_index_function id_from_index = bms_id_from_index;
        serialize_from_id_function serialize_from_id = bms_serialize_from_id;

        if (tx_msg.network == CAN_NETWORK_PRIMARY) {
            id_from_index = primary_id_from_index;
            serialize_from_id = primary_serialize_from_id;
        }

        uint8_t data[CAN_COMM_MAX_PAYLOAD_BYTE_SIZE];
        int size = 0;
        const can_id_t can_id = id_from_index(tx_msg.index);

        if (tx_msg.frame_type != CAN_FRAME_TYPE_REMOTE) {
            // Serialize message
            size = serialize_from_id(
                tx_msg.payload.tx,
                can_id,
                data);
            if (size < 0) {
                return CAN_COMM_RC_CONVERSION_ERROR;
            }
        }

        // Send message
        ret = can_comm_handler.send(
            tx_msg.network,
            can_id,
            tx_msg.frame_type,
            data,
            size);

        /*
         * Set an error in case of problems with CAN communication
         * In case of any invalid data the error is not set because the communication
         * is partially working but the data is not valid
         */
        switch (ret) {
            case CAN_COMM_RC_INVALID_INDEX:
            case CAN_COMM_RC_INVALID_PAYLOAD_SIZE:
            case CAN_COMM_RC_INVALID_FRAME_TYPE:
                // Do nothing
                break;
            case CAN_COMM_RC_OK:
                (void)error_api_reset(ERROR_GROUP_CAN_COMMUNICATION, prv_can_comm_get_error_instance_from_network(tx_msg.network));
                break;
            default:
                // (void)error_set(ERROR_GROUP_CAN_COMMUNICATION, prv_can_comm_get_error_instance_from_network(tx_msg.network));
                break;
        }
    }
    while (CAN_COMM_IS_ENABLED(can_comm_handler.enabled, CAN_COMM_RX_ENABLE_BIT) &&
           ring_buffer_api_pop_front(&can_comm_handler.rx_buf, &rx_msg) == RING_BUFFER_RC_OK) {
        // Reset the busy flag to notify that the message is not inside the buffer anymore
        can_comm_handler.rx_busy[rx_msg.network][rx_msg.index] = false;

        // Get the right canlib function for the serialization
        id_from_index_function id_from_index = bms_id_from_index;
        deserialize_from_id_function deserialize_from_id = bms_devices_deserialize_from_id;

        if (rx_msg.network == CAN_NETWORK_PRIMARY) {
            id_from_index = primary_id_from_index;
            deserialize_from_id = primary_devices_deserialize_from_id;
        }

        const can_id_t can_id = id_from_index(rx_msg.index);

        // TODO: Reset canlib watchdog
        // (void)watchdog_reset(rx_msg.index, timebase_get_time());

        if (rx_msg.frame_type != CAN_FRAME_TYPE_REMOTE) {
            // Deserialize message
            deserialize_from_id(&can_comm_handler.rx_device, can_id, rx_msg.payload.rx);

            can_comm_canlib_payload_handle_callback handle_payload = prv_can_comm_payload_handle(rx_msg.network, rx_msg.index);
            if (handle_payload != NULL) {
                handle_payload(can_comm_handler.rx_device.message);
            }
        } else {
            // TODO: Handler remote requests
        }
    }

    return ret;
}

#ifdef CONF_CAN_COMM_STRINGS_ENABLE

EAGLETRT_STATIC char *can_comm_module_name = "can communication";

EAGLETRT_STATIC char *can_comm_return_code_name[] = {
    [CAN_COMM_RC_OK] = "ok",
    [CAN_COMM_RC_NULL_POINTER] = "null pointer",
    [CAN_COMM_RC_DISABLED] = "disabled",
    [CAN_COMM_RC_OVERRUN] = "overrun",
    [CAN_COMM_RC_INVALID_INDEX] = "invalid index",
    [CAN_COMM_RC_INVALID_PAYLOAD_SIZE] = "invalid payload size",
    [CAN_COMM_RC_INVALID_FRAME_TYPE] = "invalid frame type",
    [CAN_COMM_RC_CONVERSION_ERROR] = "conversion error",
    [CAN_COMM_RC_TRANSMISSION_ERROR] = "transmission error"
};

EAGLETRT_STATIC char *can_comm_return_code_description[] = {
    [CAN_COMM_RC_OK] = "executed succesfully",
    [CAN_COMM_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [CAN_COMM_RC_DISABLED] = "the can manager is not enabled",
    [CAN_COMM_RC_OVERRUN] = "the transmission buffer is full",
    [CAN_COMM_RC_INVALID_INDEX] = "the given index does not correspond to any valid message",
    [CAN_COMM_RC_INVALID_PAYLOAD_SIZE] = "the payload size is greater than the maximum allowed length",
    [CAN_COMM_RC_INVALID_FRAME_TYPE] = "the given frame type does not correspond to any existing can frame type",
    [CAN_COMM_RC_CONVERSION_ERROR] = "can't convert the message correctly",
    [CAN_COMM_RC_TRANSMISSION_ERROR] = "error during message transmission"
};

#endif // CONF_CAN_COMM_STRINGS_ENABLE

#endif // CONF_CAN_COMM_MODULE_ENABLE
