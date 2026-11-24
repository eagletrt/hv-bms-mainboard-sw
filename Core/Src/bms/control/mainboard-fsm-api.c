/*!
 * \file mainboard-fsm-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Main mainboard FSM that handles all the operations of the BMS
 */

#include "mainboard-fsm-api.h"
#include "bal-control-api.h"
#include "eagletrt-fsm-api.h"
#include "eagletrt-fsm.h"
#include <string.h>
#include "mainboard-fsm.h"
#include "mainboard-watchdogs-api.h"
#include "pcu-api.h"
#include "pcu.h"
#include "watchdogs.h"
#include "mainboard-conf.h"

#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-status-api.h"
#include "mainboard-status.h"

EAGLETRT_STATIC struct FSMHandler mainboard_fsm_handler;

/*! \brief Timeout of each FSM watchdog, indexed by \ref MainboardFsmWatchdog */
EAGLETRT_STATIC const milliseconds mainboard_fsm_watchdog_timeouts[MAINBOARD_FSM_WATCHDOG_COUNT] = {
    [MAINBOARD_FSM_WATCHDOG_WAIT_OK] = MAINBOARD_FSM_WAIT_OK_TIMEOUT_MS,
    [MAINBOARD_FSM_WATCHDOG_AIRN_CHECK] = MAINBOARD_FSM_AIRN_TIMEOUT_MS,
    [MAINBOARD_FSM_WATCHDOG_PRECHARGE_CHECK] = MAINBOARD_FSM_PRECHARGE_TIMEOUT_MS,
    [MAINBOARD_FSM_WATCHDOG_AIRP_CHECK] = MAINBOARD_FSM_AIRP_TIMEOUT_MS,
    [MAINBOARD_FSM_WATCHDOG_DISCHARGE_TS] = MAINBOARD_FSM_DISCHARGE_TIMEOUT_MS,
    [MAINBOARD_FSM_WATCHDOG_BALANCING_READ] = MAINBOARD_FSM_BALANCING_READ_TIMEOUT_MS,
    [MAINBOARD_FSM_WATCHDOG_BALANCING_DISCHARGE] = MAINBOARD_FSM_BALANCING_DISCHARGE_TIMEOUT_MS,
};

/*! \brief The FSM watchdogs, indexed by \ref MainboardFsmWatchdog */
EAGLETRT_STATIC struct Watchdog mainboard_fsm_watchdogs[MAINBOARD_FSM_WATCHDOG_COUNT];

/*! \brief Checks of the TS on procedure, indexed by \ref MainboardFsmStep */
EAGLETRT_STATIC const struct MainboardFsmStepCheck mainboard_fsm_steps[MAINBOARD_FSM_STEP_COUNT] = {
    [MAINBOARD_FSM_STEP_WAIT_OK] = {
                                    .mask = MAINBOARD_FSM_WAIT_OK_TO_AIRN_CHECK_MASK,
                                    .expected = MAINBOARD_FSM_WAIT_OK_TO_AIRN_CHECK_EXPECTED,
                                    .next = MAINBOARD_FSM_STATE_AIRN_CHECK,
                                    },
    [MAINBOARD_FSM_STEP_AIRN_CHECK] = {
                                    .mask = MAINBOARD_FSM_AIRN_CHECK_TO_PRECHARGE_CHECK_MASK,
                                    .expected = MAINBOARD_FSM_AIRN_CHECK_TO_PRECHARGE_CHECK_EXPECTED,
                                    .next = MAINBOARD_FSM_STATE_PRECHARGE_CHECK,
                                    },
    [MAINBOARD_FSM_STEP_PRECHARGE_CHECK] = {
                                    .mask = MAINBOARD_FSM_PRECHARGE_CHECK_TO_AIRP_CHECK_MASK,
                                    .expected = MAINBOARD_FSM_PRECHARGE_CHECK_TO_AIRP_CHECK_EXPECTED,
                                    .next = MAINBOARD_FSM_STATE_AIRP_CHECK,
                                    },
    [MAINBOARD_FSM_STEP_AIRP_CHECK] = {
                                    .mask = MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_MASK,
                                    .expected = MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_EXPECTED,
                                    .next = MAINBOARD_FSM_STATE_TS_ON,
                                    },
    [MAINBOARD_FSM_STEP_TS_ON] = {
                                    .mask = MAINBOARD_FSM_TS_ON_MASK,
                                    .expected = MAINBOARD_FSM_TS_ON_EXPECTED,
                                    .next = MAINBOARD_FSM_STATE_TS_ON,
                                    },
    [MAINBOARD_FSM_STEP_DISCHARGE_TS] = {
                                    .mask = MAINBOARD_FSM_DISCHARGE_TS_TO_IDLE_MASK,
                                    .expected = MAINBOARD_FSM_DISCHARGE_TS_TO_IDLE_EXPECTED,
                                    .next = MAINBOARD_FSM_STATE_IDLE,
                                    },
};

/*!
 * \brief Transition the mainboard FSM to a new state
 * \returns nothing, as this function is only called within states if the transition fails then
 * there are way bigger problems to worry about
 */
EAGLETRT_STATIC void prv_mainboard_fsm_transition_to(enum MainboardFsmStateID state) {
    fsm_api_transition_to(&mainboard_fsm_handler, state);
}

/*!
 * \brief Checks shared by every state of the TS on procedure, run before the step checks
 *
 * \details Any fault goes to FATAL, a TS off request (or an ECU timeout) goes to DISCHARGE_TS
 *
 * \returns true if a transition was requested and the state must not run other checks, false otherwise
 */
EAGLETRT_STATIC bool prv_mainboard_fsm_check_ts_abort(void) {
    if (mainboard_status_api_has_faults()) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return true;
    }
    if (mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_TS_OFF)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_DISCHARGE_TS);
        return true;
    }
    return false;
}

/*!
 * \brief Check the conditions of a step and go to its next state when every condition matches
 *
 * \param step The step to check
 *
 * \returns true if every condition of the step matches, false otherwise
 */
EAGLETRT_STATIC bool prv_mainboard_fsm_check_step(const enum MainboardFsmStep step) {
    const struct MainboardFsmStepCheck *const check = &mainboard_fsm_steps[step];

    // TODO: keep the mismatches to report the conditions that blocked the step
    if (!mainboard_status_api_check_conditions(check->mask, check->expected, NULL)) {
        return false;
    }
    prv_mainboard_fsm_transition_to(check->next);
    return true;
}

/*!
 * \brief Open the TS if the shutdown circuit is open, used once a relay may be commanded closed
 *
 * \returns true if a transition was requested and the state must not run other checks, false otherwise
 */
EAGLETRT_STATIC bool prv_mainboard_fsm_check_sd_end(void) {
    if (mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_SD_END_CLOSED)) {
        return false;
    }
    prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_DISCHARGE_TS);
    return true;
}

/*!
 * \brief Timeout callback of every FSM watchdog
 *
 * \details Does nothing: the states poll \ref prv_mainboard_fsm_is_timed_out,
 *          the watchdog stays timed out until it is reset or restarted
 */
EAGLETRT_STATIC void prv_mainboard_fsm_watchdog_timeout(void *context) {
    EAGLETRT_API_UNUSED(context);
}

/*!
 * \brief Check if a FSM watchdog has timed out
 *
 * \param watchdog The watchdog to check
 *
 * \returns true if the watchdog has timed out, false otherwise
 */
EAGLETRT_STATIC bool prv_mainboard_fsm_is_timed_out(const enum MainboardFsmWatchdog watchdog) {
    return mainboard_watchdogs_api_is_timed_out(&mainboard_fsm_watchdogs[watchdog]);
}

/*!
 * \brief Reset every FSM watchdog, so none of them keeps a slot of the pool
 *
 * \details Used as transition callback when entering a state without a timeout
 *
 * \param data Unused
 */
EAGLETRT_STATIC void prv_mainboard_fsm_reset_watchdogs(void *data) {
    EAGLETRT_API_UNUSED(data);

    // A failure latches the watchdogs fault in the wrapper
    for (size_t i = 0U; i < MAINBOARD_FSM_WATCHDOG_COUNT; ++i) {
        EAGLETRT_API_UNUSED(mainboard_watchdogs_api_reset(&mainboard_fsm_watchdogs[i]));
    }
}

/*!
 * \brief Reset every FSM watchdog and start the given one from now
 *
 * \param watchdog The watchdog to start
 */
EAGLETRT_STATIC void prv_mainboard_fsm_start_watchdog(const enum MainboardFsmWatchdog watchdog) {
    prv_mainboard_fsm_reset_watchdogs(NULL);
    EAGLETRT_API_UNUSED(mainboard_watchdogs_api_restart(&mainboard_fsm_watchdogs[watchdog]));
}

/*! \brief Transition callback, starts the WAIT_OK watchdog */
EAGLETRT_STATIC void prv_mainboard_fsm_enter_wait_ok(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_mainboard_fsm_start_watchdog(MAINBOARD_FSM_WATCHDOG_WAIT_OK);
}

/*! \brief Transition callback, starts the AIRN_CHECK watchdog */
EAGLETRT_STATIC void prv_mainboard_fsm_enter_airn_check(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_mainboard_fsm_start_watchdog(MAINBOARD_FSM_WATCHDOG_AIRN_CHECK);
}

/*! \brief Transition callback, starts the PRECHARGE_CHECK watchdog */
EAGLETRT_STATIC void prv_mainboard_fsm_enter_precharge_check(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_mainboard_fsm_start_watchdog(MAINBOARD_FSM_WATCHDOG_PRECHARGE_CHECK);
}

/*! \brief Transition callback, starts the AIRP_CHECK watchdog */
EAGLETRT_STATIC void prv_mainboard_fsm_enter_airp_check(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_mainboard_fsm_start_watchdog(MAINBOARD_FSM_WATCHDOG_AIRP_CHECK);
}

/*! \brief Transition callback, starts the DISCHARGE_TS watchdog */
EAGLETRT_STATIC void prv_mainboard_fsm_enter_discharge_ts(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_mainboard_fsm_start_watchdog(MAINBOARD_FSM_WATCHDOG_DISCHARGE_TS);
}

/*! \brief Transition callback, starts the BALANCING_READ watchdog */
EAGLETRT_STATIC void prv_mainboard_fsm_enter_balancing_read(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_mainboard_fsm_start_watchdog(MAINBOARD_FSM_WATCHDOG_BALANCING_READ);
}

/*! \brief Transition callback, starts the BALANCING_DISCHARGE watchdog */
EAGLETRT_STATIC void prv_mainboard_fsm_enter_balancing_discharge(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_mainboard_fsm_start_watchdog(MAINBOARD_FSM_WATCHDOG_BALANCING_DISCHARGE);
}

struct Transition init_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,  NULL },
    { MAINBOARD_FSM_STATE_FATAL, NULL },
};

struct Transition idle_transitions[] = {
    { MAINBOARD_FSM_STATE_WAIT_OK,        prv_mainboard_fsm_enter_wait_ok        },
    { MAINBOARD_FSM_STATE_BALANCING_READ, prv_mainboard_fsm_enter_balancing_read },
    { MAINBOARD_FSM_STATE_FLASH,          NULL                                   },
    { MAINBOARD_FSM_STATE_FATAL,          NULL                                   },
};

struct Transition wait_ok_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, prv_mainboard_fsm_enter_discharge_ts },
    { MAINBOARD_FSM_STATE_FATAL,        NULL                                 },
    { MAINBOARD_FSM_STATE_AIRN_CHECK,   prv_mainboard_fsm_enter_airn_check   },
};

struct Transition airn_check_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS,    prv_mainboard_fsm_enter_discharge_ts    },
    { MAINBOARD_FSM_STATE_FATAL,           NULL                                    },
    { MAINBOARD_FSM_STATE_PRECHARGE_CHECK, prv_mainboard_fsm_enter_precharge_check },
};

struct Transition precharge_check_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, prv_mainboard_fsm_enter_discharge_ts },
    { MAINBOARD_FSM_STATE_FATAL,        NULL                                 },
    { MAINBOARD_FSM_STATE_AIRP_CHECK,   prv_mainboard_fsm_enter_airp_check   }
};

struct Transition airp_check_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, prv_mainboard_fsm_enter_discharge_ts },
    { MAINBOARD_FSM_STATE_FATAL,        NULL                                 },
    { MAINBOARD_FSM_STATE_TS_ON,        prv_mainboard_fsm_reset_watchdogs    }
};

struct Transition ts_on_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, prv_mainboard_fsm_enter_discharge_ts },
    { MAINBOARD_FSM_STATE_FATAL,        NULL                                 },
};

struct Transition discharge_ts_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,  prv_mainboard_fsm_reset_watchdogs },
    { MAINBOARD_FSM_STATE_FATAL, NULL                              },
};

struct Transition balancing_read_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,                prv_mainboard_fsm_reset_watchdogs           },
    { MAINBOARD_FSM_STATE_BALANCING_DISCHARGE, prv_mainboard_fsm_enter_balancing_discharge },
    { MAINBOARD_FSM_STATE_FATAL,               NULL                                        },
};

struct Transition balancing_discharge_transitions[] = {
    { MAINBOARD_FSM_STATE_BALANCING_READ, prv_mainboard_fsm_enter_balancing_read },
    { MAINBOARD_FSM_STATE_FATAL,          NULL                                   },
};

struct Transition flash_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,  NULL },
    { MAINBOARD_FSM_STATE_FATAL, NULL },
};

/*!
 * \brief Initialize all of the modules
 */
EAGLETRT_STATIC void mainboard_fsm_state_init(void *data) {
    EAGLETRT_API_UNUSED(data);

    // A failure latches the watchdogs fault in the wrapper, caught by the safety check below
    for (size_t i = 0U; i < MAINBOARD_FSM_WATCHDOG_COUNT; ++i) {
        EAGLETRT_API_UNUSED(mainboard_watchdogs_api_init_watchdog(&mainboard_fsm_watchdogs[i],
                                                                  mainboard_fsm_watchdog_timeouts[i],
                                                                  prv_mainboard_fsm_watchdog_timeout,
                                                                  NULL));
    }

    // TODO: run the POST, it sets MAINBOARD_STATUS_FAULT_POST_FAILED when it fails

    if (mainboard_status_api_has_faults()) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return;
    }
    prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_IDLE);
}

/*!
 * \brief Idle state of the mainboard FSM, does nothing but wait for requests
 */
EAGLETRT_STATIC void mainboard_fsm_state_idle(void *data) {
    EAGLETRT_API_UNUSED(data);

    // Actively do nothing (:

    // Transition logic:

    // If the FSM is unsafe in a safe state (IDLE) then transition to FATAL
    if (mainboard_status_api_has_faults()) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return;
    }
    // If a TS-ON request has been called then start the TS-ON procedure
    if (mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_TS_ON)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_WAIT_OK);
        return;
    }
    // If a FLASHING message has been heard then transition to FLASHING state
    if (mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_FLASH)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FLASH);
        return;
    }
    // If a BALANCING message has been heard then transition to BALANCING state if pack is not balanced
    if (mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_BALANCING)) {
        if (!mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_PACK_BALANCED)) {
            prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_BALANCING_READ);
        }
    }
}

/*!
 * \brief Double door mechanism for ts-on procedure, waits for the ECU to send a keep ts-on message after the ts-on request so that
 * internal ts-off requests don't trigger a loop
 */
EAGLETRT_STATIC void mainboard_fsm_state_wait_ok(void *data) {
    EAGLETRT_API_UNUSED(data);

    // Wait for the keep ts-on message to continue the ts-on procedure

    // Transition logic:

    // If there are faults go to FAULT or if ts-off is requested go to discharge
    if (prv_mainboard_fsm_check_ts_abort()) {
        return;
    }

    // If there has been no keep ts-on request then go back to idle
    if (prv_mainboard_fsm_is_timed_out(MAINBOARD_FSM_WATCHDOG_WAIT_OK)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_DISCHARGE_TS);
        return;
    }

    // If the request changed to keep ts-on then continue the procedure
    if (mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_KEEP_TS_ON)) {
        prv_mainboard_fsm_check_step(MAINBOARD_FSM_STEP_WAIT_OK);
    }
}

/*!
 * \brief Check if the air- relay closed
 */
EAGLETRT_STATIC void mainboard_fsm_state_airn_check(void *data) {
    EAGLETRT_API_UNUSED(data);

    pcu_api_close(PCU_RELAY_AIR_NEGATIVE);

    // Transition logic:

    // If there are faults go to FAULT or if ts-off is requested go to discharge
    if (prv_mainboard_fsm_check_ts_abort()) {
        return;
    }
    // Open the TS if the shutdown circuit is open
    if (prv_mainboard_fsm_check_sd_end()) {
        return;
    }
    // If the relay doesn't close discharge
    if (prv_mainboard_fsm_is_timed_out(MAINBOARD_FSM_WATCHDOG_AIRN_CHECK)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_DISCHARGE_TS);
        return;
    }

    // If the relay is confirmed closed go to precharge
    EAGLETRT_API_UNUSED(prv_mainboard_fsm_check_step(MAINBOARD_FSM_STEP_AIRN_CHECK));
}

/*!
 * \brief Check if the precharge is complete
 */
EAGLETRT_STATIC void mainboard_fsm_state_precharge_check(void *data) {
    EAGLETRT_API_UNUSED(data);

    pcu_api_close(PCU_RELAY_PRECHARGE);

    // Transition logic:

    // If there are faults go to FAULT or if ts-off is requested go to discharge
    if (prv_mainboard_fsm_check_ts_abort()) {
        return;
    }
    // Open the TS if the shutdown circuit is open
    if (prv_mainboard_fsm_check_sd_end()) {
        return;
    }
    // If the relay doesn't close or the precharge takes too long then discharge
    if (prv_mainboard_fsm_is_timed_out(MAINBOARD_FSM_WATCHDOG_PRECHARGE_CHECK)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_DISCHARGE_TS);
        return;
    }

    // If the ts voltage is >60V then close the air+
    EAGLETRT_API_UNUSED(prv_mainboard_fsm_check_step(MAINBOARD_FSM_STEP_PRECHARGE_CHECK));
}

/*!
 * \brief Check if the air+ relay closed
 */
EAGLETRT_STATIC void mainboard_fsm_state_airp_check(void *data) {
    EAGLETRT_API_UNUSED(data);

    pcu_api_close(PCU_RELAY_AIR_POSITIVE);

    // Transition logic:

    // If there are faults go to FAULT or if ts-off is requested go to discharge
    if (prv_mainboard_fsm_check_ts_abort()) {
        return;
    }
    // Open the TS if the shutdown circuit is open
    if (prv_mainboard_fsm_check_sd_end()) {
        return;
    }
    // If the relay doesn't close discharge
    if (prv_mainboard_fsm_is_timed_out(MAINBOARD_FSM_WATCHDOG_AIRP_CHECK)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_DISCHARGE_TS);
        return;
    }

    // If the relay is confirmed closed go in ts-on
    EAGLETRT_API_UNUSED(prv_mainboard_fsm_check_step(MAINBOARD_FSM_STEP_AIRP_CHECK));
}

/*!
 * \brief Stay in a active supplying state of ts-on
 */
EAGLETRT_STATIC void mainboard_fsm_state_ts_on(void *data) {
    EAGLETRT_API_UNUSED(data);

    // Enjoy 600V (:

    // Transition logic:

    // If there are faults go to FAULT or if ts-off is requested go to discharge
    if (prv_mainboard_fsm_check_ts_abort()) {
        return;
    }

    // The conditions must keep matching, the SD end is part of them
    if (!prv_mainboard_fsm_check_step(MAINBOARD_FSM_STEP_TS_ON)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_DISCHARGE_TS);
    }
}

/*!
 * \brief Return to a safe state from any "dangerous" state of the ts
 */
EAGLETRT_STATIC void mainboard_fsm_state_discharge_ts(void *data) {
    EAGLETRT_API_UNUSED(data);

    // Open all of the relays
    pcu_api_open(PCU_RELAY_AIR_NEGATIVE);
    pcu_api_open(PCU_RELAY_AIR_POSITIVE);
    pcu_api_open(PCU_RELAY_PRECHARGE);

    // Transition logic:

    // If there are faults go to FAULT
    if (mainboard_status_api_has_faults()) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return;
    }
    // If a relay gets stuck and doesn't open then go to fatal
    if (prv_mainboard_fsm_is_timed_out(MAINBOARD_FSM_WATCHDOG_DISCHARGE_TS)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return;
    }

    // Go back to IDLE only once every relay is open and the TS is below 60V
    EAGLETRT_API_UNUSED(prv_mainboard_fsm_check_step(MAINBOARD_FSM_STEP_DISCHARGE_TS));
}

/*!
 * \brief Read the minimum pack tension
 */
EAGLETRT_STATIC void mainboard_fsm_state_balancing_read(void *data) {
    EAGLETRT_API_UNUSED(data);

    bal_control_api_stop_discharge();

    bal_control_api_update_target();

    // Transition logic:

    // If there are faults go to FAULT
    if (mainboard_status_api_has_faults()) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return;
    }
    // Balancing is not requested anymore
    if (!mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_BALANCING)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_IDLE);
        return;
    }
    // If after settling time the pack is balanced then go to IDLE else go to discharge
    if (prv_mainboard_fsm_is_timed_out(MAINBOARD_FSM_WATCHDOG_BALANCING_READ)) {
        if (mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_PACK_BALANCED)) {
            prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_IDLE);
            return;
        }
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_BALANCING_DISCHARGE);
    }
}

/*!
 * \brief Discharge the cells that are over the treshold read in the read state
 */
EAGLETRT_STATIC void mainboard_fsm_state_balancing_discharge(void *data) {
    EAGLETRT_API_UNUSED(data);

    bal_control_api_start_discharge();

    // Transition logic:

    // If there are faults go to FAULT
    if (mainboard_status_api_has_faults()) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return;
    }
    // BALANCING_READ handles the stop and goes back to IDLE
    if (!mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_BALANCING)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_BALANCING_READ);
        return;
    }
    // Once the time interval has passed transition to read
    if (prv_mainboard_fsm_is_timed_out(MAINBOARD_FSM_WATCHDOG_BALANCING_DISCHARGE)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_BALANCING_READ);
    }
}

/*!
 * \brief A board is being flashed
 */
EAGLETRT_STATIC void mainboard_fsm_state_flash(void *data) {
    EAGLETRT_API_UNUSED(data);

    // Transition logic:

    // If there are faults go to FAULT
    if (mainboard_status_api_has_faults()) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_FATAL);
        return;
    }
    // The request is dropped by a none target or by the flash-control message timeout
    if (!mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_FLASH)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_IDLE);
    }
}

EAGLETRT_STATIC void mainboard_fsm_state_fatal(void *data) {
    EAGLETRT_API_UNUSED(data);

    bal_control_api_stop_discharge();
    pcu_api_open_all();
    pcu_api_ams_activate();

    // Nothing to check: faults are never cleared and FATAL has no way out
}

/*! \brief Mainboard FSM states, indexed by \ref MainboardFsmStateID */
EAGLETRT_STATIC const struct State mainboard_fsm_states[MAINBOARD_FSM_STATE_COUNT] = {
    [MAINBOARD_FSM_STATE_INIT] = {
                                  .id = MAINBOARD_FSM_STATE_INIT,
                                  .function = mainboard_fsm_state_init,
                                  .next_default = MAINBOARD_FSM_STATE_INIT,
                                  .transitions = init_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(init_transitions),
                                  },
    [MAINBOARD_FSM_STATE_IDLE] = {
                                  .id = MAINBOARD_FSM_STATE_IDLE,
                                  .function = mainboard_fsm_state_idle,
                                  .next_default = MAINBOARD_FSM_STATE_IDLE,
                                  .transitions = idle_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(idle_transitions),
                                  },
    [MAINBOARD_FSM_STATE_WAIT_OK] = {
                                  .id = MAINBOARD_FSM_STATE_WAIT_OK,
                                  .function = mainboard_fsm_state_wait_ok,
                                  .next_default = MAINBOARD_FSM_STATE_WAIT_OK,
                                  .transitions = wait_ok_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(wait_ok_transitions),
                                  },
    [MAINBOARD_FSM_STATE_AIRN_CHECK] = {
                                  .id = MAINBOARD_FSM_STATE_AIRN_CHECK,
                                  .function = mainboard_fsm_state_airn_check,
                                  .next_default = MAINBOARD_FSM_STATE_AIRN_CHECK,
                                  .transitions = airn_check_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(airn_check_transitions),
                                  },
    [MAINBOARD_FSM_STATE_PRECHARGE_CHECK] = {
                                  .id = MAINBOARD_FSM_STATE_PRECHARGE_CHECK,
                                  .function = mainboard_fsm_state_precharge_check,
                                  .next_default = MAINBOARD_FSM_STATE_PRECHARGE_CHECK,
                                  .transitions = precharge_check_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(precharge_check_transitions),
                                  },
    [MAINBOARD_FSM_STATE_AIRP_CHECK] = {
                                  .id = MAINBOARD_FSM_STATE_AIRP_CHECK,
                                  .function = mainboard_fsm_state_airp_check,
                                  .next_default = MAINBOARD_FSM_STATE_AIRP_CHECK,
                                  .transitions = airp_check_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(airp_check_transitions),
                                  },
    [MAINBOARD_FSM_STATE_TS_ON] = {
                                  .id = MAINBOARD_FSM_STATE_TS_ON,
                                  .function = mainboard_fsm_state_ts_on,
                                  .next_default = MAINBOARD_FSM_STATE_TS_ON,
                                  .transitions = ts_on_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(ts_on_transitions),
                                  },
    [MAINBOARD_FSM_STATE_DISCHARGE_TS] = {
                                  .id = MAINBOARD_FSM_STATE_DISCHARGE_TS,
                                  .function = mainboard_fsm_state_discharge_ts,
                                  .next_default = MAINBOARD_FSM_STATE_DISCHARGE_TS,
                                  .transitions = discharge_ts_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(discharge_ts_transitions),
                                  },
    [MAINBOARD_FSM_STATE_BALANCING_READ] = {
                                  .id = MAINBOARD_FSM_STATE_BALANCING_READ,
                                  .function = mainboard_fsm_state_balancing_read,
                                  .next_default = MAINBOARD_FSM_STATE_BALANCING_READ,
                                  .transitions = balancing_read_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(balancing_read_transitions),
                                  },
    [MAINBOARD_FSM_STATE_BALANCING_DISCHARGE] = {
                                  .id = MAINBOARD_FSM_STATE_BALANCING_DISCHARGE,
                                  .function = mainboard_fsm_state_balancing_discharge,
                                  .next_default = MAINBOARD_FSM_STATE_BALANCING_DISCHARGE,
                                  .transitions = balancing_discharge_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(balancing_discharge_transitions),
                                  },
    [MAINBOARD_FSM_STATE_FLASH] = {
                                  .id = MAINBOARD_FSM_STATE_FLASH,
                                  .function = mainboard_fsm_state_flash,
                                  .next_default = MAINBOARD_FSM_STATE_FLASH,
                                  .transitions = flash_transitions,
                                  .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(flash_transitions),
                                  },
    [MAINBOARD_FSM_STATE_FATAL] = {
                                  .id = MAINBOARD_FSM_STATE_FATAL,
                                  .function = mainboard_fsm_state_fatal,
                                  .next_default = MAINBOARD_FSM_STATE_FATAL,
                                  .transitions = NULL,
                                  .num_transitions = 0U,
                                  },
};

enum FSMReturnCode mainboard_fsm_api_init(void) {
    return fsm_api_init(&mainboard_fsm_handler, mainboard_fsm_states, MAINBOARD_FSM_STATE_COUNT, MAINBOARD_FSM_STATE_INIT);
}

enum FSMReturnCode mainboard_fsm_api_run(void *context) {
    return fsm_api_run_state(&mainboard_fsm_handler, context);
}

enum MainboardFsmStateID mainboard_fsm_api_get_state(void) {
    return fsm_api_get_state(&mainboard_fsm_handler);
}