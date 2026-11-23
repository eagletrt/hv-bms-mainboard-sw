/*!
 * \file mainboard-fsm-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Main mainboard FSM that handles all the operations of the BMS
 */

#include "mainboard-fsm-api.h"
#include "eagletrt-fsm-api.h"
#include "eagletrt-fsm.h"
#include <string.h>

#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-status-api.h"
#include "mainboard-status.h"

EAGLETRT_STATIC struct FSMHandler mainboard_fsm_handler;

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
};

/*!
 * \brief Transition the mainboard FSM to a new state
 * \returns nothing, as this function is only called within states if the transition fails then
 * there are way bigger problems to worry about
 */
void prv_mainboard_fsm_transition_to(enum MainboardFsmStateID state) {
    fsm_api_transition_to(&mainboard_fsm_handler, state);
}

struct Transition init_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,  NULL },
    { MAINBOARD_FSM_STATE_FATAL, NULL },
};

struct Transition idle_transitions[] = {
    { MAINBOARD_FSM_STATE_WAIT_OK,        NULL },
    { MAINBOARD_FSM_STATE_BALANCING_READ, NULL },
    { MAINBOARD_FSM_STATE_FLASH,          NULL },
    { MAINBOARD_FSM_STATE_FATAL,          NULL },
};

struct Transition wait_ok_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, NULL },
    { MAINBOARD_FSM_STATE_FATAL,        NULL },
    { MAINBOARD_FSM_STATE_AIRN_CHECK,   NULL },
};

struct Transition airn_check_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS,    NULL },
    { MAINBOARD_FSM_STATE_FATAL,           NULL },
    { MAINBOARD_FSM_STATE_PRECHARGE_CHECK, NULL },
};

struct Transition precharge_check_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, NULL },
    { MAINBOARD_FSM_STATE_FATAL,        NULL },
    { MAINBOARD_FSM_STATE_AIRP_CHECK,   NULL }
};

struct Transition airp_check_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, NULL },
    { MAINBOARD_FSM_STATE_FATAL,        NULL },
    { MAINBOARD_FSM_STATE_TS_ON,        NULL }
};

struct Transition ts_on_transitions[] = {
    { MAINBOARD_FSM_STATE_DISCHARGE_TS, NULL },
    { MAINBOARD_FSM_STATE_FATAL,        NULL },
};

struct Transition discharge_ts_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,  NULL },
    { MAINBOARD_FSM_STATE_FATAL, NULL },
};

struct Transition balancing_read_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,                NULL },
    { MAINBOARD_FSM_STATE_BALANCING_DISCHARGE, NULL },
    { MAINBOARD_FSM_STATE_FATAL,               NULL },
};

struct Transition balancing_discharge_transitions[] = {
    { MAINBOARD_FSM_STATE_BALANCING_READ, NULL },
    { MAINBOARD_FSM_STATE_FATAL,          NULL },
};

struct Transition flash_transitions[] = {
    { MAINBOARD_FSM_STATE_IDLE,  NULL },
    { MAINBOARD_FSM_STATE_FATAL, NULL },
};

EAGLETRT_STATIC void mainboard_fsm_state_init(void *data) {
    // run post, check the return code, transition either to IDLE or to fatal
}

EAGLETRT_STATIC void mainboard_fsm_state_idle(void *data) {
    EAGLETRT_API_UNUSED(data);

    // Actively do nothing (:

    // If the FSM is unsafe in a safe state then transition to FATAL
    if (!mainboard_control_is_safe()) {
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
    }
    // If a BALANCING message has been heard then transition to BALANCING state
    if (mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_BALANCING)) {
        prv_mainboard_fsm_transition_to(MAINBOARD_FSM_STATE_BALANCING_READ);
    }
}

EAGLETRT_STATIC void mainboard_fsm_state_wait_ok(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_airn_check(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_precharge_check(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_airp_check(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_ts_on(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_discharge_ts(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_balancing_read(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_balancing_discharge(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_flash(void *data) {
    EAGLETRT_API_UNUSED(data);
}

EAGLETRT_STATIC void mainboard_fsm_state_fatal(void *data) {
    EAGLETRT_API_UNUSED(data);
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