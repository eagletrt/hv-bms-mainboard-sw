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
#include "current.h"
#include "volt.h"
#include "feedback.h"
#include "internal-voltage.h"
#include "bal.h"
#include "imd.h"

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
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)identity_get_mainboard_version_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_MAINBOARD_VERSION_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}

/** @brief Send the cellboard version info via CAN */
void _tasks_send_cellboard_version(CellboardId id) {
    size_t byte_size = 0U;
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
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)fsm_get_canlib_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_STATUS_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}

/** @brief Send the BSM balancing status via CAN */
void _tasks_send_hv_balancing_status(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)bal_get_status_canlib_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_BALANCING_STATUS_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}

/** @brief Send the current via CAN */
void _tasks_send_hv_current(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)current_get_current_canlib_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_CURRENT_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}

/** @brief Send the power via CAN */
void _tasks_send_hv_power(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)current_get_power_canlib_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_POWER_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);  
}

/** @brief Send the cells voltages via CAN */
void _tasks_send_hv_cells_voltage(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)volt_get_canlib_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_CELLS_VOLTAGE_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size);
}

/** @brief Send the feedback status via CAN */
void _tasks_send_hv_feedback_status(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)feedback_get_status_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_FEEDBACK_STATUS_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size); 
}

/** @brief Send the digital feedbacks values via CAN */
void _tasks_send_hv_feedback_digital(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)feedback_get_digital_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_FEEDBACK_DIGITAL_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size); 
}

/** @brief Send the analog feedbacks values via CAN */
void _tasks_send_hv_feedback_analog(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)feedback_get_analog_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_FEEDBACK_ANALOG_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size); 
}

/** @brief Send the analog shutdown feedbacks values via CAN */
void _tasks_send_hv_feedback_analog_sd(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)feedback_get_analog_sd_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_FEEDBACK_ANALOG_SD_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size); 
}

/** @brief Send the IMD status via CAN */
void _tasks_send_hv_imd_status(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)imd_get_canlib_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_PRIMARY, PRIMARY_HV_IMD_STATUS_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size); 
}

/** @brief Send the set balancing status command via CAN */
void _tasks_send_cellboard_set_balancing_status(void) {
    size_t byte_size = 0U;
    uint8_t * payload = (uint8_t *)bal_get_set_status_canlib_payload(&byte_size);
    can_comm_tx_add(CAN_NETWORK_BMS, BMS_CELLBOARD_SET_BALANCING_STATUS_INDEX, CAN_FRAME_TYPE_DATA, payload, byte_size); 
}

/** @brief Update all the digital feedbacks */
void _tasks_read_digital_feedbacks(void) {
    (void)feedback_update_digital_feedback_all();
}

/** @brief Start the conversion of all the analog feedbacks */
void _tasks_start_analog_conversion_feedbacks(void) {
    (void)feedback_start_analog_conversion_all();
}

/** @brief Update all the feedbacks status */
void _tasks_update_feedbacks_status(void) {
    (void)feedback_update_status();
}

/** @brief Start the internal voltages ADC conversion */
void _tasks_start_internal_voltage_conversion(void) {
    (void)internal_voltage_read_all();
}

TasksReturnCode tasks_init(milliseconds_t resolution) {
    if (resolution == 0U)
        resolution = 1U;

    // Initialize the tasks with the X macro
#define TASKS_X(NAME, ENABLED, START, INTERVAL, EXEC) \
    do { \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].enabled = (ENABLED); \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].id = TASKS_NAME_TO_ID(NAME); \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].start = (START); \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].interval = TIMEBASE_TIME_TO_TICKS(INTERVAL, resolution); \
        htasks.tasks[TASKS_NAME_TO_ID(NAME)].exec = (EXEC); \
    } while(0U);

    TASKS_X_LIST
#undef TASKS_X

    return TASKS_OK;
}

TasksReturnCode tasks_set_enable(TasksId id, bool enabled) {
    if (id >= TASKS_ID_COUNT)
        return TASKS_INVALID_ID;
    htasks.tasks[id].enabled = enabled;
    return TASKS_OK; 
}

bool tasks_is_enabled(TasksId id) {
    if (id >= TASKS_ID_COUNT)
        return false;
    return htasks.tasks[id].enabled;
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
    [TASKS_OK] = "ok",
    [TASKS_INVALID_ID] = "invalid id"
};

_STATIC char * tasks_return_code_descritpion[] = {
    [TASKS_OK] = "executed successfully"
    [TASKS_INVALID_ID] = "the given identifier does not exists"
};

#define TASKS_X(NAME, START, INTERVAL, EXEC) [TASKS_NAME_TO_ID(NAME)] = #NAME,
_STATIC char * tasks_id_name[] = {
    TASKS_X_LIST
};
#undef TASKS_X

#endif // CONF_TASKS_STRINGS_ENALBE

#endif // CONF_TASKS_MODULE_ENABLE
