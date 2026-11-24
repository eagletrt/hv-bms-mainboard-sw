/*!
 * \file can-communication-info-api.c
 * \date 2026-11-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Information about the CAN communication
 *
 * \warning Once the init function is called all watchdogs for cycle time will be started immediately.
 * Only call the init function once some allowance has passed and all cellboards are fine.
 * Messages received before init are ignored.
 */

#include "can-communication-info-api.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bal-control-api.h"
#include "can-bms.h"
#include "can-communication-info.h"
#include "can-primary.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-status-api.h"
#include "mainboard-watchdogs-api.h"
#include "request-control-api.h"
#include "watchdogs.h"

/*!
 * \brief Information about the messages received on the BMS network
 */
EAGLETRT_STATIC const struct CanCommunicationInfoMessage bms_messages[] = {
    { CAN_BMS_MESSAGE_FRAME_ID_IVTMESSAGERESULTI,                  CAN_IVT_CYCLE_TIME_MS,                                 CAN_MAX_IVT_DROPPED,           MAINBOARD_STATUS_FAULT_STALE_CURRENT           },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE,              can_bms_cycle_time_tsaccellboard1voltage,              CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE,              can_bms_cycle_time_tsaccellboard2voltage,              CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE,              can_bms_cycle_time_tsaccellboard3voltage,              CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE,              can_bms_cycle_time_tsaccellboard4voltage,              CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE,              can_bms_cycle_time_tsaccellboard5voltage,              CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE,              can_bms_cycle_time_tsaccellboard6voltage,              CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard1voltageinfo,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard2voltageinfo,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard3voltageinfo,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard4voltageinfo,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard5voltageinfo,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard6voltageinfo,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES     },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE,          can_bms_cycle_time_tsaccellboard1temperature,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE,          can_bms_cycle_time_tsaccellboard2temperature,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE,          can_bms_cycle_time_tsaccellboard3temperature,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE,          can_bms_cycle_time_tsaccellboard4temperature,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE,          can_bms_cycle_time_tsaccellboard5temperature,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE,          can_bms_cycle_time_tsaccellboard6temperature,          CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard1temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard2temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard3temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard4temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard5temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard6temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED_SCS, MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard1balancingtemperature, CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard2balancingtemperature, CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard3balancingtemperature, CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard4balancingtemperature, CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard5balancingtemperature, CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard6balancingtemperature, CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING,            can_bms_cycle_time_tsaccellboard1balancing,            CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING,            can_bms_cycle_time_tsaccellboard2balancing,            CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING,            can_bms_cycle_time_tsaccellboard3balancing,            CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING,            can_bms_cycle_time_tsaccellboard4balancing,            CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING,            can_bms_cycle_time_tsaccellboard5balancing,            CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING,            can_bms_cycle_time_tsaccellboard6balancing,            CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1FSM,                  can_bms_cycle_time_tsaccellboard1fsm,                  CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2FSM,                  can_bms_cycle_time_tsaccellboard2fsm,                  CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3FSM,                  can_bms_cycle_time_tsaccellboard3fsm,                  CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4FSM,                  can_bms_cycle_time_tsaccellboard4fsm,                  CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5FSM,                  can_bms_cycle_time_tsaccellboard5fsm,                  CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6FSM,                  can_bms_cycle_time_tsaccellboard6fsm,                  CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1ERROR,                can_bms_cycle_time_tsaccellboard1error,                CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2ERROR,                can_bms_cycle_time_tsaccellboard2error,                CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3ERROR,                can_bms_cycle_time_tsaccellboard3error,                CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4ERROR,                can_bms_cycle_time_tsaccellboard4error,                CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5ERROR,                can_bms_cycle_time_tsaccellboard5error,                CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6ERROR,                can_bms_cycle_time_tsaccellboard6error,                CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSION,              can_bms_cycle_time_tsaccellboard1version,              CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSION,              can_bms_cycle_time_tsaccellboard2version,              CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSION,              can_bms_cycle_time_tsaccellboard3version,              CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSION,              can_bms_cycle_time_tsaccellboard4version,              CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSION,              can_bms_cycle_time_tsaccellboard5version,              CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSION,              can_bms_cycle_time_tsaccellboard6version,              CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSIONINFO,          can_bms_cycle_time_tsaccellboard1versioninfo,          CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSIONINFO,          can_bms_cycle_time_tsaccellboard2versioninfo,          CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSIONINFO,          can_bms_cycle_time_tsaccellboard3versioninfo,          CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSIONINFO,          can_bms_cycle_time_tsaccellboard4versioninfo,          CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSIONINFO,          can_bms_cycle_time_tsaccellboard5versioninfo,          CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSIONINFO,          can_bms_cycle_time_tsaccellboard6versioninfo,          CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSION,        can_bms_cycle_time_tsaccellboard1libcanversion,        CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSION,        can_bms_cycle_time_tsaccellboard2libcanversion,        CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSION,        can_bms_cycle_time_tsaccellboard3libcanversion,        CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSION,        can_bms_cycle_time_tsaccellboard4libcanversion,        CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSION,        can_bms_cycle_time_tsaccellboard5libcanversion,        CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSION,        can_bms_cycle_time_tsaccellboard6libcanversion,        CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard1libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard2libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard3libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard4libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard5libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard6libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED,     MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS  },
};

/*!
 * \brief Information about the messages received on the primary network
 *
 * \details The one-shot messages (e.g. flashing) are not listed since they have no cycle time.
 *          No fault is set on this network, the requests are cleared instead
 */
EAGLETRT_STATIC const struct CanCommunicationInfoMessage primary_messages[] = {
    { CAN_PRIMARY_MESSAGE_FRAME_ID_BMSSET,                    can_primary_cycle_time_bmsset,                    CAN_MAX_PRIMARY_DROPPED_SCS, MAINBOARD_STATUS_FAULT_COUNT },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYTSACBALANCINGSET, can_primary_cycle_time_raspberrytsacbalancingset, CAN_MAX_PRIMARY_DROPPED,     MAINBOARD_STATUS_FAULT_COUNT },
};

/*! \brief Number of messages received on the BMS network */
#define BMS_MESSAGES_COUNT (sizeof(bms_messages) / sizeof(bms_messages[0]))
/*! \brief Number of messages received on the primary network */
#define PRIMARY_MESSAGES_COUNT (sizeof(primary_messages) / sizeof(primary_messages[0]))

/*! \brief Number of timeframes kept, one being written and one completed */
#define TIMEFRAMES_COUNT (2U)

/*!
 * \brief Runtime state of a monitored message
 *
 * \details Each message has two watchdogs:
 *          - the cycle watchdog times out every exact cycle time and is
 *            restarted from its own callback, so it marks the slots in which
 *            a message is expected without accumulating the wiggle room
 *          - the leeway watchdog is started by the cycle watchdog and gives
 *            the message CAN_CYCLE_TIME_WIGGLE_ROOM_MS to arrive, when it
 *            times out the message is counted as dropped
 *
 *          When a message is received the leeway watchdog is stopped and the
 *          cycle watchdog is restarted, which also re-syncs the slots to the
 *          sender
 */
struct CanCommunicationInfoMessageState {
    struct Watchdog cycle_watchdog;            /*!< The watchdog marking the expected arrival of the message */
    struct Watchdog leeway_watchdog;           /*!< The watchdog counting the dropped messages */
    uint8_t consecutive_drops;                 /*!< The amount of consecutive messages dropped */
    uint16_t cumulative_drops;                 /*!< The cumulative amount of messages dropped */
    uint8_t timeframe_drops[TIMEFRAMES_COUNT]; /*!< The amount of messages dropped in each timeframe */
    enum CanNetwork network;                   /*!< The network of the message, used by the watchdog callbacks */
    size_t index;                              /*!< The index of the message in the table of its network */
};

/*!
 * \brief A monitored network
 *
 * \details states is indexed like messages
 */
struct CanCommunicationInfoNetwork {
    const struct CanCommunicationInfoMessage *messages;                      /*!< The monitored messages of the network */
    struct CanCommunicationInfoMessageState *states;                         /*!< The runtime state of each monitored message */
    size_t count;                                                            /*!< The number of monitored messages */
    void (*handle_fatal)(const struct CanCommunicationInfoMessage *message); /*!< Called once when a message reaches its maximum number of consecutive drops */
};

/*!
 * \brief CAN communication info handler structure
 */
struct CanCommunicationInfoHandler {
    struct Watchdog timeframe_switcher; /*!< The watchdog that swaps the timeframe being written every CAN_TIMEFRAME */
    uint8_t writing_timeframe_index;    /*!< The index of the timeframe drops currently being written */
    bool initialized;                   /*!< True once every watchdog has been initialized and started */
};

/*!
 * \brief Handle a message that dropped out on the BMS network
 *
 * \details Sets the stale fault of the data carried by the message
 *
 * \param[in] message The message that dropped out
 */
EAGLETRT_STATIC void prv_handle_bms_message_fatal(const struct CanCommunicationInfoMessage *message) {
    EAGLETRT_API_UNUSED(mainboard_status_api_set_fault(message->fault));
}

/*!
 * \brief Handle a message that dropped out on the primary network
 *
 * \details Clears the requests carried by the message through the request
 *          control, which owns the request flags
 *
 *          The TS request goes to off, which also clears the TS on and keep
 *          TS on requests. The balancing request is turned off keeping the
 *          last threshold received
 *
 * \param[in] message The message that dropped out
 */
EAGLETRT_STATIC void prv_handle_primary_message_fatal(const struct CanCommunicationInfoMessage *message) {
    struct RequestControlRequest request = { 0 };

    switch (message->id) {
        case CAN_PRIMARY_MESSAGE_FRAME_ID_BMSSET:
            request.request_type = REQUEST_CONTROL_REQUEST_TS;
            request.request_data.ts_request_state = REQUEST_CONTROL_TS_REQUEST_STATE_OFF;
            break;

        case CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYTSACBALANCINGSET:
            request.request_type = REQUEST_CONTROL_REQUEST_BALANCING;
            request.request_data.balancing_request_data.start = false;
            request.request_data.balancing_request_data.parameters.threshold = bal_control_api_get_threshold();
            break;

        default:
            return;
    }

    EAGLETRT_API_UNUSED(request_control_api_handle_request(request));
}

EAGLETRT_STATIC struct CanCommunicationInfoMessageState info_bms_states[BMS_MESSAGES_COUNT];
EAGLETRT_STATIC struct CanCommunicationInfoMessageState info_primary_states[PRIMARY_MESSAGES_COUNT];

/*!
 * \brief The monitored networks, indexed by enum CanNetwork
 */
EAGLETRT_STATIC const struct CanCommunicationInfoNetwork info_networks[CAN_NETWORK_COUNT] = {
    [CAN_NETWORK_PRIMARY] = { primary_messages, info_primary_states, PRIMARY_MESSAGES_COUNT, prv_handle_primary_message_fatal },
    [CAN_NETWORK_BMS] = { bms_messages,     info_bms_states,     BMS_MESSAGES_COUNT,     prv_handle_bms_message_fatal     },
};

EAGLETRT_STATIC struct CanCommunicationInfoHandler info_handler;

/*!
 * \brief Called when a message is expected, opens its leeway window and schedules its next slot
 *
 * \details A failed restart latches the watchdogs fault in the wrapper
 *
 * \param[in] context The state of the message
 */
EAGLETRT_STATIC void prv_handle_cycle_elapsed(void *context) {
    struct CanCommunicationInfoMessageState *const state = context;

    EAGLETRT_API_UNUSED(mainboard_watchdogs_api_restart(&state->leeway_watchdog));
    EAGLETRT_API_UNUSED(mainboard_watchdogs_api_restart(&state->cycle_watchdog));
}

/*!
 * \brief Called when a message did not arrive within the wiggle room, counts it as dropped
 *
 * \details The counters saturate, the fatal handler of the network is called
 *          once, when the message reaches its maximum number of consecutive drops
 *
 * \param[in] context The state of the message
 */
EAGLETRT_STATIC void prv_handle_message_dropped(void *context) {
    struct CanCommunicationInfoMessageState *const state = context;
    const struct CanCommunicationInfoNetwork *const network = &info_networks[state->network];
    const struct CanCommunicationInfoMessage *const message = &network->messages[state->index];
    uint8_t *const timeframe_drops = &state->timeframe_drops[info_handler.writing_timeframe_index];

    if (state->cumulative_drops < UINT16_MAX) {
        ++state->cumulative_drops;
    }
    if (state->consecutive_drops < UINT8_MAX) {
        ++state->consecutive_drops;
    }
    if (*timeframe_drops < UINT8_MAX) {
        ++(*timeframe_drops);
    }

    if (state->consecutive_drops == message->max_dropped) {
        network->handle_fatal(message);
    }
}

/*!
 * \brief Called every CAN_TIMEFRAME, swaps the timeframe being written and clears it
 *
 * \param[in] context Unused
 */
EAGLETRT_STATIC void prv_handle_timeframe_switch(void *context) {
    EAGLETRT_API_UNUSED(context);

    info_handler.writing_timeframe_index = !info_handler.writing_timeframe_index;
    const uint8_t timeframe = info_handler.writing_timeframe_index;

    for (size_t n = 0; n < CAN_NETWORK_COUNT; n++) {
        const struct CanCommunicationInfoNetwork *const network = &info_networks[n];
        for (size_t i = 0; i < network->count; i++) {
            network->states[i].timeframe_drops[timeframe] = 0U;
        }
    }

    EAGLETRT_API_UNUSED(mainboard_watchdogs_api_restart(&info_handler.timeframe_switcher));
}

/*!
 * \brief Get the index of the last completed timeframe
 *
 * \return The index of the timeframe drops not currently being written
 */
EAGLETRT_STATIC uint8_t prv_completed_timeframe_index(void) {
    return (uint8_t)!info_handler.writing_timeframe_index;
}

/*!
 * \brief Find the state of a monitored message
 *
 * \param[in] network The network of the message
 * \param[in] message_id The ID of the message
 * \param[out] state The state of the message, untouched if not found
 *
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK if the network is out of range
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_INDEX if the message is not monitored on the network
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
EAGLETRT_STATIC enum CanCommunicationInfoReturnCode prv_find_state(const enum CanNetwork network,
                                                                   const can_id message_id,
                                                                   struct CanCommunicationInfoMessageState **const state) {
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK;
    }

    const struct CanCommunicationInfoNetwork *const info_network = &info_networks[network];
    for (size_t i = 0; i < info_network->count; i++) {
        if (message_id == info_network->messages[i].id) {
            *state = &info_network->states[i];
            return CAN_COMMUNICATION_INFO_RC_OK;
        }
    }

    return CAN_COMMUNICATION_INFO_RC_INVALID_INDEX;
}

/*!
 * \brief Initialize the cycle and leeway watchdogs of a message and start the cycle watchdog
 *
 * \details The wiggle room must be smaller than the cycle time, otherwise the
 *          leeway window of a slot would overlap the next slot
 *
 * \param[in,out] state The state of the message, passed as the context of the callbacks
 * \param[in] cycle_time The cycle time of the message in ms
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if the cycle time is not greater than the wiggle room or a watchdog fails to initialize or start
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
EAGLETRT_STATIC enum CanCommunicationInfoReturnCode prv_init_message_watchdogs(struct CanCommunicationInfoMessageState *const state,
                                                                               const int32_t cycle_time) {
    if (cycle_time <= (int32_t)CAN_CYCLE_TIME_WIGGLE_ROOM_MS) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    if (mainboard_watchdogs_api_init_watchdog(&state->cycle_watchdog, (milliseconds)cycle_time, prv_handle_cycle_elapsed, state) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    if (mainboard_watchdogs_api_init_watchdog(&state->leeway_watchdog, CAN_CYCLE_TIME_WIGGLE_ROOM_MS, prv_handle_message_dropped, state) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    if (mainboard_watchdogs_api_start(&state->cycle_watchdog) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    return CAN_COMMUNICATION_INFO_RC_OK;
}

enum CanCommunicationInfoReturnCode can_communication_info_init(void) {
    if (mainboard_watchdogs_api_init_watchdog(&info_handler.timeframe_switcher, CAN_TIMEFRAME, prv_handle_timeframe_switch, NULL) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    for (enum CanNetwork n = 0; n < CAN_NETWORK_COUNT; n++) {
        const struct CanCommunicationInfoNetwork *const network = &info_networks[n];

        for (size_t i = 0; i < network->count; i++) {
            struct CanCommunicationInfoMessageState *const state = &network->states[i];
            state->network = n;
            state->index = i;

            if (prv_init_message_watchdogs(state, network->messages[i].cycle_time) != CAN_COMMUNICATION_INFO_RC_OK) {
                return CAN_COMMUNICATION_INFO_RC_ERROR;
            }
        }
    }

    if (mainboard_watchdogs_api_start(&info_handler.timeframe_switcher) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    info_handler.initialized = true;
    return CAN_COMMUNICATION_INFO_RC_OK;
}

enum CanCommunicationInfoReturnCode can_communication_info_handle_message(enum CanNetwork network, can_id message_id) {
    // The watchdogs are not initialized yet: touching them would latch the watchdogs fault
    if (!info_handler.initialized) {
        return CAN_COMMUNICATION_INFO_RC_NOT_INITIALIZED;
    }

    struct CanCommunicationInfoMessageState *state = NULL;
    const enum CanCommunicationInfoReturnCode rc = prv_find_state(network, message_id, &state);
    if (rc != CAN_COMMUNICATION_INFO_RC_OK) {
        return rc;
    }

    EAGLETRT_API_UNUSED(mainboard_watchdogs_api_stop(&state->leeway_watchdog));
    EAGLETRT_API_UNUSED(mainboard_watchdogs_api_restart(&state->cycle_watchdog));
    state->consecutive_drops = 0U;
    return CAN_COMMUNICATION_INFO_RC_OK;
}

enum CanCommunicationInfoReturnCode can_communication_info_get_consecutive_drops(enum CanNetwork network, can_id message_id, uint8_t *drops) {
    if (drops == NULL) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    struct CanCommunicationInfoMessageState *state = NULL;
    const enum CanCommunicationInfoReturnCode rc = prv_find_state(network, message_id, &state);
    if (rc != CAN_COMMUNICATION_INFO_RC_OK) {
        return rc;
    }

    *drops = state->consecutive_drops;
    return CAN_COMMUNICATION_INFO_RC_OK;
}

enum CanCommunicationInfoReturnCode can_communication_info_get_cumulative_drops(enum CanNetwork network, can_id message_id, uint16_t *drops) {
    if (drops == NULL) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    struct CanCommunicationInfoMessageState *state = NULL;
    const enum CanCommunicationInfoReturnCode rc = prv_find_state(network, message_id, &state);
    if (rc != CAN_COMMUNICATION_INFO_RC_OK) {
        return rc;
    }

    *drops = state->cumulative_drops;
    return CAN_COMMUNICATION_INFO_RC_OK;
}

enum CanCommunicationInfoReturnCode can_communication_info_get_timeframe_drops(enum CanNetwork network, can_id message_id, uint8_t *drops) {
    if (drops == NULL) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    struct CanCommunicationInfoMessageState *state = NULL;
    const enum CanCommunicationInfoReturnCode rc = prv_find_state(network, message_id, &state);
    if (rc != CAN_COMMUNICATION_INFO_RC_OK) {
        return rc;
    }

    *drops = state->timeframe_drops[prv_completed_timeframe_index()];
    return CAN_COMMUNICATION_INFO_RC_OK;
}

enum CanCommunicationInfoReturnCode can_communication_info_get_total_timeframe_drops(enum CanNetwork network, uint32_t *drops) {
    if (drops == NULL) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }
    if (network >= CAN_NETWORK_COUNT) {
        return CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK;
    }

    const struct CanCommunicationInfoNetwork *const info_network = &info_networks[network];
    const uint8_t timeframe = prv_completed_timeframe_index();
    uint32_t total = 0U;

    for (size_t i = 0; i < info_network->count; i++) {
        total += info_network->states[i].timeframe_drops[timeframe];
    }

    *drops = total;
    return CAN_COMMUNICATION_INFO_RC_OK;
}
