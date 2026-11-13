/*!
 * \file mainboard-status.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the mainboard status
 *
 * \details The mainboard status holds three groups of flags:
 *          - faults: set by the module that detects them and never cleared
 *          - requests: set and cleared by the request-control module
 *          - conditions: the current state of the hardware, kept up to date
 *            by the module that reads it
 *
 * \attention Every flag must be written by one module only, the one named
 *            in its description
 */

#ifndef MAINBOARD_STATUS_H
#define MAINBOARD_STATUS_H

#include <stdbool.h>

#include "mainboard-def.h"

/*!
 * \brief Get the bit of a flag inside the bit flags returned by the getters
 *
 * \details Useful to build masks for \ref mainboard_status_api_check_conditions
 *
 * \param FLAG A fault, request or condition
 */
#define MAINBOARD_STATUS_BIT(FLAG) ((bit_flag32)(1UL << (FLAG)))

/*!
 * \brief Return code for the mainboard status module functions
 */
enum MainboardStatusReturnCode {
    MAINBOARD_STATUS_RC_OK,                /*!< The function executed succesfully */
    MAINBOARD_STATUS_RC_INVALID_FAULT,     /*!< The given fault does not exist */
    MAINBOARD_STATUS_RC_INVALID_REQUEST,   /*!< The given request does not exist */
    MAINBOARD_STATUS_RC_INVALID_CONDITION, /*!< The given condition does not exist */
};

/*!
 * \brief Faults of the mainboard
 *
 * \details A fault is set as soon as the filtered values of the owner module
 *          show it, and it is never cleared
 */
enum MainboardStatusFault {
    /* Startup */
    MAINBOARD_STATUS_FAULT_POST_FAILED, /*!< The Power-On Self Test failed (owner: POST) */

    /* Values out of range */
    MAINBOARD_STATUS_FAULT_CELL_UNDER_VOLTAGE,        /*!< A cell voltage is below the minimum (owner: voltages) */
    MAINBOARD_STATUS_FAULT_CELL_OVER_VOLTAGE,         /*!< A cell voltage is above the maximum (owner: voltages) */
    MAINBOARD_STATUS_FAULT_CELL_UNDER_TEMPERATURE,    /*!< A cell temperature is below the minimum (owner: temperatures) */
    MAINBOARD_STATUS_FAULT_CELL_OVER_TEMPERATURE,     /*!< A cell temperature is above the maximum (owner: temperatures) */
    MAINBOARD_STATUS_FAULT_OVER_CURRENT,              /*!< The pack current is above the maximum (owner: current) */
    MAINBOARD_STATUS_FAULT_OVER_POWER,                /*!< The pack power is above the maximum (owner: current) */
    MAINBOARD_STATUS_FAULT_COOLING_UNDER_TEMPERATURE, /*!< A cooling temperature is below the minimum (owner: internal-temperatures) */
    MAINBOARD_STATUS_FAULT_COOLING_OVER_TEMPERATURE,  /*!< A cooling temperature is above the maximum (owner: internal-temperatures) */

    /* Hardware */
    MAINBOARD_STATUS_FAULT_CELLBOARD_FAULT,                /*!< A cellboard reported an error or went in its fatal state (owner: PHC) */
    MAINBOARD_STATUS_FAULT_IMD_FAULT,                      /*!< The IMD detected an insulation fault (owner: sensors) */
    MAINBOARD_STATUS_FAULT_VOLTAGE_INDICATOR_DISCONNECTED, /*!< The TS voltage indicator is not connected (owner: sensors) */

    /* Freshness: the data was not updated in time */
    MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES,         /*!< The cell voltages from the cellboards are too old (owner: can-router watchdogs) */
    MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES,     /*!< The cell temperatures from the cellboards are too old (owner: can-router watchdogs) */
    MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS,      /*!< The status of a cellboard is too old (owner: can-router watchdogs) */
    MAINBOARD_STATUS_FAULT_STALE_CURRENT,               /*!< The current sensor readings are too old (owner: can-router watchdogs) */
    MAINBOARD_STATUS_FAULT_STALE_INTERNAL_VOLTAGES,     /*!< The TS and pack voltage readings are too old (owner: can-router watchdogs) */
    MAINBOARD_STATUS_FAULT_STALE_INTERNAL_TEMPERATURES, /*!< The cooling temperature readings are too old (owner: can-router watchdogs) */
    MAINBOARD_STATUS_FAULT_STALE_SENSORS,               /*!< The shutdown, IMD and relay readings are too old (owner: can-router watchdogs) */

    MAINBOARD_STATUS_FAULT_COUNT
};

/*!
 * \brief Requests received from outside the mainboard
 *
 * \details Every request is set and cleared by the request-control module only,
 *          after it has validated the message and its parameters
 */
enum MainboardStatusRequest {
    MAINBOARD_STATUS_REQUEST_TS_ON,           /*!< The ECU asked to turn on the Tractive System */
    MAINBOARD_STATUS_REQUEST_TS_OFF,          /*!< The ECU asked to turn off the Tractive System, or its messages timed out */
    MAINBOARD_STATUS_REQUEST_ECU_OK,          /*!< The ECU acknowledged the BMS during the TS-ON procedure */
    MAINBOARD_STATUS_REQUEST_FLASH,           /*!< A flash procedure was requested */
    MAINBOARD_STATUS_REQUEST_BALANCING_START, /*!< The start of the cell balancing was requested */
    MAINBOARD_STATUS_REQUEST_BALANCING_STOP,  /*!< The stop of the cell balancing was requested */

    MAINBOARD_STATUS_REQUEST_COUNT
};

/*!
 * \brief Conditions of the mainboard hardware
 *
 * \details Every condition is true when the fact in its name is true. The
 *          name of the old feedback is given for reference. "Inverted"
 *          means the condition is the opposite of the old feedback value
 *
 * \warning The polarity of the old feedbacks was inferred from the masks used
 *          by the old FSM, double check it against the schematic
 */
enum MainboardStatusCondition {
    /* Relays (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED,      /*!< The AIR- is commanded closed (old: AIRN_OPEN_COM, inverted) (owner: pcu) */
    MAINBOARD_STATUS_CONDITION_AIRN_CLOSED,               /*!< The AIR- is mechanically closed (old: AIRN_OPEN_MEC, inverted) (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED, /*!< The precharge relay is commanded closed (old: PRECHARGE_OPEN_COM, inverted) (owner: pcu) */
    MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED,          /*!< The precharge relay is mechanically closed (old: PRECHARGE_OPEN_MEC, inverted) (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED,      /*!< The AIR+ is commanded closed (old: AIRP_OPEN_COM, inverted) (owner: pcu) */
    MAINBOARD_STATUS_CONDITION_AIRP_CLOSED,               /*!< The AIR+ is mechanically closed (old: AIRP_OPEN_MEC, inverted) (owner: sensors) */

    /* Tractive System */
    MAINBOARD_STATUS_CONDITION_TS_BELOW_60V,                /*!< The TS voltage is below 60V (old: TS_LESS_THAN_60V) (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_TSAL_GREEN,                  /*!< The TSAL shows green (old: TSAL_GREEN) (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_VOLTAGE_INDICATOR_CONNECTED, /*!< The TS voltage indicator is connected (old: INDICATOR_CONNECTED) (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_PRECHARGE_COMPLETE,          /*!< The TS voltage reached the precharge threshold (owner: internal-voltages) */

    /* Hardware plausibility check (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE,           /*!< The plausibility check passes, raw value (old: PLAUSIBLE_STATE) */
    MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_FILTERED,  /*!< The plausibility check passes after the RC filter (old: PLAUSIBLE_STATE_RC) */
    MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_PERSISTED, /*!< The plausibility check passes, persisted value, prefer this one (old: PLAUSIBLE_STATE_PERSISTED) */

    /* Hardware fault latches (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_AMS_FAULT_LATCHED,          /*!< The AMS fault latch tripped (old: BMS_FAULT_LATCHED, inverted) */
    MAINBOARD_STATUS_CONDITION_IMD_FAULT_LATCHED,          /*!< The IMD fault latch tripped (old: IMD_FAULT_LATCHED, inverted) */
    MAINBOARD_STATUS_CONDITION_PLAUSIBILITY_FAULT_LATCHED, /*!< The plausibility fault latch tripped (old: PLAUSIBLE_STATE_LATCHED, inverted) */
    MAINBOARD_STATUS_CONDITION_EXTERNAL_FAULT_LATCHED,     /*!< The external fault latch tripped (old: EXT_FAULT_LATCHED, inverted) */
    MAINBOARD_STATUS_CONDITION_LATCH_RESET_ACTIVE,         /*!< The latch reset signal is active (old: LATCH_RESET) */

    /* Cockpit LEDs (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_AMS_COCKPIT_LED_ON, /*!< The AMS LED in the cockpit is on (old: BMS_FAULT_COCKPIT_LED, inverted) */
    MAINBOARD_STATUS_CONDITION_IMD_COCKPIT_LED_ON, /*!< The IMD LED in the cockpit is on (old: IMD_FAULT_COCKPIT_LED, inverted) */

    /* IMD (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_IMD_OK, /*!< The IMD reports no insulation fault (old: IMD_OK) */

    /* Shutdown circuit, closed means there is voltage at that point (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_SD_IN_CLOSED,       /*!< The shutdown circuit is closed up to the BMS input (old: SD_IN) */
    MAINBOARD_STATUS_CONDITION_SD_AMS_NODE_CLOSED, /*!< The shutdown circuit is closed after the AMS node (old: SD_BMS_FB) */
    MAINBOARD_STATUS_CONDITION_SD_IMD_NODE_CLOSED, /*!< The shutdown circuit is closed after the IMD node (old: SD_IMD_FB) */
    MAINBOARD_STATUS_CONDITION_SD_OUT_CLOSED,      /*!< The shutdown circuit is closed up to the BMS output (old: SD_OUT) */
    MAINBOARD_STATUS_CONDITION_SD_END_CLOSED,      /*!< The shutdown circuit is closed up to the AIRs (old: SD_END) */

    /* Power supplies (owner: sensors) */
    MAINBOARD_STATUS_CONDITION_MCU_5V_OK,          /*!< The 5V supply of the MCU is present (old: V5_MCU) */
    MAINBOARD_STATUS_CONDITION_PROBING_3V3_OK,     /*!< The 3V3 supply is present (old: PROBING_3V3) */
    MAINBOARD_STATUS_CONDITION_HANDCART_CONNECTED, /*!< The mainboard is connected to the handcart (old: HC connected callback) */

    MAINBOARD_STATUS_CONDITION_COUNT
};

static_assert(MAINBOARD_STATUS_FAULT_COUNT <= 32U, "The faults must fit in a bit_flag32");
static_assert(MAINBOARD_STATUS_REQUEST_COUNT <= 32U, "The requests must fit in a bit_flag32");
static_assert(MAINBOARD_STATUS_CONDITION_COUNT <= 32U, "The conditions must fit in a bit_flag32");

/*!
 * \brief Internal handler of the mainboard status
 *
 * \details Each flag is a separate bool so that a single flag can be written
 *          from an interrupt without a read-modify-write on the others
 *
 * \warning This structure should never be used outside of this module
 */
struct MainboardStatusHandler {
    volatile bool faults[MAINBOARD_STATUS_FAULT_COUNT];         /*!< The faults, never cleared once set */
    volatile bool requests[MAINBOARD_STATUS_REQUEST_COUNT];     /*!< The pending requests */
    volatile bool conditions[MAINBOARD_STATUS_CONDITION_COUNT]; /*!< The current hardware conditions */
};

#endif // MAINBOARD_STATUS_H
