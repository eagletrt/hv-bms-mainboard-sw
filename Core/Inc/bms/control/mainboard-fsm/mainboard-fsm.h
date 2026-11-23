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
 * \brief Conditions needed to stay in TS_ON (old: FEEDBACK_TS_ON_*)
 */
#define MAINBOARD_FSM_TS_ON_MASK (MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_MASK)
#define MAINBOARD_FSM_TS_ON_EXPECTED (MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_EXPECTED)

#endif // MAINBOARD_FSM_H
