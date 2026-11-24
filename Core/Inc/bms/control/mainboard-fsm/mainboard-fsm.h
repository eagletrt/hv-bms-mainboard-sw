/*!
 * \file mainboard-fsm.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the mainboard FSM
 */

#ifndef MAINBOARD_FSM_H
#define MAINBOARD_FSM_H

#include "mainboard-status.h"
#include "eagletrt-api.h"

/*!
 * \brief Watchdogs of the FSM, each one bounds the time spent in one state
 *
 * \details At most one of them runs at a time: it is restarted on entry in its
 *          state and every other one is reset
 */
enum MainboardFsmWatchdog {
    MAINBOARD_FSM_WATCHDOG_WAIT_OK,         /*!< Timeout of WAIT_OK, goes to DISCHARGE_TS */
    MAINBOARD_FSM_WATCHDOG_AIRN_CHECK,      /*!< Timeout of AIRN_CHECK, goes to DISCHARGE_TS */
    MAINBOARD_FSM_WATCHDOG_PRECHARGE_CHECK, /*!< Timeout of PRECHARGE_CHECK, goes to DISCHARGE_TS */
    MAINBOARD_FSM_WATCHDOG_AIRP_CHECK,      /*!< Timeout of AIRP_CHECK, goes to DISCHARGE_TS */
    MAINBOARD_FSM_WATCHDOG_DISCHARGE_TS,    /*!< Timeout of DISCHARGE_TS, goes to FATAL */

    MAINBOARD_FSM_WATCHDOG_COUNT
};

enum MainboardFsmStateID {
    MAINBOARD_FSM_STATE_INIT,
    MAINBOARD_FSM_STATE_IDLE,

    // TS-ON procedure
    MAINBOARD_FSM_STATE_WAIT_OK,
    MAINBOARD_FSM_STATE_AIRN_CHECK,
    MAINBOARD_FSM_STATE_PRECHARGE_CHECK,
    MAINBOARD_FSM_STATE_AIRP_CHECK,
    MAINBOARD_FSM_STATE_TS_ON,
    MAINBOARD_FSM_STATE_DISCHARGE_TS,

    // BALANCING
    MAINBOARD_FSM_STATE_BALANCING_READ,
    MAINBOARD_FSM_STATE_BALANCING_DISCHARGE,

    // FLASHING
    MAINBOARD_FSM_STATE_FLASH,

    // FATAL
    MAINBOARD_FSM_STATE_FATAL,

    MAINBOARD_FSM_STATE_COUNT
};

/*!
 * \brief Steps of the TS on procedure that check the mainboard conditions
 */
enum MainboardFsmStep {
    MAINBOARD_FSM_STEP_WAIT_OK,
    MAINBOARD_FSM_STEP_AIRN_CHECK,
    MAINBOARD_FSM_STEP_PRECHARGE_CHECK,
    MAINBOARD_FSM_STEP_AIRP_CHECK,
    MAINBOARD_FSM_STEP_TS_ON,
    MAINBOARD_FSM_STEP_DISCHARGE_TS,

    MAINBOARD_FSM_STEP_COUNT
};

/*!
 * \brief Conditions checked by a step of the TS on procedure
 *
 * \details The conditions are checked with \ref mainboard_status_api_check_conditions
 */
struct MainboardFsmStepCheck {
    bit_flag32 mask;               /*!< The conditions to check */
    bit_flag32 expected;           /*!< The expected value of each condition in the mask */
    enum MainboardFsmStateID next; /*!< The state to go to when every condition matches, for TS_ON the conditions must keep matching */
};

/*!
 * \brief Conditions checked in every step of the TS on procedure
 *
 * \details Shared part of the old FEEDBACK_*_HIGH masks: plausibility, voltage indicator,
 * IMD, shutdown end, MCU 5V, IMD cockpit LED and the three hardware fault latches
 *
 * \details Used with \ref mainboard_status_api_check_conditions, every bit in the mask is
 * checked and must have the value it has in the expected macro
 */
#define MAINBOARD_FSM_COMMON_MASK                                                      \
    (                                                                                  \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_PERSISTED) |   \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_VOLTAGE_INDICATOR_CONNECTED) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_IMD_OK) |                      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_SD_END_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_MCU_5V_OK) |                   \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_IMD_COCKPIT_LED_ON) |          \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AMS_FAULT_LATCHED) |           \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_IMD_FAULT_LATCHED) |           \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PLAUSIBILITY_FAULT_LATCHED))
#define MAINBOARD_FSM_COMMON_EXPECTED                                                  \
    (                                                                                  \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_PERSISTED) |   \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_VOLTAGE_INDICATOR_CONNECTED) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_IMD_OK) |                      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_SD_END_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_MCU_5V_OK))

/*!
 * \brief Relay commands and positions, TS voltage and TSAL, checked in every step of
 * the TS on procedure with a different expected value
 *
 * \details The old *_OPEN_COM bits were high when the relay was commanded closed,
 * so they map to *_CLOSE_COMMANDED with the same value. The old *_OPEN_MEC bits map
 * to *_CLOSED with the opposite value
 */
#define MAINBOARD_FSM_RELAYS_MASK                                                    \
    (                                                                                \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED) |      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED) |          \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED) |      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRP_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_TS_BELOW_60V) |              \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_TSAL_GREEN))

/*!
 * \brief Conditions needed to go from WAIT_OK to AIRN_CHECK (old: FEEDBACK_IDLE_TO_AIRN_CHECK_*)
 *
 * \details Every relay open, TS below 60V and TSAL green. The AMS cockpit LED is not checked
 */
#define MAINBOARD_FSM_WAIT_OK_TO_AIRN_CHECK_MASK ((MAINBOARD_FSM_COMMON_MASK) | (MAINBOARD_FSM_RELAYS_MASK))
#define MAINBOARD_FSM_WAIT_OK_TO_AIRN_CHECK_EXPECTED                    \
    (                                                                   \
        (MAINBOARD_FSM_COMMON_EXPECTED) |                               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_TS_BELOW_60V) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_TSAL_GREEN))

/*!
 * \brief Conditions needed to go from AIRN_CHECK to PRECHARGE_CHECK (old: FEEDBACK_AIRN_CHECK_TO_PRECHARGE_*)
 *
 * \details Only the AIR- commanded and mechanically closed, TS below 60V, TSAL not green, AMS cockpit LED off
 */
#define MAINBOARD_FSM_AIRN_CHECK_TO_PRECHARGE_CHECK_MASK \
    (                                                    \
        (MAINBOARD_FSM_COMMON_MASK) |                    \
        (MAINBOARD_FSM_RELAYS_MASK) |                    \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AMS_COCKPIT_LED_ON))
#define MAINBOARD_FSM_AIRN_CHECK_TO_PRECHARGE_CHECK_EXPECTED                    \
    (                                                                           \
        (MAINBOARD_FSM_COMMON_EXPECTED) |                                       \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSED) |          \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_TS_BELOW_60V))

/*!
 * \brief Conditions needed to go from PRECHARGE_CHECK to AIRP_CHECK (old: FEEDBACK_PRECHARGE_TO_AIRP_CHECK_*
 * and pcu_api_is_precharge_complete)
 *
 * \details AIR- and precharge commanded and mechanically closed, AIR+ open, TS above 60V,
 * TSAL not green, AMS cockpit LED off and precharge complete
 */
#define MAINBOARD_FSM_PRECHARGE_CHECK_TO_AIRP_CHECK_MASK                      \
    (                                                                         \
        (MAINBOARD_FSM_COMMON_MASK) |                                         \
        (MAINBOARD_FSM_RELAYS_MASK) |                                         \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AMS_COCKPIT_LED_ON) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_COMPLETE))
#define MAINBOARD_FSM_PRECHARGE_CHECK_TO_AIRP_CHECK_EXPECTED                         \
    (                                                                                \
        (MAINBOARD_FSM_COMMON_EXPECTED) |                                            \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED) |      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED) |          \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_COMPLETE))

/*!
 * \brief Conditions needed to go from AIRP_CHECK to TS_ON (old: FEEDBACK_AIRP_CHECK_TO_TS_ON_*)
 *
 * \details Every relay commanded and mechanically closed, TS above 60V, TSAL not green, AMS cockpit LED off
 */
#define MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_MASK \
    (                                          \
        (MAINBOARD_FSM_COMMON_MASK) |          \
        (MAINBOARD_FSM_RELAYS_MASK) |          \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AMS_COCKPIT_LED_ON))
#define MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_EXPECTED                                   \
    (                                                                                \
        (MAINBOARD_FSM_COMMON_EXPECTED) |                                            \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED) |      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED) |          \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED) |      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRP_CLOSED))

/*!
 * \brief Conditions needed to stay in TS_ON
 */
#define MAINBOARD_FSM_TS_ON_MASK (MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_MASK)
#define MAINBOARD_FSM_TS_ON_EXPECTED (MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_EXPECTED)

/*!
 * \brief Conditions needed to go from DISCHARGE_TS to IDLE
 *
 * \details Every relay commanded open and mechanically open and TS below 60V.
 * The TSAL and the common conditions are not checked, any fault is handled before this check
 */
#define MAINBOARD_FSM_DISCHARGE_TS_TO_IDLE_MASK                                      \
    (                                                                                \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED) |      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED) | \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED) |          \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED) |      \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRP_CLOSED) |               \
        MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_TS_BELOW_60V))
#define MAINBOARD_FSM_DISCHARGE_TS_TO_IDLE_EXPECTED (MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_TS_BELOW_60V))

#endif // MAINBOARD_FSM_H
