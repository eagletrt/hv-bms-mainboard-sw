/*!
 * \file can-communication-info-api.c
 * \date 2026-11-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Information about the CAN communication
 */

#include "can-communication-info-api.h"

#include <stdint.h>

#include "can-primary.h"
#include "eagletrt.h"
#include "can-bms.h"
#include "mainboard-conf.h"
#include "watchdogs.h"

/*!
 * \brief Information about a message received on the BMS network
 */
struct CanCommunicationInfoBmsMessage {
    enum CanBmsMessageFrameId id;                        /*!< The frame ID of the message */
    int32_t cycle_time;                                  /*!< The cycle time of the message in ms */
    uint8_t max_dropped;                                 /*!< The maximum number of consecutive messages that can be dropped */
    watchdog_timeout_callback message_dropped_timeout;   /*!< Called when a message is dropped */
    watchdog_timeout_callback fatal_dropped_out_timeout; /*!< Called when the message completely drops out */
};

/*!
 * \brief Information about a message received on the primary network
 */
struct CanCommunicationInfoPrimaryMessage {
    enum CanPrimaryMessageFrameId id;                    /*!< The frame ID of the message */
    int32_t cycle_time;                                  /*!< The cycle time of the message in ms */
    uint8_t max_dropped;                                 /*!< The maximum number of consecutive messages that can be dropped */
    watchdog_timeout_callback message_dropped_timeout;   /*!< Called when a message is dropped */
    watchdog_timeout_callback fatal_dropped_out_timeout; /*!< Called when the message completely drops out */
};

/*!
 * \brief Handle a message dropped on the BMS network
 *
 * \param[in] context The index of the message in bms_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_bms_message_dropped(void *context) {
    (void)context;
}

/*!
 * \brief Handle a message that completely dropped out on the BMS network
 *
 * \param[in] context The index of the message in bms_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_bms_message_fatal(void *context) {
    (void)context;
}

/*!
 * \brief Handle a message dropped on the primary network
 *
 * \param[in] context The index of the message in primary_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_primary_message_dropped(void *context) {
    (void)context;
}

/*!
 * \brief Handle a message that completely dropped out on the primary network
 *
 * \param[in] context The index of the message in primary_messages, cast to a pointer
 */
EAGLETRT_STATIC void prv_handle_primary_message_fatal(void *context) {
    (void)context;
}

/*!
 * \brief Information about the messages received on the BMS network
 */
EAGLETRT_STATIC const struct CanCommunicationInfoBmsMessage bms_messages[] = {
    { CAN_BMS_MESSAGE_FRAME_ID_IVTMESSAGERESULTI,                  CAN_IVT_CYCLE_TIME_MS,                                 CAN_MAX_IVT_DROPPED,       prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE,              can_bms_cycle_time_tsaccellboard1voltage,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE,              can_bms_cycle_time_tsaccellboard2voltage,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE,              can_bms_cycle_time_tsaccellboard3voltage,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE,              can_bms_cycle_time_tsaccellboard4voltage,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE,              can_bms_cycle_time_tsaccellboard5voltage,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE,              can_bms_cycle_time_tsaccellboard6voltage,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard1voltageinfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard2voltageinfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard3voltageinfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard4voltageinfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard5voltageinfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGEINFO,          can_bms_cycle_time_tsaccellboard6voltageinfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE,          can_bms_cycle_time_tsaccellboard1temperature,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE,          can_bms_cycle_time_tsaccellboard2temperature,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE,          can_bms_cycle_time_tsaccellboard3temperature,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE,          can_bms_cycle_time_tsaccellboard4temperature,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE,          can_bms_cycle_time_tsaccellboard5temperature,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE,          can_bms_cycle_time_tsaccellboard6temperature,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard1temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard2temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard3temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard4temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard5temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATUREINFO,      can_bms_cycle_time_tsaccellboard6temperatureinfo,      CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard1balancingtemperature, CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard2balancingtemperature, CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard3balancingtemperature, CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard4balancingtemperature, CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard5balancingtemperature, CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCINGTEMPERATURE, can_bms_cycle_time_tsaccellboard6balancingtemperature, CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING,            can_bms_cycle_time_tsaccellboard1balancing,            CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING,            can_bms_cycle_time_tsaccellboard2balancing,            CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING,            can_bms_cycle_time_tsaccellboard3balancing,            CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING,            can_bms_cycle_time_tsaccellboard4balancing,            CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING,            can_bms_cycle_time_tsaccellboard5balancing,            CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING,            can_bms_cycle_time_tsaccellboard6balancing,            CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1FSM,                  can_bms_cycle_time_tsaccellboard1fsm,                  CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2FSM,                  can_bms_cycle_time_tsaccellboard2fsm,                  CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3FSM,                  can_bms_cycle_time_tsaccellboard3fsm,                  CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4FSM,                  can_bms_cycle_time_tsaccellboard4fsm,                  CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5FSM,                  can_bms_cycle_time_tsaccellboard5fsm,                  CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6FSM,                  can_bms_cycle_time_tsaccellboard6fsm,                  CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1ERROR,                can_bms_cycle_time_tsaccellboard1error,                CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2ERROR,                can_bms_cycle_time_tsaccellboard2error,                CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3ERROR,                can_bms_cycle_time_tsaccellboard3error,                CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4ERROR,                can_bms_cycle_time_tsaccellboard4error,                CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5ERROR,                can_bms_cycle_time_tsaccellboard5error,                CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6ERROR,                can_bms_cycle_time_tsaccellboard6error,                CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSION,              can_bms_cycle_time_tsaccellboard1version,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSION,              can_bms_cycle_time_tsaccellboard2version,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSION,              can_bms_cycle_time_tsaccellboard3version,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSION,              can_bms_cycle_time_tsaccellboard4version,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSION,              can_bms_cycle_time_tsaccellboard5version,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSION,              can_bms_cycle_time_tsaccellboard6version,              CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSIONINFO,          can_bms_cycle_time_tsaccellboard1versioninfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSIONINFO,          can_bms_cycle_time_tsaccellboard2versioninfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSIONINFO,          can_bms_cycle_time_tsaccellboard3versioninfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSIONINFO,          can_bms_cycle_time_tsaccellboard4versioninfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSIONINFO,          can_bms_cycle_time_tsaccellboard5versioninfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSIONINFO,          can_bms_cycle_time_tsaccellboard6versioninfo,          CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSION,        can_bms_cycle_time_tsaccellboard1libcanversion,        CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSION,        can_bms_cycle_time_tsaccellboard2libcanversion,        CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSION,        can_bms_cycle_time_tsaccellboard3libcanversion,        CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSION,        can_bms_cycle_time_tsaccellboard4libcanversion,        CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSION,        can_bms_cycle_time_tsaccellboard5libcanversion,        CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSION,        can_bms_cycle_time_tsaccellboard6libcanversion,        CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard1libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard2libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard3libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard4libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard5libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSIONINFO,    can_bms_cycle_time_tsaccellboard6libcanversioninfo,    CAN_MAX_CELLBOARD_DROPPED, prv_handle_bms_message_dropped, prv_handle_bms_message_fatal },
};

/*!
 * \brief Information about the messages received on the primary network
 *
 * \details The one-shot messages (e.g. flashing) are not listed since they have no cycle time
 */
EAGLETRT_STATIC const struct CanCommunicationInfoPrimaryMessage primary_messages[] = {
    { CAN_PRIMARY_MESSAGE_FRAME_ID_BMSSET,                    can_primary_cycle_time_bmsset,                    CAN_MAX_PRIMARY_DROPPED, prv_handle_primary_message_dropped, prv_handle_primary_message_fatal },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYTSACBALANCINGSET, can_primary_cycle_time_raspberrytsacbalancingset, CAN_MAX_PRIMARY_DROPPED, prv_handle_primary_message_dropped, prv_handle_primary_message_fatal },
};

/*! \brief Number of messages received on the BMS network */
#define BMS_MESSAGES_COUNT (sizeof(bms_messages) / sizeof(bms_messages[0]))
/*! \brief Number of messages received on the primary network */
#define PRIMARY_MESSAGES_COUNT (sizeof(primary_messages) / sizeof(primary_messages[0]))

/*!
 * \brief CAN communication info handler structure
 *
 * \details Every array is indexed like the table of its network
 */
struct CanCommunicationInfoHandler {
    struct Watchdog bms_dropped_packages[BMS_MESSAGES_COUNT]; /*!< The watchdogs for dropped message detection on the BMS network */
    struct Watchdog bms_fatal_timeouts[BMS_MESSAGES_COUNT];   /*!< The watchdogs for when the messages completely drop out on the BMS network */
    uint16_t bms_cumulative_drops[BMS_MESSAGES_COUNT];        /*!< The cumulative amount of messages dropped for a given ID on the BMS network */
    uint16_t bms_last_second_drops[2][BMS_MESSAGES_COUNT];    /*!< The amount of messages dropped in a timeframe on the BMS network */

    struct Watchdog primary_dropped_packages[PRIMARY_MESSAGES_COUNT]; /*!< The watchdogs for dropped message detection on the primary network */
    struct Watchdog primary_fatal_timeouts[PRIMARY_MESSAGES_COUNT];   /*!< The watchdogs for when the messages completely drop out on the primary network */
    uint16_t primary_cumulative_drops[PRIMARY_MESSAGES_COUNT];        /*!< The cumulative amount of messages dropped for a given ID on the primary network */
    uint16_t primary_last_second_drops[2][PRIMARY_MESSAGES_COUNT];    /*!< The amount of messages dropped in a timeframe on the primary network */
};
