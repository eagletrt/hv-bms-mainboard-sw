/*!
 * \file tasks.c
 * \date 2024-05-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Tasks that have to be executed at a certain interval
 */

#include "tasks.h"

#include "eagletrt.h"
#include "bms_network.h"
#include "can-comm-api.h"
#include "identity-api.h"
#include "timebase.h"
#include "fsm.h"
#include "current-api.h"
#include "volt-api.h"
#include "feedback-api.h"
#include "internal-voltage-api.h"
#include "bal-api.h"
#include "imd-api.h"
#include "temp-api.h"
#include "error-api.h"
#include "cooling-temp-api.h"

#ifdef CONF_TASKS_MODULE_ENABLE

EAGLETRT_STATIC struct TaskHandler task_handler;

/*! \brief Send the mainboard version info via CAN */
void prv_tasks_send_mainboard_version(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)identity_api_get_mainboard_version_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_MAINBOARD_VERSION_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the cellboard version info via CAN */
void prv_tasks_send_cellboard_version(CellboardId cellboard_id) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)identity_api_get_cellboard_version_payload(cellboard_id, &byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_CELLBOARD_VERSION_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}
void prv_tasks_send_cellboard_0_version(void) {
    prv_tasks_send_cellboard_version(CELLBOARD_ID_0);
}
void prv_tasks_send_cellboard_1_version(void) {
    prv_tasks_send_cellboard_version(CELLBOARD_ID_1);
}
void prv_tasks_send_cellboard_2_version(void) {
    prv_tasks_send_cellboard_version(CELLBOARD_ID_2);
}
void prv_tasks_send_cellboard_3_version(void) {
    prv_tasks_send_cellboard_version(CELLBOARD_ID_3);
}
void prv_tasks_send_cellboard_4_version(void) {
    prv_tasks_send_cellboard_version(CELLBOARD_ID_4);
}
void prv_tasks_send_cellboard_5_version(void) {
    prv_tasks_send_cellboard_version(CELLBOARD_ID_5);
}

/*! \brief Send the mainboard and cellboard FSM status via CAN */
void prv_tasks_send_hv_status(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)fsm_get_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_STATUS_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the BSM balancing status via CAN */
void prv_tasks_send_hv_balancing_status(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)bal_api_get_status_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_BALANCING_STATUS_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the current via CAN */
void prv_tasks_send_hv_current(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)current_api_get_current_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_CURRENT_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the power via CAN */
void prv_tasks_send_hv_power(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)current_api_get_power_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_POWER_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the Tractive System voltages info via CAN */
void prv_tasks_send_hv_ts_voltage(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)internal_voltage_api_get_ts_voltage_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_TS_VOLTAGE_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the cells voltages via CAN */
void prv_tasks_send_hv_cells_voltage(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)volt_api_get_cells_voltage_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_CELLS_VOLTAGE_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the cells voltage stats via CAN */
void prv_tasks_send_hv_cells_voltage_stats(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)volt_api_get_cells_voltage_stats_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_CELLS_VOLTAGE_STATS_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the cells temperature via CAN */
void prv_tasks_send_hv_cells_temperature(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)temp_api_get_cells_temperature_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_CELLS_TEMPERATURE_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the cells temperature stats via CAN */
void prv_tasks_send_hv_cells_temperature_stats(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)temp_api_get_cells_temperature_stats_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_CELLS_TEMP_STATS_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the cooling temperatures via CAN */
void prv_tasks_send_hv_cooling_temperature(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)cooling_temp_api_get_temperatures_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_CELLS_TEMP_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the feedback status via CAN */
void prv_tasks_send_hv_feedback_status(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)feedback_api_get_status_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_FEEDBACK_STATUS_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the digital feedbacks values via CAN */
void prv_tasks_send_hv_feedback_digital(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)feedback_api_get_digital_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_FEEDBACK_DIGITAL_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the analog feedbacks values via CAN */
void prv_tasks_send_hv_feedback_analog(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)feedback_api_get_analog_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_FEEDBACK_ANALOG_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the analog shutdown feedbacks values via CAN */
void prv_tasks_send_hv_feedback_analog_sd(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)feedback_api_get_analog_sd_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_FEEDBACK_ANALOG_SD_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the IMD status via CAN */
void prv_tasks_send_hv_imd_status(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)imd_api_get_status_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_IMD_STATUS_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the set balancing status command via CAN */
void prv_tasks_send_cellboard_set_balancing_status(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)bal_api_get_set_status_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_BMS,
        BMS_CELLBOARD_SET_BALANCING_STATUS_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Send the errors status via CAN if an error occoured */
void prv_tasks_send_errors(void) {
    size_t byte_size = 0U;
    uint8_t *const payload = (uint8_t *const)error_api_get_error_canlib_payload(&byte_size);
    can_comm_tx_add(
        CAN_NETWORK_PRIMARY,
        PRIMARY_HV_ERROR_INDEX,
        CAN_FRAME_TYPE_DATA,
        payload,
        byte_size);
}

/*! \brief Update all the digital feedbacks */
void prv_tasks_read_digital_feedbacks(void) {
    (void)feedback_api_update_digital_feedback_all();
}

/*! \brief Start the conversion of all the analog feedbacks */
void prv_tasks_start_analog_conversion_feedbacks(void) {
    (void)feedback_api_start_analog_conversion_all();
}

/*! \brief Update all the feedbacks status */
void prv_tasks_update_feedbacks_status(void) {
    (void)feedback_api_update_status();
}

/*! \brief Start the internal voltages ADC conversion */
void prv_tasks_start_internal_voltage_conversion(void) {
    (void)internal_voltage_api_read_all();
}

enum TasksReturnCode tasks_init(milliseconds_t resolution) {
    resolution = EAGLETRT_API_MAX(1, resolution);

    // Initialize the tasks with the X macro
#define TASKS_X(NAME, ENABLED, START, INTERVAL, EXEC)                                                       \
    do {                                                                                                    \
        task_handler.tasks[TASKS_NAME_TO_ID(NAME)].enabled = (ENABLED);                                     \
        task_handler.tasks[TASKS_NAME_TO_ID(NAME)].id = TASKS_NAME_TO_ID(NAME);                             \
        task_handler.tasks[TASKS_NAME_TO_ID(NAME)].start = (START);                                         \
        task_handler.tasks[TASKS_NAME_TO_ID(NAME)].interval = TIMEBASE_TIME_TO_TICKS(INTERVAL, resolution); \
        task_handler.tasks[TASKS_NAME_TO_ID(NAME)].exec = (EXEC);                                           \
    } while (0U);

    TASKS_X_LIST
#undef TASKS_X

    return TASKS_RC_OK;
}

enum TasksReturnCode tasks_set_enable(const enum TasksId task_id, const bool enabled) {
    if (task_id >= TASKS_ID_COUNT) {
        return TASKS_RC_INVALID_ID;
    }
    task_handler.tasks[task_id].enabled = enabled;
    return TASKS_RC_OK;
}

bool tasks_is_enabled(const enum TasksId task_id) {
    if (task_id >= TASKS_ID_COUNT) {
        return false;
    }
    return task_handler.tasks[task_id].enabled;
}

struct Task *tasks_get_task(const enum TasksId task_id) {
    if (task_id >= TASKS_ID_COUNT) {
        return NULL;
    }
    return &task_handler.tasks[task_id];
}

ticks_t tasks_get_start(const enum TasksId task_id) {
    if (task_id >= TASKS_ID_COUNT) {
        return 0U;
    }
    return task_handler.tasks[task_id].start;
}

ticks_t tasks_get_interval(const enum TasksId task_id) {
    if (task_id >= TASKS_ID_COUNT) {
        return 0U;
    }
    return task_handler.tasks[task_id].interval;
}

tasks_callback tasks_get_callback(const enum TasksId task_id) {
    if (task_id >= TASKS_ID_COUNT) {
        return 0U;
    }
    return task_handler.tasks[task_id].exec;
}

#ifdef CONF_TASKS_STRINGS_ENABLE

EAGLETRT_STATIC char *tasks_module_name = "tasks";

EAGLETRT_STATIC char *tasks_return_code_name[] = {
    [TASKS_RC_OK] = "ok",
    [TASKS_INVALID_ID] = "invalid id"
};

EAGLETRT_STATIC char *tasks_return_code_description[] = {
    [TASKS_RC_OK] = "executed successfully",
    [TASKS_INVALID_ID] = "the given identifier does not exists"
};

#define TASKS_X(NAME, START, INTERVAL, EXEC) [TASKS_NAME_TO_ID(NAME)] = #NAME,
EAGLETRT_STATIC char *tasks_id_name[] = {
    TASKS_X_LIST
};
#undef TASKS_X

#endif // CONF_TASKS_STRINGS_ENALBE

#endif // CONF_TASKS_MODULE_ENABLE
