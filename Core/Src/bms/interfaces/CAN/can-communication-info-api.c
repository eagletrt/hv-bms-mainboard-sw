/*!
 * \file can-communication-info-api.c
 * \date 2026-11-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Information about the CAN communication
 */

#include "can-communication-info-api.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "can-bms.h"
#include "can-primary.h"
#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-watchdogs-api.h"
#include "watchdogs.h"

/*!
 * \brief Information about the messages received on the BMS network
 */
EAGLETRT_STATIC const struct CanCommunicationInfoBmsMessage bms_messages[] = {
    { CAN_BMS_MESSAGE_FRAME_ID_IVTMESSAGERESULTI,                  CAN_IVT_CYCLE_TIME_MS,                                 CAN_MAX_IVT_DROPPED       },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE,              can_bms_cycle_time_tsaccellboard1voltage,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE,              can_bms_cycle_time_tsaccellboard2voltage,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE,              can_bms_cycle_time_tsaccellboard3voltage,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE,              can_bms_cycle_time_tsaccellboard4voltage,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE,              can_bms_cycle_time_tsaccellboard5voltage,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE,              can_bms_cycle_time_tsaccellboard6voltage,              CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard1voltageinfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard2voltageinfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard3voltageinfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard4voltageinfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard5voltageinfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard6voltageinfo,          CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE,          can_bms_cycle_time_tsaccellboard1temperature,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE,          can_bms_cycle_time_tsaccellboard2temperature,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE,          can_bms_cycle_time_tsaccellboard3temperature,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE,          can_bms_cycle_time_tsaccellboard4temperature,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE,          can_bms_cycle_time_tsaccellboard5temperature,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE,          can_bms_cycle_time_tsaccellboard6temperature,          CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard1temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard2temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard3temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard4temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard5temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard6temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard1balancingtemperature, CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard2balancingtemperature, CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard3balancingtemperature, CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard4balancingtemperature, CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard5balancingtemperature, CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard6balancingtemperature, CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING,            can_bms_cycle_time_tsaccellboard1balancing,            CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING,            can_bms_cycle_time_tsaccellboard2balancing,            CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING,            can_bms_cycle_time_tsaccellboard3balancing,            CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING,            can_bms_cycle_time_tsaccellboard4balancing,            CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING,            can_bms_cycle_time_tsaccellboard5balancing,            CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING,            can_bms_cycle_time_tsaccellboard6balancing,            CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1FSM,                  can_bms_cycle_time_tsaccellboard1fsm,                  CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2FSM,                  can_bms_cycle_time_tsaccellboard2fsm,                  CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3FSM,                  can_bms_cycle_time_tsaccellboard3fsm,                  CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4FSM,                  can_bms_cycle_time_tsaccellboard4fsm,                  CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5FSM,                  can_bms_cycle_time_tsaccellboard5fsm,                  CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6FSM,                  can_bms_cycle_time_tsaccellboard6fsm,                  CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1ERROR,                can_bms_cycle_time_tsaccellboard1error,                CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2ERROR,                can_bms_cycle_time_tsaccellboard2error,                CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3ERROR,                can_bms_cycle_time_tsaccellboard3error,                CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4ERROR,                can_bms_cycle_time_tsaccellboard4error,                CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5ERROR,                can_bms_cycle_time_tsaccellboard5error,                CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6ERROR,                can_bms_cycle_time_tsaccellboard6error,                CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSION,              can_bms_cycle_time_tsaccellboard1version,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSION,              can_bms_cycle_time_tsaccellboard2version,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSION,              can_bms_cycle_time_tsaccellboard3version,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSION,              can_bms_cycle_time_tsaccellboard4version,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSION,              can_bms_cycle_time_tsaccellboard5version,              CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSION,              can_bms_cycle_time_tsaccellboard6version,              CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSIONINFO,          can_bms_cycle_time_tsaccellboard1versioninfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSIONINFO,          can_bms_cycle_time_tsaccellboard2versioninfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSIONINFO,          can_bms_cycle_time_tsaccellboard3versioninfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSIONINFO,          can_bms_cycle_time_tsaccellboard4versioninfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSIONINFO,          can_bms_cycle_time_tsaccellboard5versioninfo,          CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSIONINFO,          can_bms_cycle_time_tsaccellboard6versioninfo,          CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSION,        can_bms_cycle_time_tsaccellboard1libcanversion,        CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSION,        can_bms_cycle_time_tsaccellboard2libcanversion,        CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSION,        can_bms_cycle_time_tsaccellboard3libcanversion,        CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSION,        can_bms_cycle_time_tsaccellboard4libcanversion,        CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSION,        can_bms_cycle_time_tsaccellboard5libcanversion,        CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSION,        can_bms_cycle_time_tsaccellboard6libcanversion,        CAN_MAX_CELLBOARD_DROPPED },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard1libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard2libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard3libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard4libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard5libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard6libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED },
};

/*!
 * \brief Information about the messages received on the primary network
 *
 * \details The one-shot messages (e.g. flashing) are not listed since they have no cycle time
 */
EAGLETRT_STATIC const struct CanCommunicationInfoPrimaryMessage primary_messages[] = {
    { CAN_PRIMARY_MESSAGE_FRAME_ID_BMSSET,                    can_primary_cycle_time_bmsset,                    CAN_MAX_PRIMARY_DROPPED },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYTSACBALANCINGSET, can_primary_cycle_time_raspberrytsacbalancingset, CAN_MAX_PRIMARY_DROPPED },
};

/*! \brief Number of messages received on the BMS network */
#define BMS_MESSAGES_COUNT (sizeof(bms_messages) / sizeof(bms_messages[0]))
/*! \brief Number of messages received on the primary network */
#define PRIMARY_MESSAGES_COUNT (sizeof(primary_messages) / sizeof(primary_messages[0]))

/*!
 * \brief CAN communication info handler structure
 *
 * \details Every array is indexed like the table of its network
 *
 *          Each message has two watchdogs:
 *          - the cycle watchdog times out every exact cycle time and is
 *            restarted from its own callback, so it marks the slots in which
 *            a message is expected without accumulating the wiggle room
 *          - the leeway watchdog is started by the cycle watchdog and gives
 *            the message CAN_CYCLE_TIME_WIGGLE_ROOM_MS to arrive, when it
 *            times out the message is counted as dropped
 *
 *          When a message is received the cycle watchdog has to be fed and the
 *          leeway watchdog has to be reset, which also re-syncs the slots to
 *          the sender
 */
struct CanCommunicationInfoHandler {
    struct Watchdog bms_cycle_watchdogs[BMS_MESSAGES_COUNT];  /*!< The watchdogs marking the expected arrival of the messages on the BMS network */
    struct Watchdog bms_leeway_watchdogs[BMS_MESSAGES_COUNT]; /*!< The watchdogs counting the dropped messages on the BMS network */
    uint8_t bms_consecutive_drops[BMS_MESSAGES_COUNT];        /*!< The amount of consecutive messages dropped for a given ID on the BMS network */
    uint16_t bms_cumulative_drops[BMS_MESSAGES_COUNT];        /*!< The cumulative amount of messages dropped for a given ID on the BMS network */
    uint8_t bms_timeframe_drops[2][BMS_MESSAGES_COUNT];       /*!< The amount of messages dropped in a timeframe on the BMS network */

    struct Watchdog primary_cycle_watchdogs[PRIMARY_MESSAGES_COUNT];  /*!< The watchdogs marking the expected arrival of the messages on the primary network */
    struct Watchdog primary_leeway_watchdogs[PRIMARY_MESSAGES_COUNT]; /*!< The watchdogs counting the dropped messages on the primary network */
    uint8_t primary_consecutive_drops[PRIMARY_MESSAGES_COUNT];        /*!< The amount of consecutive messages dropped for a given ID on the primary network */
    uint16_t primary_cumulative_drops[PRIMARY_MESSAGES_COUNT];        /*!< The cumulative amount of messages dropped for a given ID on the primary network */
    uint8_t primary_timeframe_drops[2][PRIMARY_MESSAGES_COUNT];       /*!< The amount of messages dropped in a timeframe on the primary network */

    struct Watchdog timeframe_switcher; /*!< The watchdog that swaps the timeframe being written every CAN_TIMEFRAME */
    uint8_t writing_timeframe_index;    /*!< The index of the timeframe drops currently being written */
};

EAGLETRT_STATIC struct CanCommunicationInfoHandler info_handler;

/*!
 * \brief Handle a message that dropped out on the BMS network
 *
 * \details Called once, when the message reaches its maximum number of
 *          consecutive drops
 *
 * \param[in] index The index of the message in bms_messages
 */
EAGLETRT_STATIC void prv_handle_bms_message_fatal(const size_t index) {
    (void)index;
}

/*!
 * \brief Handle a message that dropped out on the primary network
 *
 * \details Called once, when the message reaches its maximum number of
 *          consecutive drops
 *
 * \param[in] index The index of the message in primary_messages
 */
EAGLETRT_STATIC void prv_handle_primary_message_fatal(const size_t index) {
    (void)index;
}

/*!
 * \brief Open the leeway window of a message and schedule its next slot
 *
 * \param[in] cycle_watchdog The cycle watchdog of the message, which just timed out
 * \param[in] leeway_watchdog The leeway watchdog of the message
 */
EAGLETRT_STATIC void prv_cycle_elapsed(struct Watchdog *const cycle_watchdog, struct Watchdog *const leeway_watchdog) {
    (void)mainboard_watchdogs_api_restart(leeway_watchdog);
    (void)mainboard_watchdogs_api_restart(cycle_watchdog);
}

/*!
 * \brief Count a dropped message
 *
 * \param[in,out] consecutive_drops The consecutive drops of the message
 * \param[in,out] cumulative_drops The cumulative drops of the message
 * \param[in] max_dropped The maximum number of consecutive drops of the message
 *
 * \return true if the message just reached its maximum number of consecutive drops
 */
EAGLETRT_STATIC bool prv_leeway_elapsed(uint8_t *const consecutive_drops,
                                        uint16_t *const cumulative_drops,
                                        uint8_t *const timeframe_drops,
                                        const uint8_t max_dropped) {

    if (*cumulative_drops < UINT16_MAX) {
        ++(*cumulative_drops);
    }
    if (*consecutive_drops < UINT8_MAX) {
        ++(*consecutive_drops);
    }
    if (*timeframe_drops < UINT8_MAX) {
        ++(*timeframe_drops);
    }

    return *consecutive_drops == max_dropped;
}

/*!
 * \brief Called when a message on the BMS network is expected
 *
 * \param[in] context The index of the message in bms_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_bms_cycle_elapsed(void *context) {
    const size_t index = (size_t)(uintptr_t)context;

    prv_cycle_elapsed(&info_handler.bms_cycle_watchdogs[index],
                      &info_handler.bms_leeway_watchdogs[index]);
}

/*!
 * \brief Called when a message on the BMS network did not arrive within the wiggle room
 *
 * \param[in] context The index of the message in bms_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_bms_message_dropped(void *context) {
    const size_t index = (size_t)(uintptr_t)context;

    if (prv_leeway_elapsed(&info_handler.bms_consecutive_drops[index],
                           &info_handler.bms_cumulative_drops[index],
                           &info_handler.bms_timeframe_drops[info_handler.writing_timeframe_index][index],
                           bms_messages[index].max_dropped)) {
        prv_handle_bms_message_fatal(index);
    }
}

/*!
 * \brief Called when a message on the primary network is expected
 *
 * \param[in] context The index of the message in primary_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_primary_cycle_elapsed(void *context) {
    const size_t index = (size_t)(uintptr_t)context;

    prv_cycle_elapsed(&info_handler.primary_cycle_watchdogs[index],
                      &info_handler.primary_leeway_watchdogs[index]);
}

/*!
 * \brief Called when a message on the primary network did not arrive within the wiggle room
 *
 * \param[in] context The index of the message in primary_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_primary_message_dropped(void *context) {
    const size_t index = (size_t)(uintptr_t)context;

    if (prv_leeway_elapsed(&info_handler.primary_consecutive_drops[index],
                           &info_handler.primary_cumulative_drops[index],
                           &info_handler.primary_timeframe_drops[info_handler.writing_timeframe_index][index],
                           primary_messages[index].max_dropped)) {
        prv_handle_primary_message_fatal(index);
    }
}

EAGLETRT_STATIC void prv_handle_timeframe_switch(void *context) {
    (void)context;

    info_handler.writing_timeframe_index = !info_handler.writing_timeframe_index;

    uint8_t index = info_handler.writing_timeframe_index;

    memset(info_handler.bms_timeframe_drops[index], 0, sizeof(info_handler.bms_timeframe_drops[index]));
    memset(info_handler.primary_timeframe_drops[index], 0, sizeof(info_handler.primary_timeframe_drops[index]));
    mainboard_watchdogs_api_restart(&info_handler.timeframe_switcher);
}

/*!
 * \brief Initialize the cycle and leeway watchdogs of a message
 *
 * \details The wiggle room must be smaller than the cycle time, otherwise the
 *          leeway window of a slot would overlap the next slot
 *
 * \param[out] cycle_watchdog The cycle watchdog of the message
 * \param[out] leeway_watchdog The leeway watchdog of the message
 * \param[in] cycle_time The cycle time of the message in ms
 * \param[in] cycle_callback Called when the cycle watchdog times out
 * \param[in] leeway_callback Called when the leeway watchdog times out
 * \param[in] index The index of the message in its table, passed as the context of the callbacks
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if the cycle time is not greater than the wiggle room or a watchdog fails to initialize
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
EAGLETRT_STATIC enum CanCommunicationInfoReturnCode prv_init_message_watchdogs(struct Watchdog *const cycle_watchdog,
                                                                               struct Watchdog *const leeway_watchdog,
                                                                               const int32_t cycle_time,
                                                                               const watchdog_timeout_callback cycle_callback,
                                                                               const watchdog_timeout_callback leeway_callback,
                                                                               const size_t index) {
    if (cycle_time <= (int32_t)CAN_CYCLE_TIME_WIGGLE_ROOM_MS) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    void *const context = (void *)(uintptr_t)index;

    if (mainboard_watchdogs_api_init_watchdog(cycle_watchdog, (milliseconds)cycle_time, cycle_callback, context) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }
    if (mainboard_watchdogs_api_init_watchdog(leeway_watchdog, CAN_CYCLE_TIME_WIGGLE_ROOM_MS, leeway_callback, context) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    return CAN_COMMUNICATION_INFO_RC_OK;
}

enum CanCommunicationInfoReturnCode can_communication_info_init(void) {

    if (mainboard_watchdogs_api_init_watchdog(&info_handler.timeframe_switcher, CAN_TIMEFRAME, prv_handle_timeframe_switch, NULL) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    for (size_t i = 0; i < BMS_MESSAGES_COUNT; i++) {
        if (prv_init_message_watchdogs(&info_handler.bms_cycle_watchdogs[i],
                                       &info_handler.bms_leeway_watchdogs[i],
                                       bms_messages[i].cycle_time,
                                       prv_handle_bms_cycle_elapsed,
                                       prv_handle_bms_message_dropped,
                                       i) != CAN_COMMUNICATION_INFO_RC_OK) {
            return CAN_COMMUNICATION_INFO_RC_ERROR;
        }
    }

    for (size_t i = 0; i < PRIMARY_MESSAGES_COUNT; i++) {
        if (prv_init_message_watchdogs(&info_handler.primary_cycle_watchdogs[i],
                                       &info_handler.primary_leeway_watchdogs[i],
                                       primary_messages[i].cycle_time,
                                       prv_handle_primary_cycle_elapsed,
                                       prv_handle_primary_message_dropped,
                                       i) != CAN_COMMUNICATION_INFO_RC_OK) {
            return CAN_COMMUNICATION_INFO_RC_ERROR;
        }
    }

    if (mainboard_watchdogs_api_start(&info_handler.timeframe_switcher) != WATCHDOG_RC_OK) {
        return CAN_COMMUNICATION_INFO_RC_ERROR;
    }

    return CAN_COMMUNICATION_INFO_RC_OK;
}
