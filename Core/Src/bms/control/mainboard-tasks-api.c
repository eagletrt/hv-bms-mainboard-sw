/*!
 * \file mainboard-tasks-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tasks that have to be executed at a certain interval
 */

#include "mainboard-tasks-api.h"

#include <stddef.h>
#include <stdint.h>

#include "can-communication-api.h"
#include "display-api.h"
#include "display.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "feedback-api.h"
#include "internal-voltages-api.h"
#include "internal-voltages.h"
#include "led-api.h"
#include "led.h"
#include "mainboard-conf.h"
#include "mainboard-def.h"
#include "mainboard-fsm-api.h"
#include "mainboard-fsm.h"
#include "mainboard-tasks.h"
#include "phc-api.h"
#include "sensing-api.h"
#include "tasks.h"
#include "tasks-api.h"

EAGLETRT_STATIC struct TasksHandler mainboard_tasks_handler;

/*! \brief Current time used by the display animations */
EAGLETRT_STATIC task_tick_callback mainboard_tasks_get_tick;

// Every task needs a slot in the tasks module
static_assert(MAINBOARD_TASKS_COUNT <= TASKS_MAX_COUNT, "Increase TASKS_MAX_COUNT");

/*! \brief Number of frames of a display animation */
#define MAINBOARD_TASKS_ANIMATION_SIZE(animation) (sizeof(animation) / sizeof((animation)[0]))

/*! \brief Display animation of INIT and IDLE */
EAGLETRT_STATIC const enum DisplaySegmentBit mainboard_tasks_idle_animation[] = {
    DISPLAY_SEGMENT_BIT_TOP_LEFT,
    DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_SEGMENT_BIT_BOTTOM | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_TOP_RIGHT,
    DISPLAY_SEGMENT_BIT_TOP_RIGHT,
    DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_SEGMENT_BIT_BOTTOM | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_TOP_LEFT,
};

/*! \brief Display animation of TS_ON */
EAGLETRT_STATIC const enum DisplaySegmentBit mainboard_tasks_ts_on_animation[] = {
    DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT,
    DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_SEGMENT_BIT_BOTTOM | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_TOP_LEFT,
    DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_TOP,
};

/*! \brief Display animation of the balancing states */
EAGLETRT_STATIC const enum DisplaySegmentBit mainboard_tasks_balancing_animation[] = {
    DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT,
    DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_MIDDLE,
    DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_SEGMENT_BIT_BOTTOM | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_MIDDLE,
    DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_TOP_LEFT,
    DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_TOP,
};

/*! \brief Display string of FATAL */
EAGLETRT_STATIC const char mainboard_tasks_fatal_string[] = "E E EEEE ";

/*! \brief The FATAL string frames last longer so that it can be read */
#define MAINBOARD_TASKS_FATAL_FRAME_MS (MAINBOARD_TASKS_DISPLAY_FRAME_MS * 4U)

/*!
 * \brief Run the sensing routine
 *
 * \details A source that keeps failing is caught by the sensing watchdogs
 *
 * \param[in] task_id Unused
 */
EAGLETRT_STATIC void prv_mainboard_tasks_sensing_run(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    EAGLETRT_API_UNUSED(sensing_api_routine());
}

/*!
 * \brief Run the feedback routine
 *
 * \param[in] task_id Unused
 */
EAGLETRT_STATIC void prv_mainboard_tasks_feedback_run(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    EAGLETRT_API_UNUSED(feedback_api_routine());
}

/*!
 * \brief Run the mainboard FSM
 *
 * \param[in] task_id Unused
 */
EAGLETRT_STATIC void prv_mainboard_tasks_fsm_run(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    EAGLETRT_API_UNUSED(mainboard_fsm_api_run(NULL));
}

/*!
 * \brief Map a CAN task to the network it processes
 *
 * \param[in] task_id The ID of the CAN task, a \ref MainboardTasksID
 *
 * \return The network of the task, \ref CAN_NETWORK_COUNT if the task is not a CAN task
 */
EAGLETRT_STATIC enum CanNetwork prv_mainboard_tasks_can_network(uint8_t task_id) {
    switch (task_id) {
        case MAINBOARD_TASKS_CAN_TX_PRIMARY:
        case MAINBOARD_TASKS_CAN_RX_PRIMARY:
            return CAN_NETWORK_PRIMARY;
        case MAINBOARD_TASKS_CAN_TX_BMS:
        case MAINBOARD_TASKS_CAN_RX_BMS:
            return CAN_NETWORK_BMS;
        default:
            return CAN_NETWORK_COUNT;
    }
}

/*!
 * \brief Process the TX queue of the network of the task
 *
 * \param[in] task_id The ID of the executed task, a \ref MainboardTasksID
 */
EAGLETRT_STATIC void prv_mainboard_tasks_can_tx(uint8_t task_id) {
    EAGLETRT_API_UNUSED(can_communication_api_process_tx(prv_mainboard_tasks_can_network(task_id)));
}

/*!
 * \brief Process the RX queue of the network of the task
 *
 * \param[in] task_id The ID of the executed task, a \ref MainboardTasksID
 */
EAGLETRT_STATIC void prv_mainboard_tasks_can_rx(uint8_t task_id) {
    EAGLETRT_API_UNUSED(can_communication_api_process_rx(prv_mainboard_tasks_can_network(task_id)));
}

/*!
 * \brief Run the PHC control of the task
 *
 * \details A check that does not run (data not received yet) is caught by the PHC watchdogs
 *
 * \param[in] task_id The ID of the executed task, a \ref MainboardTasksID
 */
EAGLETRT_STATIC void prv_mainboard_tasks_phc_check(uint8_t task_id) {
    if (task_id < MAINBOARD_TASKS_PHC_CELL_UNDER_VOLTAGE || task_id > MAINBOARD_TASKS_PHC_PRECHARGE_COMPLETE) {
        return;
    }
    const enum PHCControl control = (enum PHCControl)(task_id - MAINBOARD_TASKS_PHC_CELL_UNDER_VOLTAGE);
    EAGLETRT_API_UNUSED(phc_api_check(control));
}

/*!
 * \brief Toggle the heartbeat LED
 *
 * \param[in] task_id Unused
 */
EAGLETRT_STATIC void prv_mainboard_tasks_led_heartbeat(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    EAGLETRT_API_UNUSED(led_api_toggle_status(LED_ID_1));
}

/*!
 * \brief Show the precharge progress as a hex digit from 0 to A with the decimal point on
 *
 * \details The progress is the ratio between the TS and the pack voltage, 0 until both are received
 */
EAGLETRT_STATIC void prv_mainboard_tasks_display_precharge(void) {
    constexpr float digit_max = 10.F;
    float ratio = 0.F;

    if (internal_voltages_api_is_voltages_ready()) {
        const volt pack = internal_voltages_api_get_value(INTERNAL_VOLTAGES_INDEX_PACK);
        if (pack > 0.F) {
            ratio = internal_voltages_api_get_value(INTERNAL_VOLTAGES_INDEX_TS) / pack;
        }
    }

    const uint8_t digit = (uint8_t)(EAGLETRT_API_CLAMP(ratio, 0.F, 1.F) * digit_max);
    const enum DisplayCharacterCode code = display_api_get_code_from_hex_digit(digit);
    EAGLETRT_API_UNUSED(display_api_set_segment_all((bit_flag8)code | (bit_flag8)DISPLAY_SEGMENT_BIT_DECIMAL_POINT));
}

/*!
 * \brief Show the state of the mainboard FSM on the 7-segment display
 *
 * \param[in] task_id Unused
 */
EAGLETRT_STATIC void prv_mainboard_tasks_display_update(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    const ticks now = mainboard_tasks_get_tick();

    switch (mainboard_fsm_api_get_state()) {
        case MAINBOARD_FSM_STATE_INIT:
        case MAINBOARD_FSM_STATE_IDLE:
            EAGLETRT_API_UNUSED(display_api_run_animation(mainboard_tasks_idle_animation,
                                                          MAINBOARD_TASKS_ANIMATION_SIZE(mainboard_tasks_idle_animation),
                                                          MAINBOARD_TASKS_DISPLAY_FRAME_MS,
                                                          now));
            break;

        case MAINBOARD_FSM_STATE_WAIT_OK:
        case MAINBOARD_FSM_STATE_AIRN_CHECK:
        case MAINBOARD_FSM_STATE_PRECHARGE_CHECK:
        case MAINBOARD_FSM_STATE_AIRP_CHECK:
        case MAINBOARD_FSM_STATE_DISCHARGE_TS:
            prv_mainboard_tasks_display_precharge();
            break;

        case MAINBOARD_FSM_STATE_TS_ON:
            EAGLETRT_API_UNUSED(display_api_run_animation(mainboard_tasks_ts_on_animation,
                                                          MAINBOARD_TASKS_ANIMATION_SIZE(mainboard_tasks_ts_on_animation),
                                                          MAINBOARD_TASKS_DISPLAY_FRAME_MS,
                                                          now));
            break;

        case MAINBOARD_FSM_STATE_BALANCING_READ:
        case MAINBOARD_FSM_STATE_BALANCING_DISCHARGE:
            EAGLETRT_API_UNUSED(display_api_run_animation(mainboard_tasks_balancing_animation,
                                                          MAINBOARD_TASKS_ANIMATION_SIZE(mainboard_tasks_balancing_animation),
                                                          MAINBOARD_TASKS_DISPLAY_FRAME_MS,
                                                          now));
            break;

        case MAINBOARD_FSM_STATE_FLASH:
            EAGLETRT_API_UNUSED(display_api_set_character('F', true));
            break;

        case MAINBOARD_FSM_STATE_FATAL:
        default:
            // The terminator is not shown
            EAGLETRT_API_UNUSED(display_api_run_animation_string(mainboard_tasks_fatal_string,
                                                                 sizeof(mainboard_tasks_fatal_string) - 1U,
                                                                 MAINBOARD_TASKS_FATAL_FRAME_MS,
                                                                 now));
            break;
    }
}

/*!
 * \brief Task initializer, repeats 0 means the task runs forever
 *
 * \details The start spreads the first executions over consecutive ticks
 */
#define TASK(id, period, fn)          \
    [(id)] = {                        \
        .task_id = (id),              \
        .state = TASKS_STATE_ENABLED, \
        .repeats = 0U,                \
        .function = (fn),             \
        .interval = (period),         \
        .start = (id),                \
    }

EAGLETRT_STATIC TaskList mainboard_control_tasks = {
    // Sensing
    TASK(MAINBOARD_TASKS_SENSING_RUN, MAINBOARD_TASKS_SENSING_PERIOD_MS, prv_mainboard_tasks_sensing_run),
    TASK(MAINBOARD_TASKS_FEEDBACK_RUN, MAINBOARD_TASKS_FEEDBACK_PERIOD_MS, prv_mainboard_tasks_feedback_run),

    // FSM
    TASK(MAINBOARD_TASKS_FSM_RUN, MAINBOARD_TASKS_FSM_PERIOD_MS, prv_mainboard_tasks_fsm_run),

    // CAN
    TASK(MAINBOARD_TASKS_CAN_TX_PRIMARY, MAINBOARD_TASKS_CAN_PERIOD_MS, prv_mainboard_tasks_can_tx),
    TASK(MAINBOARD_TASKS_CAN_TX_BMS, MAINBOARD_TASKS_CAN_PERIOD_MS, prv_mainboard_tasks_can_tx),
    TASK(MAINBOARD_TASKS_CAN_RX_PRIMARY, MAINBOARD_TASKS_CAN_PERIOD_MS, prv_mainboard_tasks_can_rx),
    TASK(MAINBOARD_TASKS_CAN_RX_BMS, MAINBOARD_TASKS_CAN_PERIOD_MS, prv_mainboard_tasks_can_rx),

    // PHC faults
    TASK(MAINBOARD_TASKS_PHC_CELL_UNDER_VOLTAGE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_CELL_OVER_VOLTAGE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_CELL_UNDER_TEMPERATURE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_CELL_OVER_TEMPERATURE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_DISCHARGE_UNDER_TEMPERATURE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_DISCHARGE_OVER_TEMPERATURE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_OVER_CURRENT, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_OVER_POWER, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_COOLING_UNDER_TEMPERATURE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_COOLING_OVER_TEMPERATURE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_CELLBOARD_FAULT, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_IMD_FAULT, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_VOLTAGE_INDICATOR_DISCONNECTED, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),

    // PHC conditions
    TASK(MAINBOARD_TASKS_PHC_PACK_BALANCED, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),
    TASK(MAINBOARD_TASKS_PHC_PRECHARGE_COMPLETE, MAINBOARD_TASKS_PHC_PERIOD_MS, prv_mainboard_tasks_phc_check),

    // User interface
    TASK(MAINBOARD_TASKS_LED_HEARTBEAT, MAINBOARD_TASKS_HEARTBEAT_PERIOD_MS, prv_mainboard_tasks_led_heartbeat),
    TASK(MAINBOARD_TASKS_DISPLAY_UPDATE, MAINBOARD_TASKS_DISPLAY_PERIOD_MS, prv_mainboard_tasks_display_update),
};

#undef TASK

enum TasksReturnCode mainboard_tasks_api_init(task_tick_callback tick_callback) {
    if (tick_callback == NULL) {
        return TASKS_RC_NULL_POINTER;
    }
    mainboard_tasks_get_tick = tick_callback;
    return tasks_api_init(&mainboard_tasks_handler, mainboard_control_tasks, MAINBOARD_TASKS_COUNT, tick_callback);
}

enum TasksReturnCode mainboard_tasks_api_routine(void) {
    return tasks_api_routine(&mainboard_tasks_handler);
}