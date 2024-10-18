/**
 * @file can-comm.c
 * @date 2024-06-21
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Functions to handle CAN communication with other devices
 */

#include "can-comm.h"

#include <string.h>

#include "fsm.h"
#include "programmer.h"
#include "watchdog.h"
#include "timebase.h"
#include "current.h"
#include "pcu.h"
#include "identity.h"
#include "volt.h"
#include "temp.h"
#include "bal.h"
#include "error.h"

#include "canlib_device.h"

#ifdef CONF_CAN_COMM_MODULE_ENABLE

_STATIC _CanCommHandler hcan_comm;

/**
 * @brief Get the CAN communication error instance from its network
 *
 * @param network The CAN network
 *
 * @return error_instance_t The error instance or 0 on error
 */
error_instance_t _can_comm_get_error_instance_from_network(const CanNetwork network) {
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

/**
 * @brief Handle the message payload received from the BMS internal CAN network
 *
 * @param index The canlib index of the message
 * 
 * @return can_comm_canlib_payload_handle_callback_t A pointer to the function callback used to handle the canlib payload
 * or NULL if the index is not valid
 */
can_comm_canlib_payload_handle_callback_t _can_comm_bms_payload_handle(const can_index_t index) {
    switch (index) {
        case BMS_CELLBOARD_CELLS_VOLTAGE_INDEX:     
            return (can_comm_canlib_payload_handle_callback_t)volt_cells_voltage_handle;
        case BMS_CELLBOARD_CELLS_TEMPERATURE_INDEX:     
            return (can_comm_canlib_payload_handle_callback_t)temp_cells_temperature_handle;
        case BMS_CELLBOARD_FLASH_RESPONSE_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)programmer_cellboard_flash_response_handle;
        case BMS_CELLBOARD_STATUS_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)fsm_cellboard_state_handle;
        case BMS_CELLBOARD_VERSION_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)identity_cellboard_version_handle;
        case BMS_CELLBOARD_BALANCING_STATUS_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)bal_cellboard_balancing_status_handle;
        case BMS_IVT_MSG_RESULT_I_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)current_handle;
        default:
            return NULL;
    }

}

/**
 * @brief Handle the message payload received from the primary CAN network of the car
 *
 * @param index The canlib index of the message
 * 
 * @return can_comm_canlib_payload_handle_callback_t A pointer to the function callback used to handle the canlib payload
 * or NULL if the index is not valid
 */
can_comm_canlib_payload_handle_callback_t _can_comm_primary_payload_handle(const can_index_t index) {
    switch (index) {
        case PRIMARY_HV_FLASH_REQUEST_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)programmer_flash_request_handle;
        case PRIMARY_HV_FLASH_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)programmer_flash_handle;
        case PRIMARY_HV_SET_STATUS_ECU_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)pcu_set_state_from_ecu_handle;
        case PRIMARY_HV_SET_STATUS_HANDCART_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)pcu_set_state_from_handcart_handle;
        case PRIMARY_HV_SET_BALANCING_STATUS_STEERING_WHEEL_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)bal_set_balancing_state_from_steering_wheel_handle;
        case PRIMARY_HV_SET_BALANCING_STATUS_HANDCART_INDEX:
            return (can_comm_canlib_payload_handle_callback_t)bal_set_balancing_state_from_handcart_handle;
        default:
            return NULL;
    }
}

/**
 * @brief Handle the message payload received from a CAN network
 *
 * @param index The canlib index of the message
 * 
 * @return can_comm_canlib_payload_handle_callback_t A pointer to the function callback used to handle the canlib payload
 * or NULL if the index is not valid
 */
can_comm_canlib_payload_handle_callback_t _can_comm_payload_handle(const CanNetwork network, const can_index_t index) {
    switch (network) {
        case CAN_NETWORK_BMS:
            return _can_comm_bms_payload_handle(index);
        case CAN_NETWORK_PRIMARY:
            return _can_comm_primary_payload_handle(index);
        default:
            return NULL;
    }
}

CanCommReturnCode can_comm_init(const can_comm_transmit_callback_t send) {
    if (send == NULL)
        return CAN_COMM_NULL_POINTER;

    CAN_COMM_DISABLE_ALL(hcan_comm.enabled);
    hcan_comm.send = send;

    // Return values are ignored becuase the buffer addresses are always not NULL
    (void)ring_buffer_init(&hcan_comm.tx_buf, CanMessage, CAN_COMM_TX_BUFFER_BYTE_SIZE, NULL, NULL);
    // TODO: Add callbacks to stop CAN reception interrupt during ring buffer operations?
    (void)ring_buffer_init(&hcan_comm.rx_buf, CanMessage, CAN_COMM_RX_BUFFER_BYTE_SIZE, NULL, NULL);

    // Initialize the canlib device
    device_init(&hcan_comm.rx_device);
    device_set_address(
        &hcan_comm.rx_device,
        &hcan_comm.rx_raw,
        bms_MAX_STRUCT_SIZE_RAW,
        &hcan_comm.rx_conv,
        bms_MAX_STRUCT_SIZE_CONVERSION
    );
    return CAN_COMM_OK;
}

void can_comm_enable_all(void) {
    CAN_COMM_ENABLE_ALL(hcan_comm.enabled);
}

void can_comm_disable_all(void) {
    CAN_COMM_DISABLE_ALL(hcan_comm.enabled);
}

bool can_comm_is_enabled_all(void) {
    return CAN_COMM_IS_ENABLED_ALL(hcan_comm.enabled);
}

void can_comm_enable(const CanCommEnableBit bit) {
    if (bit >= CAN_COMM_ENABLE_BIT_COUNT)
        return;
    CAN_COMM_ENABLE(hcan_comm.enabled, bit);
}

void can_comm_disable(const CanCommEnableBit bit) {
    if (bit >= CAN_COMM_ENABLE_BIT_COUNT)
        return;
    CAN_COMM_DISABLE(hcan_comm.enabled, bit);
}

bool can_comm_is_enabled(const CanCommEnableBit bit) {
    if (bit >= CAN_COMM_ENABLE_BIT_COUNT)
        return false;
    return CAN_COMM_IS_ENABLED(hcan_comm.enabled, bit);
}

CanCommReturnCode can_comm_send_immediate(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t * const data,
    const size_t size)
{
    if (!CAN_COMM_IS_ENABLED(hcan_comm.enabled, CAN_COMM_TX_ENABLE_BIT))
        return CAN_COMM_DISABLED;

    // Check parameters validity
    if (network >= CAN_NETWORK_COUNT)
        return CAN_COMM_INVALID_NETWORK;
    if (network == CAN_NETWORK_BMS && index >= bms_MESSAGE_COUNT)
        return CAN_COMM_INVALID_INDEX;
    if (network == CAN_NETWORK_PRIMARY && index >= primary_MESSAGE_COUNT)
        return CAN_COMM_INVALID_INDEX;
    if (frame_type >= CAN_FRAME_TYPE_COUNT)
        return CAN_COMM_INVALID_FRAME_TYPE;
    // TODO: Change the max payload with the maximum size of the converted struct
    // if (size > CAN_COMM_MAX_PAYLOAD_BYTE_SIZE)
    //     return CAN_COMM_INVALID_PAYLOAD_SIZE;
    if (data == NULL && frame_type != CAN_FRAME_TYPE_REMOTE)
        return CAN_COMM_NULL_POINTER;


    // Prepare and push message to the buffer
    CanMessage msg = {
        .network = network,
        .index = index,
        .frame_type = frame_type
    };
    if (frame_type != CAN_FRAME_TYPE_REMOTE)
        memcpy(msg.payload.tx, data, size);

    // If the buffer is full run the routine to free space for the new message
    if (ring_buffer_is_full(&hcan_comm.tx_buf))
        (void)can_comm_routine();

    // Add and send the new message
    if (ring_buffer_push_front(&hcan_comm.tx_buf, &msg) == RING_BUFFER_OK)
        return can_comm_routine();
    return CAN_COMM_OVERRUN;
}

CanCommReturnCode can_comm_tx_add(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t * const data,
    const size_t size)
{
    if (!CAN_COMM_IS_ENABLED(hcan_comm.enabled, CAN_COMM_TX_ENABLE_BIT))
        return CAN_COMM_DISABLED;

    // Check parameters validity
    if (network >= CAN_NETWORK_COUNT)
        return CAN_COMM_INVALID_NETWORK;
    if (network == CAN_NETWORK_BMS && index >= bms_MESSAGE_COUNT)
        return CAN_COMM_INVALID_INDEX;
    if (network == CAN_NETWORK_PRIMARY && index >= primary_MESSAGE_COUNT)
        return CAN_COMM_INVALID_INDEX;
    if (frame_type >= CAN_FRAME_TYPE_COUNT)
        return CAN_COMM_INVALID_FRAME_TYPE;
    // TODO: Change the max payload with the maximum size of the converted struct
    // if (size > CAN_COMM_MAX_PAYLOAD_BYTE_SIZE)
    //     return CAN_COMM_INVALID_PAYLOAD_SIZE;
    if (data == NULL && frame_type != CAN_FRAME_TYPE_REMOTE)
        return CAN_COMM_NULL_POINTER;

    // Return if a message with the same index is still inside the buffer
    if (hcan_comm.tx_busy[network][index])
        return CAN_COMM_OK;

    // Prepare and push message to the buffer
    CanMessage msg = {
        .network = network,
        .index = index,
        .frame_type = frame_type
    };
    if (frame_type != CAN_FRAME_TYPE_REMOTE)
        memcpy(msg.payload.tx, data, size);

    if (ring_buffer_push_back(&hcan_comm.tx_buf, &msg) == RING_BUFFER_FULL)
        return CAN_COMM_OVERRUN;
    hcan_comm.tx_busy[network][index] = true;
    return CAN_COMM_OK;
}

CanCommReturnCode can_comm_rx_add(
    const CanNetwork network,
    const can_index_t index,
    const CanFrameType frame_type,
    uint8_t * const data,
    const size_t size)
{    
    if (!CAN_COMM_IS_ENABLED(hcan_comm.enabled, CAN_COMM_RX_ENABLE_BIT))
        return CAN_COMM_DISABLED;

    // Check parameters validity
    if (network >= CAN_NETWORK_COUNT)
        return CAN_COMM_INVALID_NETWORK;
    if (network == CAN_NETWORK_BMS && index >= bms_MESSAGE_COUNT)
        return CAN_COMM_INVALID_INDEX;
    if (network == CAN_NETWORK_PRIMARY && index >= primary_MESSAGE_COUNT)
        return CAN_COMM_INVALID_INDEX;
    if (data == NULL && frame_type != CAN_FRAME_TYPE_REMOTE)
        return CAN_COMM_NULL_POINTER;
    if (size > CAN_COMM_MAX_PAYLOAD_BYTE_SIZE)
        return CAN_COMM_INVALID_PAYLOAD_SIZE;
    if (frame_type >= CAN_FRAME_TYPE_COUNT)
        return CAN_COMM_INVALID_FRAME_TYPE;

    // Prepare and push message to the buffer
    CanMessage msg = {
        .network = network,
        .index = index,
        .frame_type = frame_type
    };
    if (frame_type != CAN_FRAME_TYPE_REMOTE)
        memcpy(msg.payload.rx, data, size);

    if (ring_buffer_push_back(&hcan_comm.rx_buf, &msg) == RING_BUFFER_FULL)
        return CAN_COMM_OVERRUN;
    hcan_comm.rx_busy[network][index] = true;
    return CAN_COMM_OK;
}

CanCommReturnCode can_comm_routine(void) {
    if (!CAN_COMM_IS_ENABLED_ALL(hcan_comm.enabled))
        return CAN_COMM_DISABLED;

    // Handler transmit and receive data
    CanCommReturnCode ret = CAN_COMM_OK;
    CanMessage tx_msg, rx_msg;
    while (CAN_COMM_IS_ENABLED(hcan_comm.enabled, CAN_COMM_TX_ENABLE_BIT) &&
        ring_buffer_pop_front(&hcan_comm.tx_buf, &tx_msg) == RING_BUFFER_OK)
    {
        // Reset the busy flag to notify that the message is not inside the buffer anymore
        hcan_comm.tx_busy[tx_msg.network][tx_msg.index] = false;

        // Get the right canlib function for the serialization
        id_from_index_t id_from_index = bms_id_from_index;
        serialize_from_id_t serialize_from_id = bms_serialize_from_id;

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
                data
            );
            if (size < 0)
                return CAN_COMM_CONVERSION_ERROR;
        }

        // Send message
        ret = hcan_comm.send(
            tx_msg.network,
            can_id,
            tx_msg.frame_type,
            data,
            size
        );

        /*
         * Set an error in case of problems with CAN communication
         * In case of any invalid data the error is not set because the communication
         * is partially working but the data is not valid
         */
        switch (ret) {
            case CAN_COMM_INVALID_INDEX:
            case CAN_COMM_INVALID_PAYLOAD_SIZE:
            case CAN_COMM_INVALID_FRAME_TYPE:
                // Do nothing
                break;
            case CAN_COMM_OK:
                (void)error_reset(ERROR_GROUP_CAN_COMMUNICATION, _can_comm_get_error_instance_from_network(tx_msg.network));
                break;
            default:
                (void)error_set(ERROR_GROUP_CAN_COMMUNICATION, _can_comm_get_error_instance_from_network(tx_msg.network));
                break;
        }
    }
    while (CAN_COMM_IS_ENABLED(hcan_comm.enabled, CAN_COMM_RX_ENABLE_BIT) &&
        ring_buffer_pop_front(&hcan_comm.rx_buf, &rx_msg) == RING_BUFFER_OK)
    {
        // Reset the busy flag to notify that the message is not inside the buffer anymore
        hcan_comm.rx_busy[rx_msg.network][rx_msg.index] = false;

        // Get the right canlib function for the serialization
        id_from_index_t id_from_index = bms_id_from_index;
        deserialize_from_id_t deserialize_from_id = bms_devices_deserialize_from_id;

        if (rx_msg.network == CAN_NETWORK_PRIMARY) {
            id_from_index = primary_id_from_index;
            deserialize_from_id = primary_devices_deserialize_from_id;
        }

        const can_id_t can_id = id_from_index(rx_msg.index);

        // TODO: Reset canlib watchdog
        // (void)watchdog_reset(rx_msg.index, timebase_get_time());

        if (rx_msg.frame_type != CAN_FRAME_TYPE_REMOTE) {
            // Deserialize message
            deserialize_from_id(&hcan_comm.rx_device, can_id, rx_msg.payload.rx);

            can_comm_canlib_payload_handle_callback_t handle_payload = _can_comm_payload_handle(rx_msg.network, rx_msg.index);
            if (handle_payload == NULL) {
                (void)error_set(ERROR_GROUP_CAN_COMMUNICATION, _can_comm_get_error_instance_from_network(rx_msg.network));
            }
            else {
                (void)error_reset(ERROR_GROUP_CAN_COMMUNICATION, _can_comm_get_error_instance_from_network(rx_msg.network));
                handle_payload(hcan_comm.rx_device.message);
            }
        }
        else { 
            // TODO: Handler remote requests
        }
    }

    return ret;
}

#ifdef CONF_CAN_COMM_STRINGS_ENABLE

_STATIC char * can_comm_module_name = "can communication";

_STATIC char * can_comm_return_code_name[] = {
    [CAN_COMM_OK] = "ok",
    [CAN_COMM_NULL_POINTER] = "null pointer",
    [CAN_COMM_DISABLED] = "disabled",
    [CAN_COMM_OVERRUN] = "overrun",
    [CAN_COMM_INVALID_INDEX] = "invalid index",
    [CAN_COMM_INVALID_PAYLOAD_SIZE] = "invalid payload size",
    [CAN_COMM_INVALID_FRAME_TYPE] = "invalid frame type",
    [CAN_COMM_CONVERSION_ERROR] = "conversion error",
    [CAN_COMM_TRANSMISSION_ERROR] = "transmission error"
};

_STATIC char * can_comm_return_code_description[] = {
    [CAN_COMM_OK] = "executed succesfully",
    [CAN_COMM_NULL_POINTER] = "attempt to dereference a null pointer"
    [CAN_COMM_DISABLED] = "the can manager is not enabled"
    [CAN_COMM_OVERRUN] = "the transmission buffer is full"
    [CAN_COMM_INVALID_INDEX] = "the given index does not correspond to any valid message",
    [CAN_COMM_INVALID_PAYLOAD_SIZE] = "the payload size is greater than the maximum allowed length"
    [CAN_COMM_INVALID_FRAME_TYPE] = "the given frame type does not correspond to any existing can frame type",
    [CAN_COMM_CONVERSION_ERROR] = "can't convert the message correctly",
    [CAN_COMM_TRANSMISSION_ERROR] = "error during message transmission"
};

#endif // CONF_CAN_COMM_STRINGS_ENABLE

#endif // CONF_CAN_COMM_MODULE_ENABLE
