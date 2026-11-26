/*!
 * \file can-communication.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types, enums and structures for the CAN communication module
 */

#ifndef CAN_COMMUNICATION_H
#define CAN_COMMUNICATION_H

#include <stdint.h>

#include "pal.h"
#include "arena-allocator.h"
#include "mainboard-def.h"

/*!
 * \brief Maximum payload size of a classic-CAN frame, in bytes.
 *
 * \details TODO: Should be defined by the libcan
 */
#define CAN_COMMUNICATION_FRAME_DATA_SIZE (8U)

/*!
 * \brief Size of the per-network reception queue.
 */
#define CAN_COMMUNICATION_RX_QUEUE_CAPACITY (128U)

/*!
 * \brief Size of the per-network transmission queue.
 */
#define CAN_COMMUNICATION_TX_QUEUE_CAPACITY (128U)

/*!
 * \brief System return and status codes for CAN communication operations.
 */
enum CanCommunicationReturnCode : uint8_t {
    CAN_COMMUNICATION_RC_OK,                    /*!< Operation completed successfully. */
    CAN_COMMUNICATION_RC_NULL_POINTER,          /*!< A required pointer argument was NULL. */
    CAN_COMMUNICATION_RC_INVALID_NETWORK,       /*!< Network ID is out of range or already initialized. */
    CAN_COMMUNICATION_RC_INVALID_LENGTH,        /*!< Frame length is larger than CAN_COMMUNICATION_FRAME_DATA_SIZE or differs from the expected one. */
    CAN_COMMUNICATION_RC_QUEUE_FULL,            /*!< Target queue has no space left. */
    CAN_COMMUNICATION_RC_QUEUE_EMPTY,           /*!< Target queue is empty. */
    CAN_COMMUNICATION_RC_TRANSMISSION_ERROR,    /*!< The user-supplied send callback reported failure. */
    CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR, /*!< The user-supplied receive callback reported failure. */
    CAN_COMMUNICATION_RC_ERROR,                 /*!< Generic failure (PAL/arena/internal). */
};

/*!
 * \brief Fault confinement state of a CAN transceiver, from the best to the worst
 */
enum CanCommunicationBusState : uint8_t {
    CAN_COMMUNICATION_BUS_STATE_ERROR_ACTIVE,  /*!< Both error counters below 96, normal operation */
    CAN_COMMUNICATION_BUS_STATE_ERROR_WARNING, /*!< An error counter reached 96 */
    CAN_COMMUNICATION_BUS_STATE_ERROR_PASSIVE, /*!< An error counter is above 127: the node can no longer signal the errors it sees */
    CAN_COMMUNICATION_BUS_STATE_BUS_OFF,       /*!< The TX error counter is above 255: the node is disconnected from the bus until it recovers */
    CAN_COMMUNICATION_BUS_STATE_STOPPED,       /*!< The peripheral is not started (initialization or sleep) */
    CAN_COMMUNICATION_BUS_STATE_COUNT,         /*!< Number of bus states */
};

/*!
 * \brief Kind of an error detected on the bus by a CAN transceiver
 */
enum CanCommunicationBusError : uint8_t {
    CAN_COMMUNICATION_BUS_ERROR_NONE,          /*!< No error */
    CAN_COMMUNICATION_BUS_ERROR_STUFF,         /*!< More than 5 equal bits in a row */
    CAN_COMMUNICATION_BUS_ERROR_FORM,          /*!< A fixed format field had a wrong value */
    CAN_COMMUNICATION_BUS_ERROR_ACK,           /*!< Nobody acknowledged a transmitted frame (alone on the bus, no termination, wrong bitrate) */
    CAN_COMMUNICATION_BUS_ERROR_BIT_RECESSIVE, /*!< A recessive bit was sent but a dominant one was read back */
    CAN_COMMUNICATION_BUS_ERROR_BIT_DOMINANT,  /*!< A dominant bit was sent but a recessive one was read back */
    CAN_COMMUNICATION_BUS_ERROR_CRC,           /*!< The CRC of a received frame did not match */
    CAN_COMMUNICATION_BUS_ERROR_COUNT,         /*!< Number of bus error kinds */
};

/*!
 * \brief Snapshot of the state of a CAN transceiver and of its lifetime counters
 *
 * \details The counters never reset and saturate; they count what the hardware
 *          driver sees, below the TX and RX queues of this module
 */
struct CanCommunicationTransceiverStatus {
    uint32_t bitrate;                         /*!< Nominal bitrate in bit/s */
    enum CanCommunicationBusState state;      /*!< Fault confinement state */
    uint8_t tx_error_counter;                 /*!< Transmit error counter (TEC) */
    uint8_t rx_error_counter;                 /*!< Receive error counter (REC) */
    enum CanCommunicationBusError last_error; /*!< The last bus error seen, NONE if no error was ever seen */
    uint8_t tx_mailboxes_free;                /*!< Hardware TX mailboxes free right now */
    uint8_t tx_mailboxes_count;               /*!< Hardware TX mailboxes in total */
    uint8_t rx_fifo_pending;                  /*!< Frames waiting in the hardware RX FIFO right now */
    uint8_t rx_fifo_size;                     /*!< Frames that fit in the hardware RX FIFO */
    uint32_t tx_requested;                    /*!< Frames handed to a TX mailbox */
    uint32_t tx_completed;                    /*!< Frames transmitted and acknowledged */
    uint32_t tx_arbitration_lost;             /*!< Frames aborted after losing the arbitration, the hardware retransmits the others */
    uint32_t tx_errors;                       /*!< Frames aborted after a bus error, the hardware retransmits the others */
    uint32_t tx_mailboxes_busy;               /*!< Frames not sent because every TX mailbox was busy */
    uint32_t rx_received;                     /*!< Frames read from the hardware RX FIFO */
    uint32_t rx_fifo_overruns;                /*!< Times a frame was lost because the hardware RX FIFO was full */
    uint32_t rx_queue_full;                   /*!< Frames lost because the RX queue of this module was full */
    uint32_t bus_errors;                      /*!< Bus errors seen, sampled when the status is read so it is a lower bound */
    uint32_t bus_off_count;                   /*!< Times the transceiver went bus-off, it recovers by itself */
    uint8_t rx_filter_entries;                /*!< Hardware filter entries that let only the handled frames through, 0 if every frame is received */
};

/*!
 * \brief Counters of the TX and RX queues of a network, they never reset and saturate
 */
struct CanCommunicationQueueCounters {
    uint32_t tx_dropped;  /*!< Frames dropped because the TX queue was full */
    uint32_t tx_failed;   /*!< Frames the send callback failed to send */
    uint32_t rx_rejected; /*!< Received frames rejected by the receive callback */
};

/*!
 * \brief Range of standard CAN identifiers, used to configure the hardware filters
 */
struct CanCommunicationIdRange {
    uint32_t first; /*!< First ID of the range */
    uint32_t last;  /*!< Last ID of the range, included */
};

/*!
 * \brief Hardware-agnostic container representing a single raw CAN frame payload.
 */
struct [[gnu::packed]] CanCommunicationFrame {
    uint32_t id;                                     /*!< CAN identifier (Standard 11-bit or Extended 29-bit) */
    uint8_t length;                                  /*!< Count of valid data bytes within the array (0..8) */
    uint8_t data[CAN_COMMUNICATION_FRAME_DATA_SIZE]; /*!< Raw payload byte buffer */
};

/*!
 * \brief Callback invoked when a frame need to be sent through real hardware.
 * \param[in] frame The frame to transmit.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_TRANSMISSION_ERROR on a hardware-side failure.
 */
typedef enum CanCommunicationReturnCode (*can_communication_send_callback)(const struct CanCommunicationFrame *frame);

/*!
 * \brief Callback invoked once per frame removed by can_communication_api_process_rx.
 * \details This function is used to allocate the logic based on the deserialization using
 * canlib.
 *
 * \param[in] frame The frame removed from the RX queue.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR if dispatch fails.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is \c NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if the frame's network is invalid.
 * \retval CAN_COMMUNICATION_RC_ERROR if deserialization fails.
 */
typedef enum CanCommunicationReturnCode (*can_communication_receive_callback)(struct CanCommunicationFrame *frame);

/*!
 * \brief Critical-section enter/exit callback.
 * \details Such callback can be passed at init time of even \c NULL if not required.
 */
typedef void (*can_communication_critical_section_callback)(void);

/*!
 * \brief Callback that tells if the hardware can take a frame right away
 *
 * \return true if a frame can be sent, false if the hardware is full
 */
typedef bool (*can_communication_tx_ready_callback)(void);

/*!
 * \brief Callback that reads the state of the transceiver of a network
 *
 * \details Never called from an interrupt
 *
 * \param[out] status The status to fill
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p status is \c NULL.
 */
typedef enum CanCommunicationReturnCode (*can_communication_status_callback)(struct CanCommunicationTransceiverStatus *status);

/*!
 * \brief Configuration parameters required to open a single bus channel.
 */
struct CanCommunicationNetworkConfig {
    can_communication_send_callback send;                 /*!< Peripheral driver frame transmitter function hook */
    can_communication_receive_callback on_receive;        /*!< Deserialization routing engine function hook */
    can_communication_critical_section_callback cs_enter; /*!< Optional: Thread-safety block lock initializer (May be \c NULL) */
    can_communication_critical_section_callback cs_exit;  /*!< Optional: Thread-safety block lock release initializer (May be \c NULL) */
    can_communication_status_callback get_status;         /*!< Optional: reads the transceiver state for the logs (May be \c NULL) */
    can_communication_tx_ready_callback tx_ready;         /*!< Optional: the TX queue is drained only while it returns true (May be \c NULL) */
};

/*!
 * \brief Explicit tracking layout block dedicated to a singular bus network slot.
 */
struct CanCommunicationNetworkState {
    struct PalHandler pal;                                /*!< Local PAL tracking instance allocated for this network's queues */
    can_communication_send_callback send;                 /*!< Buffered reference pointing to driver transmitter */
    can_communication_receive_callback on_receive;        /*!< Buffered reference pointing to deserializer */
    can_communication_status_callback get_status;         /*!< Reads the transceiver state, \c NULL if not available */
    can_communication_tx_ready_callback tx_ready;         /*!< Tells if the hardware can take a frame, \c NULL if it always can */
    struct CanCommunicationQueueCounters counters;        /*!< Errors of the queues, used by the logs */
    struct CanCommunicationTransceiverStatus last_status; /*!< The transceiver status seen by the last monitor run */
    bool monitored;                                       /*!< last_status is valid, the monitor ran at least once */
    uint32_t runs_since_state_log;                        /*!< Monitor runs since the bus state was last logged */
    uint32_t unlogged_state_changes;                      /*!< Bus state changes not logged because they came too close to the last log */
};

/*!
 * \brief CAN manager handler structure
 *
 * \details One handler covers every network. The arena allocator is shared across
 *          all PAL instances since PAL only allocates at init time.
 */
struct CanCommunicationHandler {
    struct ArenaAllocatorHandler arena;                              /*!< Internal memory arena shared across network handlers */
    struct CanCommunicationNetworkState networks[CAN_NETWORK_COUNT]; /*!< State structures managing individual physical networks */
};

#endif // CAN_COMMUNICATION_H
