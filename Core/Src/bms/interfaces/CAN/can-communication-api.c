/*!
 * \file can-communication-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Generic API interface for sending and receiving abstracted CAN frames via PAL
 */

#include "can-communication-api.h"

#include <inttypes.h>
#include <string.h>

#include "eagletrt.h"
#include "eagletrt-api.h"
#include "mainboard-conf.h"
#include "mainboard-logger-api.h"
#include "pal-api.h"
#include "arena-allocator-api.h"

EAGLETRT_STATIC struct CanCommunicationHandler handler;

/*! \brief Name of every network, used by the logs */
EAGLETRT_STATIC const char *const can_communication_network_names[CAN_NETWORK_COUNT] = {
    [CAN_NETWORK_PRIMARY] = "primary",
    [CAN_NETWORK_BMS] = "BMS",
};

/*! \brief Name of every bus state, used by the logs */
EAGLETRT_STATIC const char *const can_communication_bus_state_names[CAN_COMMUNICATION_BUS_STATE_COUNT] = {
    [CAN_COMMUNICATION_BUS_STATE_ERROR_ACTIVE] = "error active",
    [CAN_COMMUNICATION_BUS_STATE_ERROR_WARNING] = "error warning",
    [CAN_COMMUNICATION_BUS_STATE_ERROR_PASSIVE] = "error passive",
    [CAN_COMMUNICATION_BUS_STATE_BUS_OFF] = "bus-off",
    [CAN_COMMUNICATION_BUS_STATE_STOPPED] = "stopped",
};

/*! \brief Name of every bus error, used by the logs */
EAGLETRT_STATIC const char *const can_communication_bus_error_names[CAN_COMMUNICATION_BUS_ERROR_COUNT] = {
    [CAN_COMMUNICATION_BUS_ERROR_NONE] = "none",
    [CAN_COMMUNICATION_BUS_ERROR_STUFF] = "stuff",
    [CAN_COMMUNICATION_BUS_ERROR_FORM] = "form",
    [CAN_COMMUNICATION_BUS_ERROR_ACK] = "ACK",
    [CAN_COMMUNICATION_BUS_ERROR_BIT_RECESSIVE] = "bit recessive",
    [CAN_COMMUNICATION_BUS_ERROR_BIT_DOMINANT] = "bit dominant",
    [CAN_COMMUNICATION_BUS_ERROR_CRC] = "CRC",
};

/*!
 * \brief Increment an error counter and tell if the new count must be logged
 *
 * \details The errors can happen at every task run, so they are logged at the
 *          1st, 2nd, 4th, 8th, ... occurrence: the logs grow with the logarithm
 *          of the errors while the count stays visible. The counter saturates
 *
 * \param[in,out] counter The error counter
 *
 * \return true if the new count is a power of two, false otherwise
 */
EAGLETRT_STATIC bool prv_count_and_check_log(uint32_t *const counter) {
    if (*counter < UINT32_MAX) {
        ++(*counter);
    }
    return (*counter & (*counter - 1U)) == 0U;
}

/*!
 * \brief Tell if a counter that can grow by more than one at a time must be logged
 *
 * \details Same rule as prv_count_and_check_log: the counter is logged when it
 *          reaches or passes 1, 2, 4, 8, ...
 *
 * \param[in] previous The value at the last check
 * \param[in] current The value now
 *
 * \return true if a power of two was reached since the last check, false otherwise
 */
EAGLETRT_STATIC bool prv_crossed_power_of_two(const uint32_t previous, const uint32_t current) {
    if (current <= previous) {
        return false;
    }
    if (previous == 0U) {
        return true;
    }
    // Index of the highest bit set: a new power of two was reached if it grew (counts leading zeros)
    return __builtin_clz(current) < __builtin_clz(previous);
}

/*!
 * \brief Shared body of the per-network PAL sending message.
 *
 * \param[in] network Network ID of network to which send the message.
 * \param[in] message PAL message whose payload is a raw CanCommunicationFrame.
 *
 * \retval PAL_RC_OK on success.
 * \retval PAL_RC_NULL_POINTER if \p message or the user send callback is \c NULL.
 * \retval PAL_RC_INVALID_ARGUMENT if the message size does not match a frame.
 * \retval PAL_RC_IO_ERROR if the user send callback reported failure.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_dispatch(enum CanNetwork network, const struct PalMessage *message) {
    if (message == NULL) {
        return PAL_RC_NULL_POINTER;
    }
    if (message->size != sizeof(struct CanCommunicationFrame)) {
        return PAL_RC_INVALID_ARGUMENT;
    }

    struct CanCommunicationFrame frame;
    EAGLETRT_API_UNUSED(memcpy(&frame, message->payload, sizeof(frame)));

    const can_communication_send_callback user_send = handler.networks[network].send;
    if (user_send == NULL) {
        return PAL_RC_NULL_POINTER;
    }
    if (user_send(&frame) != CAN_COMMUNICATION_RC_OK) {
        return PAL_RC_IO_ERROR;
    }
    return PAL_RC_OK;
}

/*!
 * \brief PAL send message for the primary network.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_primary(const struct PalMessage *message) {
    return prv_pal_send_dispatch(CAN_NETWORK_PRIMARY, message);
}

/*!
 * \brief PAL send message for the secondary network.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_bms(const struct PalMessage *message) {
    return prv_pal_send_dispatch(CAN_NETWORK_BMS, message);
}

/*!
 * \brief Initialize one network slot in the module's handler.
 *
 * \param[in] network  Network ID to initialise.
 * \param[in] config   User-supplied callbacks for this network.
 * \param[in] pal_send PAL sending function to this network.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if any required callback is \c NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range.
 * \retval CAN_COMMUNICATION_RC_ERROR if PAL fails to initialise.
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_can_communication_api_init_network(enum CanNetwork network, const struct CanCommunicationNetworkConfig config, pal_send_callback pal_send) {
    if (config.send == NULL || config.on_receive == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    memset(&handler.networks[network], 0, sizeof(handler.networks[network]));

    handler.networks[network].send = config.send;
    handler.networks[network].on_receive = config.on_receive;
    handler.networks[network].get_status = config.get_status;
    handler.networks[network].tx_ready = config.tx_ready;

    if (pal_api_init(
            &handler.networks[network].pal,
            CAN_COMMUNICATION_RX_QUEUE_CAPACITY,
            CAN_COMMUNICATION_TX_QUEUE_CAPACITY,
            (uint32_t)sizeof(struct CanCommunicationFrame),
            NULL,
            pal_send,
            config.cs_enter,
            config.cs_exit,
            &handler.arena) != PAL_RC_OK) {
        MAINBOARD_LOGGER_ERROR("CAN %s: PAL init failed", can_communication_network_names[network]);
        return CAN_COMMUNICATION_RC_ERROR;
    }

    return CAN_COMMUNICATION_RC_OK;
}

/*!
 * \brief Shared body of can_communication_api_add_to_tx and can_communication_api_add_to_rx.
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_enqueue(enum CanNetwork network, const struct CanCommunicationFrame *frame, bool to_tx) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    if (frame->length > CAN_COMMUNICATION_FRAME_DATA_SIZE) {
        return CAN_COMMUNICATION_RC_INVALID_LENGTH;
    }

    struct CanCommunicationFrame buffer = *frame;
    const enum PalReturnCode return_code = to_tx
                                               ? pal_api_add_to_tx_queue(&handler.networks[network].pal, &buffer, (uint32_t)sizeof(buffer))
                                               : pal_api_add_to_rx_queue(&handler.networks[network].pal, (uint8_t *)&buffer, (uint32_t)sizeof(buffer));

    switch (return_code) {
        case PAL_RC_OK:
            return CAN_COMMUNICATION_RC_OK;
        case PAL_RC_QUEUE_FULL:
            return CAN_COMMUNICATION_RC_QUEUE_FULL;
        case PAL_RC_NULL_POINTER:
            return CAN_COMMUNICATION_RC_NULL_POINTER;
        default:
            return CAN_COMMUNICATION_RC_ERROR;
    }
}

enum CanCommunicationReturnCode can_communication_api_init(const struct CanCommunicationNetworkConfig configs[CAN_NETWORK_COUNT]) {
    if (configs == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    memset(&handler, 0, sizeof(handler));
    arena_allocator_api_init(&handler.arena);

    const pal_send_callback pal_send_callbacks[CAN_NETWORK_COUNT] = {
        [CAN_NETWORK_BMS] = prv_pal_send_bms,
        [CAN_NETWORK_PRIMARY] = prv_pal_send_primary
    };

    for (enum CanNetwork network = 0; network < CAN_NETWORK_COUNT; ++network) {
        const enum CanCommunicationReturnCode return_code = prv_can_communication_api_init_network(network, configs[network], pal_send_callbacks[network]);
        if (return_code != CAN_COMMUNICATION_RC_OK) {
            return return_code;
        }
    }

    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_communication_api_add_to_tx(enum CanNetwork network, const struct CanCommunicationFrame *frame) {
    const enum CanCommunicationReturnCode return_code = prv_enqueue(network, frame, true);

    // A full queue is checked after the arguments, so network and frame are valid here
    if (return_code == CAN_COMMUNICATION_RC_QUEUE_FULL && prv_count_and_check_log(&handler.networks[network].counters.tx_dropped)) {
        MAINBOARD_LOGGER_WARNING("CAN %s: TX queue full, %" PRIu32 " frames dropped (last 0x%03" PRIX32 ")",
                                 can_communication_network_names[network],
                                 handler.networks[network].counters.tx_dropped,
                                 frame->id);
    }
    return return_code;
}

// Interrupt context: never log here
enum CanCommunicationReturnCode can_communication_api_add_to_rx(enum CanNetwork network, const struct CanCommunicationFrame *frame) {
    return prv_enqueue(network, frame, false);
}

enum CanCommunicationReturnCode can_communication_api_process_tx(enum CanNetwork network) {
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    struct CanCommunicationNetworkState *const state = &handler.networks[network];
    enum CanCommunicationReturnCode result = CAN_COMMUNICATION_RC_OK;
    enum PalReturnCode return_code = PAL_RC_OK;

    // PAL removes a frame from the queue before sending it, so the queue is drained only while the
    // hardware can take a frame: the others wait for the next run instead of failing and being lost
    while (state->tx_ready == NULL || state->tx_ready()) {
        return_code = pal_api_process_tx(&state->pal);

        if (return_code == PAL_RC_IO_ERROR) {
            result = CAN_COMMUNICATION_RC_TRANSMISSION_ERROR;
            if (prv_count_and_check_log(&state->counters.tx_failed)) {
                MAINBOARD_LOGGER_WARNING("CAN %s: %" PRIu32 " frames failed to send",
                                         can_communication_network_names[network],
                                         state->counters.tx_failed);
            }
        } else if (return_code != PAL_RC_OK) {
            break;
        }
    }

    if (return_code != PAL_RC_OK && return_code != PAL_RC_QUEUE_EMPTY && return_code != PAL_RC_IO_ERROR) {
        MAINBOARD_LOGGER_ERROR("CAN %s: TX processing error %d", can_communication_network_names[network], (int)return_code);
        return CAN_COMMUNICATION_RC_ERROR;
    }

    return result;
}

enum CanCommunicationReturnCode can_communication_api_process_rx(enum CanNetwork network) {
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    const can_communication_receive_callback dispatch = handler.networks[network].on_receive;
    enum CanCommunicationReturnCode result = CAN_COMMUNICATION_RC_OK;
    enum PalReturnCode return_code;

    do {
        struct CanCommunicationFrame frame;
        return_code = pal_api_process_rx(&handler.networks[network].pal, &frame);

        if (return_code == PAL_RC_OK) {
            const enum CanCommunicationReturnCode dispatch_code = dispatch(&frame);
            if (dispatch_code != CAN_COMMUNICATION_RC_OK) {
                result = CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR;
                if (prv_count_and_check_log(&handler.networks[network].counters.rx_rejected)) {
                    MAINBOARD_LOGGER_WARNING("CAN %s: %" PRIu32 " frames rejected (last 0x%03" PRIX32 ", rc %d)",
                                             can_communication_network_names[network],
                                             handler.networks[network].counters.rx_rejected,
                                             frame.id,
                                             (int)dispatch_code);
                }
            }
        }
    } while (return_code == PAL_RC_OK);

    if (return_code != PAL_RC_OK && return_code != PAL_RC_QUEUE_EMPTY) {
        MAINBOARD_LOGGER_ERROR("CAN %s: RX processing error %d", can_communication_network_names[network], (int)return_code);
        return CAN_COMMUNICATION_RC_ERROR;
    }

    return result;
}

/*!
 * \brief Log a change of the bus state, with a level that matches the new state
 *
 * \param[in] network The network
 * \param[in] status The new status
 */
EAGLETRT_STATIC void prv_log_bus_state(enum CanNetwork network, const struct CanCommunicationTransceiverStatus *const status) {
    const char *const name = can_communication_network_names[network];
    const char *const state = can_communication_api_get_bus_state_name(status->state);

    switch (status->state) {
        case CAN_COMMUNICATION_BUS_STATE_ERROR_ACTIVE: // NOLINT(bugprone-branch-clone): the log macros differ only in their arguments
            MAINBOARD_LOGGER_INFO("CAN %s: %s (TEC %u, REC %u)", name, state, status->tx_error_counter, status->rx_error_counter);
            break;
        case CAN_COMMUNICATION_BUS_STATE_BUS_OFF:
            // The peripheral recovers by itself after 128 sequences of 11 recessive bits
            MAINBOARD_LOGGER_ERROR("CAN %s: bus-off (TEC %u), recovering", name, status->tx_error_counter);
            break;
        case CAN_COMMUNICATION_BUS_STATE_ERROR_WARNING:
        case CAN_COMMUNICATION_BUS_STATE_ERROR_PASSIVE:
        case CAN_COMMUNICATION_BUS_STATE_STOPPED:
        default:
            MAINBOARD_LOGGER_WARNING("CAN %s: %s (TEC %u, REC %u, last error %s)",
                                     name,
                                     state,
                                     status->tx_error_counter,
                                     status->rx_error_counter,
                                     can_communication_api_get_bus_error_name(status->last_error));
            break;
    }
}

/*!
 * \brief Log the error counters of the transceiver that reached a power of two since the last run
 *
 * \param[in] network The network
 * \param[in] previous The status of the last run
 * \param[in] current The status now
 */
EAGLETRT_STATIC void prv_log_transceiver_counters(enum CanNetwork network,
                                                  const struct CanCommunicationTransceiverStatus *const previous,
                                                  const struct CanCommunicationTransceiverStatus *const current) {
    const char *const name = can_communication_network_names[network];

    if (prv_crossed_power_of_two(previous->bus_off_count, current->bus_off_count)) {
        MAINBOARD_LOGGER_ERROR("CAN %s: went bus-off %" PRIu32 " times, last error %s",
                               name,
                               current->bus_off_count,
                               can_communication_api_get_bus_error_name(current->last_error));
    }
    if (prv_crossed_power_of_two(previous->bus_errors, current->bus_errors)) {
        MAINBOARD_LOGGER_WARNING("CAN %s: %" PRIu32 " bus errors, last %s (TEC %u, REC %u)",
                                 name,
                                 current->bus_errors,
                                 can_communication_api_get_bus_error_name(current->last_error),
                                 current->tx_error_counter,
                                 current->rx_error_counter);
    }
    if (prv_crossed_power_of_two(previous->tx_errors, current->tx_errors)) {
        MAINBOARD_LOGGER_WARNING("CAN %s: %" PRIu32 " frames aborted after a bus error, last %s",
                                 name,
                                 current->tx_errors,
                                 can_communication_api_get_bus_error_name(current->last_error));
    }
    if (prv_crossed_power_of_two(previous->tx_arbitration_lost, current->tx_arbitration_lost)) {
        MAINBOARD_LOGGER_WARNING("CAN %s: %" PRIu32 " frames aborted after losing arbitration", name, current->tx_arbitration_lost);
    }
    if (prv_crossed_power_of_two(previous->tx_mailboxes_busy, current->tx_mailboxes_busy)) {
        MAINBOARD_LOGGER_WARNING("CAN %s: %" PRIu32 " frames found every TX mailbox busy", name, current->tx_mailboxes_busy);
    }
    if (prv_crossed_power_of_two(previous->rx_fifo_overruns, current->rx_fifo_overruns)) {
        MAINBOARD_LOGGER_WARNING("CAN %s: RX FIFO overrun %" PRIu32 " times, frames lost", name, current->rx_fifo_overruns);
    }
    if (prv_crossed_power_of_two(previous->rx_queue_full, current->rx_queue_full)) {
        MAINBOARD_LOGGER_WARNING("CAN %s: RX queue full, %" PRIu32 " frames lost", name, current->rx_queue_full);
    }
}

enum CanCommunicationReturnCode can_communication_api_monitor(enum CanNetwork network) {
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    struct CanCommunicationNetworkState *const state = &handler.networks[network];
    if (state->get_status == NULL) {
        return CAN_COMMUNICATION_RC_OK;
    }

    constexpr uint32_t state_log_interval_runs = MAINBOARD_TASKS_CAN_STATE_LOG_INTERVAL_MS / MAINBOARD_TASKS_CAN_MONITOR_PERIOD_MS;

    struct CanCommunicationTransceiverStatus status;
    if (state->get_status(&status) != CAN_COMMUNICATION_RC_OK) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    if (!state->monitored) {
        MAINBOARD_LOGGER_INFO("CAN %s: %" PRIu32 " kbit/s, %s, TEC %u, REC %u",
                              can_communication_network_names[network],
                              status.bitrate / 1000U,
                              can_communication_api_get_bus_state_name(status.state),
                              status.tx_error_counter,
                              status.rx_error_counter);
        if (status.rx_filter_entries > 0U) { // NOLINT(bugprone-branch-clone): the log macros differ only in their arguments
            MAINBOARD_LOGGER_INFO("CAN %s: RX filter of %u entries", can_communication_network_names[network], (unsigned)status.rx_filter_entries);
        } else {
            MAINBOARD_LOGGER_INFO("CAN %s: no RX filter, every frame is received", can_communication_network_names[network]);
        }
        // Log the errors already seen before the first run
        const struct CanCommunicationTransceiverStatus none = { 0 };
        prv_log_transceiver_counters(network, &none, &status);
        // The first change is logged right away
        state->runs_since_state_log = state_log_interval_runs;
    } else {
        if (state->runs_since_state_log < UINT32_MAX) {
            ++state->runs_since_state_log;
        }
        if (status.state != state->last_status.state) {
            if (state->unlogged_state_changes < UINT32_MAX) {
                ++state->unlogged_state_changes;
            }
        }
        if (state->unlogged_state_changes > 0U && state->runs_since_state_log >= state_log_interval_runs) {
            prv_log_bus_state(network, &status);
            if (state->unlogged_state_changes > 1U) {
                MAINBOARD_LOGGER_WARNING("CAN %s: bus state changed %" PRIu32 " times in %u ms",
                                         can_communication_network_names[network],
                                         state->unlogged_state_changes,
                                         (unsigned)(state->runs_since_state_log * MAINBOARD_TASKS_CAN_MONITOR_PERIOD_MS));
            }
            state->unlogged_state_changes = 0U;
            state->runs_since_state_log = 0U;
        }
        if (status.bitrate != state->last_status.bitrate) {
            MAINBOARD_LOGGER_INFO("CAN %s: bitrate changed to %" PRIu32 " kbit/s", can_communication_network_names[network], status.bitrate / 1000U);
        }
        prv_log_transceiver_counters(network, &state->last_status, &status);
    }

    state->last_status = status;
    state->monitored = true;
    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_communication_api_get_status(enum CanNetwork network, struct CanCommunicationTransceiverStatus *status) {
    if (status == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    if (handler.networks[network].get_status == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    return (handler.networks[network].get_status(status) == CAN_COMMUNICATION_RC_OK) ? CAN_COMMUNICATION_RC_OK : CAN_COMMUNICATION_RC_ERROR;
}

struct CanCommunicationQueueCounters can_communication_api_get_queue_counters(enum CanNetwork network) {
    if (network >= CAN_NETWORK_COUNT) {
        return (struct CanCommunicationQueueCounters){ 0 };
    }
    return handler.networks[network].counters;
}

const char *can_communication_api_get_network_name(enum CanNetwork network) {
    return (network < CAN_NETWORK_COUNT) ? can_communication_network_names[network] : "?";
}

const char *can_communication_api_get_bus_state_name(enum CanCommunicationBusState state) {
    return (state < CAN_COMMUNICATION_BUS_STATE_COUNT) ? can_communication_bus_state_names[state] : "?";
}

const char *can_communication_api_get_bus_error_name(enum CanCommunicationBusError error) {
    return (error < CAN_COMMUNICATION_BUS_ERROR_COUNT) ? can_communication_bus_error_names[error] : "?";
}
