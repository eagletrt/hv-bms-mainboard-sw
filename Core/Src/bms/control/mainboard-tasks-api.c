/*!
 * \file mainboard-tasks-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tasks that have to be executed at a certain interval
 */

#include "mainboard-tasks-api.h"

#include <stdint.h>

#include "can-communication-api.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-def.h"
#include "mainboard-fsm-api.h"
#include "mainboard-tasks.h"
#include "phc-api.h"
#include "tasks.h"
#include "tasks-api.h"

EAGLETRT_STATIC struct TasksHandler mainboard_tasks_handler;

// Every task needs a slot in the tasks module
static_assert(MAINBOARD_TASKS_COUNT <= TASKS_MAX_COUNT, "Increase TASKS_MAX_COUNT");

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
};

#undef TASK

enum TasksReturnCode mainboard_tasks_api_init(task_tick_callback tick_callback) {
    return tasks_api_init(&mainboard_tasks_handler, mainboard_control_tasks, MAINBOARD_TASKS_COUNT, tick_callback);
}

enum TasksReturnCode mainboard_tasks_api_routine(void) {
    return tasks_api_routine(&mainboard_tasks_handler);
}