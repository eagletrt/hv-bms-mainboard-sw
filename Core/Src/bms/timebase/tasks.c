/**
 * @brief tasks.c
 * @date 2024-05-16
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Tasks that have to be executed at a certain interval
 */

#include "tasks.h"

#include "bms_network.h"
#include "can-comm.h"
#include "identity.h"
#include "timebase.h"
#include "fsm.h"

#ifdef CONF_TASKS_MODULE_ENABLE

/**
 * @brief Tasks hanlder struct
 *
 * @param tasks The array of tasks
 */
_STATIC struct {
    Task tasks[TASKS_COUNT];
} htasks;

/** @brief Send the mainboard version info via CAN */
void _tasks_send_mainboard_version(void) {
    size_t byte_size;
    uint8_t * payload = (uint8_t *)identity_get_mainboard_version_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_MAINBOARD_VERSION_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}

/** @brief Send the cellboard version info via CAN */
void _tasks_send_cellboard_version(CellboardId id) {
    size_t byte_size;
    uint8_t * payload = (uint8_t *)identity_get_cellboard_version_payload(id, &byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_CELLBOARD_VERSION_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}
void _tasks_send_cellboard_0_version(void) { _tasks_send_cellboard_version(CELLBOARD_ID_0); }
void _tasks_send_cellboard_1_version(void) { _tasks_send_cellboard_version(CELLBOARD_ID_1); }
void _tasks_send_cellboard_2_version(void) { _tasks_send_cellboard_version(CELLBOARD_ID_2); }
void _tasks_send_cellboard_3_version(void) { _tasks_send_cellboard_version(CELLBOARD_ID_3); }
void _tasks_send_cellboard_4_version(void) { _tasks_send_cellboard_version(CELLBOARD_ID_4); }
void _tasks_send_cellboard_5_version(void) { _tasks_send_cellboard_version(CELLBOARD_ID_5); }

/** @brief Send the mainboard and cellboard FSM status via CAN */
void _tasks_send_hv_status(void) {
    size_t byte_size;
    uint8_t * payload = (uint8_t *)fsm_get_can_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_STATUS_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}

TasksReturnCode tasks_init(milliseconds_t resolution) {
    if (resolution == 0U)
        resolution = 1U;

    // Initialize the tasks with the X macro
#define TASKS_X(NAME, START, INTERVAL, EXEC) \
    do { \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].id = TASKS_NAME_TO_ID(NAME); \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].start = (START); \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].interval = TIMEBASE_TIME_TO_TICKS(INTERVAL, resolution); \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].exec = (EXEC); \
    } while(0U);

    TASKS_X_LIST
#undef TASKS_X

    return TASKS_OK;
}

Task * tasks_get_task(TasksId id) {
    if (id >= TASKS_ID_COUNT)
        return NULL;
    return &htasks.tasks[id];
}

ticks_t tasks_get_start(TasksId id) {
    if (id >= TASKS_ID_COUNT)
        return 0U;
    return htasks.tasks[id].start;
}

ticks_t tasks_get_interval(TasksId id) {
    if (id >= TASKS_ID_COUNT)
        return 0U;
    return htasks.tasks[id].interval;
}

tasks_callback tasks_get_callback(TasksId id) {
    if (id >= TASKS_ID_COUNT)
        return 0U;
    return htasks.tasks[id].exec;
}

#ifdef CONF_TASKS_STRINGS_ENABLE

_STATIC char * tasks_module_name = "tasks";

_STATIC char * tasks_return_code_name[] = {
    [TASKS_OK] = "ok"
};

_STATIC char * tasks_return_code_descritpion[] = {
    [TASKS_OK] = "executed successfully"
};

#define TASKS_X(NAME, START, INTERVAL, EXEC) [TASKS_NAME_TO_ID(NAME)] = #NAME,
_STATIC char * tasks_id_name[] = {
    TASKS_X_LIST
};
#undef TASKS_X

#endif // CONF_TASKS_STRINGS_ENALBE

#endif // CONF_TASKS_MODULE_ENABLE
