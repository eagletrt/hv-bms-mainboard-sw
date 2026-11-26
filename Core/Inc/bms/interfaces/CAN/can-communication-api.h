/*!
 * \file can-communication-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Generic API interface for sending and receiving abstracted CAN frames via PAL
 */

#ifndef CAN_COMMUNICATION_API_H
#define CAN_COMMUNICATION_API_H

#include "can-communication.h"

/*!
 * \brief Initialize the CAN communication module.
 * \param[in] configs Per-network configuration array containing all required callbacks.
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p configs is \c NULL or any of its required callbacks is \c NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if a network ID within the initialization sequence is out of range.
 * \retval CAN_COMMUNICATION_RC_ERROR if PAL or the arena fail to set up.
 */
enum CanCommunicationReturnCode can_communication_api_init(const struct CanCommunicationNetworkConfig configs[CAN_NETWORK_COUNT]);

/*!
 * \brief Pushes an abstracted data frame into a specific network's local TX buffer queue.
 *
 * \param[in] network The targeted physical CAN network the frame is destined for.
 * \param[in] frame Pointer to the completely formed frame data structure to stage.
 *
 * \retval CAN_COMMUNICATION_RC_OK If the frame was successfully appended to the queue.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER If the provided \p frame pointer evaluates to NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK If \p network matches or exceeds CAN_COMMUNICATION_NET_COUNT.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH If \p frame->length exceeds CAN_COMMUNICATION_FRAME_DATA_SIZE.
 * \retval CAN_COMMUNICATION_RC_QUEUE_FULL If the underlying transmission queue is completely saturated.
 * \retval CAN_COMMUNICATION_RC_ERROR If a low-level structural exception occurs inside the PAL ring buffer.
 */
enum CanCommunicationReturnCode can_communication_api_add_to_tx(enum CanNetwork network,
                                                                const struct CanCommunicationFrame *frame);

/*!
 * \brief Pushes a raw frame received from a peripheral bus into a specific network's local RX buffer queue.
 * \note Call this function directly inside low-level hardware ISR handler routines.
 *
 * \param[in] network The physical CAN network where the incoming packet arrived.
 * \param[in] frame Pointer to the data structure holding the raw ID, length, and payload metrics.
 *
 * \retval CAN_COMMUNICATION_RC_OK If the frame was safely staged for future state processing.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER If the provided \p frame pointer evaluates to NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK If \p network matches or exceeds CAN_COMMUNICATION_NET_COUNT.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH If \p frame->length exceeds CAN_COMMUNICATION_FRAME_DATA_SIZE.
 * \retval CAN_COMMUNICATION_RC_QUEUE_FULL If the reception queue is saturated.
 * \retval CAN_COMMUNICATION_RC_ERROR If an internal verification error occurs within the PAL layer.
 */
enum CanCommunicationReturnCode can_communication_api_add_to_rx(enum CanNetwork network,
                                                                const struct CanCommunicationFrame *frame);

/*!
 * \brief Flushes and drains the entire TX queue of a specific network out to physical hardware.
 *
 * \details This flushes staged frames sequentially, passing them to the user-supplied
 * hardware transmitter callback. It loops continuously until the queue reports empty or a failure occurs.
 *
 * \param[in] network The physical CAN network whose transmission queue should be flushed.
 *
 * \retval CAN_COMMUNICATION_RC_OK If all queued elements were completely flushed out.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK If \p network matches or exceeds CAN_COMMUNICATION_NET_COUNT.
 * \retval CAN_COMMUNICATION_RC_TRANSMISSION_ERROR If the underlying hardware driver callback reports an operation failure.
 * \retval CAN_COMMUNICATION_RC_ERROR If a low-level internal driver fault occurs.
 */
enum CanCommunicationReturnCode can_communication_api_process_tx(enum CanNetwork network);

/*!
 * \brief Drains the entire RX queue of a specific network, dispatching frames to the application layer.
 *
 * \details This pops frames out of the network's reception buffer and routes them through
 * the registered `on_receive` deserialization engine callback. It processes elements until empty.
 *
 * \param[in] network The physical CAN network whose reception queue should be processed.
 *
 * \retval CAN_COMMUNICATION_RC_OK If all received messages were popped and processed cleanly.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK If \p network matches or exceeds CAN_COMMUNICATION_NET_COUNT.
 * \retval CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR If the user routing callback fails to process an extracted frame.
 * \retval CAN_COMMUNICATION_RC_ERROR If a PAL internal pipeline fault is encountered.
 */
enum CanCommunicationReturnCode can_communication_api_process_rx(enum CanNetwork network);

/*!
 * \brief Read the transceiver status of a network and log what changed since the last run
 *
 * \details The first run logs the bitrate and the state, then the changes of the bus
 *          state are logged at most once per MAINBOARD_TASKS_CAN_STATE_LOG_INTERVAL_MS
 *          (the ones in between are counted) and every error counter is logged when it
 *          reaches 1, 2, 4, 8, ... so that a broken bus does not flood the output.
 *          Call it every MAINBOARD_TASKS_CAN_MONITOR_PERIOD_MS, never from an interrupt
 *
 * \param[in] network The network to monitor
 *
 * \retval CAN_COMMUNICATION_RC_OK on success, also if the network has no status callback
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range
 * \retval CAN_COMMUNICATION_RC_ERROR if the status callback failed
 */
enum CanCommunicationReturnCode can_communication_api_monitor(enum CanNetwork network);

/*!
 * \brief Read the current transceiver status of a network
 *
 * \param[in] network The network
 * \param[out] status The status to fill
 *
 * \retval CAN_COMMUNICATION_RC_OK on success
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p status is \c NULL or the network has no status callback
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range
 * \retval CAN_COMMUNICATION_RC_ERROR if the status callback failed
 */
enum CanCommunicationReturnCode can_communication_api_get_status(enum CanNetwork network, struct CanCommunicationTransceiverStatus *status);

/*!
 * \brief Get the error counters of the queues of a network
 *
 * \param[in] network The network
 *
 * \return The counters, all zero if \p network is out of range
 */
struct CanCommunicationQueueCounters can_communication_api_get_queue_counters(enum CanNetwork network);

/*!
 * \brief Get the name of a network
 *
 * \param[in] network The network
 *
 * \return The name, "?" if \p network is out of range
 */
const char *can_communication_api_get_network_name(enum CanNetwork network);

/*!
 * \brief Get the name of a bus state
 *
 * \param[in] state The bus state
 *
 * \return The name, "?" if \p state is out of range
 */
const char *can_communication_api_get_bus_state_name(enum CanCommunicationBusState state);

/*!
 * \brief Get the name of a bus error
 *
 * \param[in] error The bus error
 *
 * \return The name, "?" if \p error is out of range
 */
const char *can_communication_api_get_bus_error_name(enum CanCommunicationBusError error);

#endif // CAN_COMMUNICATION_API_H
