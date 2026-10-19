/*!
 * \file mainboard-tasks-api.c
 * \date 2024-05-16
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tasks that have to be executed at a certain interval
 */

#include "mainboard-tasks-api.h"

#include "tasks.h"
#include "tasks-api.h"
#include "mainboard-tasks.h"

#include <stdint.h>
#include <string.h>

#include "can-bms-api.h"
#include "can-bms.h"
#include "can-communication.h"
#include "can-communication-api.h"
#include "can-primary-api.h"

#include "current-api.h"
#include "eagletrt-api.h"

#include "feedback-api.h"
#include "volt-api.h"
#include "temp-api.h"
#include "cooling-temp-api.h"
#include "internal-voltage-api.h"
#include "fsm.h"
#include "imd-api.h"
#include "identity-api.h"
#include "error-api.h"
#include "bal-api.h"

#ifdef CONF_TASKS_MODULE_ENABLE

EAGLETRT_STATIC struct TasksHandler tasks_handler;
EAGLETRT_STATIC struct MainboardTaskHandler mainboard_tasks_handler;

EAGLETRT_STATIC const struct TaskCanPrimaryMsg can_msgs[] = {
    // VERSIONS
    [TASK_SEND_MAINBOARD_VERSION] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVERSION, .get_payload = identity_api_get_mainboard_version_payload },
    [TASK_SEND_MAINBOARD_VERSION_INFO] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVERSIONINFO, .get_payload = identity_api_get_mainboard_version_info_payload },
    [TASK_SEND_MAINBOARD_LIBCAN_VERSION] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDLIBCANVERSION, .get_payload = identity_api_get_mainboard_libcan_version_payload },
    [TASK_SEND_MAINBOARD_LIBCAN_VERSION_INFO] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDLIBCANVERSIONINFO, .get_payload = identity_api_get_mainboard_libcan_version_info_payload },

    // STATUS
    [TASK_SEND_STATUS] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACSTATUS, .get_payload = fsm_get_canlib_payload },
    [TASK_SEND_IMD_STATUS] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDIMD, .get_payload = imd_api_get_canlib_payload },

    // HV INFO
    [TASK_SEND_CURRENT] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCURRENTINFO, .get_payload = current_api_get_canlib_payload },
    [TASK_SEND_VOLTAGE_INFO] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVOLTAGEINFO, .get_payload = internal_voltage_api_get_canlib_payload },
    [TASK_SEND_CELLBOARD1_VOLTAGE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE, .get_payload = volt_api_get_cellboard1_voltage_canlib_payload },
    [TASK_SEND_CELLBOARD2_VOLTAGE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE, .get_payload = volt_api_get_cellboard2_voltage_canlib_payload },
    [TASK_SEND_CELLBOARD3_VOLTAGE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE, .get_payload = volt_api_get_cellboard3_voltage_canlib_payload },
    [TASK_SEND_CELLBOARD4_VOLTAGE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE, .get_payload = volt_api_get_cellboard4_voltage_canlib_payload },
    [TASK_SEND_CELLBOARD5_VOLTAGE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE, .get_payload = volt_api_get_cellboard5_voltage_canlib_payload },
    [TASK_SEND_CELLBOARD6_VOLTAGE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE, .get_payload = volt_api_get_cellboard6_voltage_canlib_payload },

    // TEMPERATURES
    [TASK_SEND_TEMPERATURE_INFO] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDTEMPERATUREINFO, .get_payload = temp_api_get_cells_temperature_info_canlib_payload },
    [TASK_SEND_CELLBOARD1_TEMPERATURE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE, .get_payload = temp_api_get_cellboard1_temperature_canlib_payload },
    [TASK_SEND_CELLBOARD2_TEMPERATURE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE, .get_payload = temp_api_get_cellboard2_temperature_canlib_payload },
    [TASK_SEND_CELLBOARD3_TEMPERATURE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE, .get_payload = temp_api_get_cellboard3_temperature_canlib_payload },
    [TASK_SEND_CELLBOARD4_TEMPERATURE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE, .get_payload = temp_api_get_cellboard4_temperature_canlib_payload },
    [TASK_SEND_CELLBOARD5_TEMPERATURE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE, .get_payload = temp_api_get_cellboard5_temperature_canlib_payload },
    [TASK_SEND_CELLBOARD6_TEMPERATURE] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE, .get_payload = temp_api_get_cellboard6_temperature_canlib_payload },
    [TASK_SEND_COOLING_TEMPERATURE1] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCOOLINGTEMPERATURE1, .get_payload = cooling_temp_get_cooling1_canlib_payload },
    [TASK_SEND_COOLING_TEMPERATURE2] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCOOLINGTEMPERATURE2, .get_payload = cooling_temp_get_cooling2_canlib_payload },

    // BALANCING
    [TASK_SEND_CELLBOARD1_BALANCING] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING, .get_payload = bal_api_get_cellboard1_canlib_payload },
    [TASK_SEND_CELLBOARD2_BALANCING] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING, .get_payload = bal_api_get_cellboard2_canlib_payload },
    [TASK_SEND_CELLBOARD3_BALANCING] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING, .get_payload = bal_api_get_cellboard3_canlib_payload },
    [TASK_SEND_CELLBOARD4_BALANCING] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING, .get_payload = bal_api_get_cellboard4_canlib_payload },
    [TASK_SEND_CELLBOARD5_BALANCING] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING, .get_payload = bal_api_get_cellboard5_canlib_payload },
    [TASK_SEND_CELLBOARD6_BALANCING] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING, .get_payload = bal_api_get_cellboard6_canlib_payload },

    // FEEDBACKS / SHUTDOWN
    [TASK_SEND_FEEDBACK] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDFEEDBACK, .get_payload = feedback_api_get_feedaback_payload },
    [TASK_SEND_SHUTDOWN] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDSHUTDOWN, .get_payload = feedback_api_get_shutdown_payload },
    [TASK_SEND_FEEDBACK_SHUTDOWN] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDFEEDBACKSHUTDOWN, .get_payload = feedback_api_get_feedback_shutdown_payload },

    // ERRORS
    [TASK_SEND_ERRORS] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDERROR, .get_payload = error_api_get_canlib_payload },
    [TASK_SEND_CELLBOARD_ERRORS_A] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARDERRORA, .get_payload = error_api_get_cellboard_a_canlib_payload },
    [TASK_SEND_CELLBOARD_ERRORS_B] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARDERRORB, .get_payload = error_api_get_cellboard_b_canlib_payload },
    [TASK_SEND_CELLBOARD_ERRORS_C] = { .frame_id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARDERRORC, .get_payload = error_api_get_cellboard_c_canlib_payload },
};

void prv_tasks_primary_enqueue(enum CanPrimaryMessageFrameId frame_id, union CanPrimaryMessages *message) {
    struct CanCommunicationFrame frame = {
        .id = frame_id
    };

    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    if (byte_size >= 0) {
        frame.length = byte_size;
        if (!mainboard_tasks_handler.suppress_primary_messages || frame.id == CAN_PRIMARY_MESSAGE_FRAME_ID_TSACSTATUS) {
            can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame);
        }
    }
}

void prv_tasks_send_to_primary(uint8_t task_id) {
    // Only the first tasks are CAN messages, the other ones are not in the table
    if (task_id >= sizeof(can_msgs) / sizeof(can_msgs[0])) {
        return;
    }

    const struct TaskCanPrimaryMsg msg = can_msgs[task_id];
    if (msg.get_payload == NULL) {
        return;
    }
    union CanPrimaryMessages *payload = msg.get_payload(NULL);
    prv_tasks_primary_enqueue(msg.frame_id, payload);
}

void prv_tasks_bms_enqueue(enum CanBmsMessageFrameId frame_id, union CanBmsMessages *message) {
    struct CanCommunicationFrame frame = {
        .id = frame_id
    };

    int byte_size = can_bms_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    if (byte_size >= 0) {
        frame.length = byte_size;
        can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_BMS, &frame);
    }
}

/*! \brief Send the set balancing status command via CAN */
void prv_tasks_send_cellboard_set_balancing_status(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    prv_tasks_bms_enqueue(CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDBALANCINGSET, bal_api_get_balancing_set_canlib_payload(NULL));
}

/*! \brief Update all the digital feedbacks */
void prv_tasks_read_digital_feedbacks(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    (void)feedback_api_update_digital_feedback_all();
}

/*! \brief Start the conversion of all the analog feedbacks */
void prv_tasks_start_analog_conversion_feedbacks(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    (void)feedback_api_start_analog_conversion_all();
}

/*! \brief Update all the feedbacks status */
void prv_tasks_update_feedbacks_status(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    (void)feedback_api_update_status();
}

/*! \brief Update all the feedbacks status */
void prv_tasks_check_temperature(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    temp_api_check_temperature();
}

/*! \brief Start the internal voltages ADC conversion */
void prv_tasks_start_internal_voltage_conversion(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    (void)internal_voltage_api_read_all();
}

EAGLETRT_STATIC TaskList task_list = {

    // STATUS SEND TASKS

    {
        .task_id = TASK_SEND_STATUS,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacstatus,
    },
    {
        .task_id = TASK_SEND_MAINBOARD_VERSION,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardversion,
    },
    {
        .task_id = TASK_SEND_MAINBOARD_VERSION_INFO,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardversioninfo,
    },
    {
        .task_id = TASK_SEND_MAINBOARD_LIBCAN_VERSION,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardlibcanversion,
    },
    {
        .task_id = TASK_SEND_MAINBOARD_LIBCAN_VERSION_INFO,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardlibcanversioninfo,
    },

    // HV INFO SEND TASKS

    {
        .task_id = TASK_SEND_CURRENT,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardcurrentinfo,
    },
    {
        .task_id = TASK_SEND_VOLTAGE_INFO,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardvoltageinfo,
    },
    {
        .task_id = TASK_SEND_CELLBOARD1_VOLTAGE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard1voltage,
    },
    {
        .task_id = TASK_SEND_CELLBOARD2_VOLTAGE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard2voltage,
    },
    {
        .task_id = TASK_SEND_CELLBOARD3_VOLTAGE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard3voltage,
    },
    {
        .task_id = TASK_SEND_CELLBOARD4_VOLTAGE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard4voltage,
    },
    {
        .task_id = TASK_SEND_CELLBOARD5_VOLTAGE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard5voltage,
    },
    {
        .task_id = TASK_SEND_CELLBOARD6_VOLTAGE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard6voltage,
    },

    // TEMPERATURE INFO SEND TASKS

    {
        .task_id = TASK_SEND_TEMPERATURE_INFO,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardtemperatureinfo,
    },
    {
        .task_id = TASK_SEND_CELLBOARD1_TEMPERATURE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard1temperature,
    },
    {
        .task_id = TASK_SEND_CELLBOARD2_TEMPERATURE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard2temperature,
    },
    {
        .task_id = TASK_SEND_CELLBOARD3_TEMPERATURE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard3temperature,
    },
    {
        .task_id = TASK_SEND_CELLBOARD4_TEMPERATURE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard4temperature,
    },
    {
        .task_id = TASK_SEND_CELLBOARD5_TEMPERATURE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard5temperature,
    },
    {
        .task_id = TASK_SEND_CELLBOARD6_TEMPERATURE,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard6temperature,
    },
    {
        .task_id = TASK_SEND_COOLING_TEMPERATURE1,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardcoolingtemperature1,
    },
    {
        .task_id = TASK_SEND_COOLING_TEMPERATURE2,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardcoolingtemperature2,
    },
    {
        .task_id = TASK_SEND_CELLBOARD1_BALANCING,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard1balancing,
    },
    {
        .task_id = TASK_SEND_CELLBOARD2_BALANCING,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard2balancing,
    },
    {
        .task_id = TASK_SEND_CELLBOARD3_BALANCING,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard3balancing,
    },
    {
        .task_id = TASK_SEND_CELLBOARD4_BALANCING,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard4balancing,
    },
    {
        .task_id = TASK_SEND_CELLBOARD5_BALANCING,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard5balancing,
    },
    {
        .task_id = TASK_SEND_CELLBOARD6_BALANCING,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboard6balancing,
    },

    // BMS STATUS SEND TASKS

    {
        .task_id = TASK_SEND_IMD_STATUS,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardimd,
    },
    {
        .task_id = TASK_SEND_FEEDBACK,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardfeedback,
    },
    {
        .task_id = TASK_SEND_SHUTDOWN,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardshutdown,
    },
    {
        .task_id = TASK_SEND_FEEDBACK_SHUTDOWN,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboardfeedbackshutdown,
    },
    {
        .task_id = TASK_SEND_ERRORS,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsacmainboarderror,
    },
    {
        .task_id = TASK_SEND_CELLBOARD_ERRORS_A,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboarderrora,
    },
    {
        .task_id = TASK_SEND_CELLBOARD_ERRORS_B,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboarderrorb,
    },
    {
        .task_id = TASK_SEND_CELLBOARD_ERRORS_C,
        .function = prv_tasks_send_to_primary,
        .interval = can_primary_cycle_time_tsaccellboarderrorc,
    },
    {
        .task_id = TASK_SEND_CELLBOARD_SET_BALANCING_STATUS,
        .function = prv_tasks_send_cellboard_set_balancing_status,
        .interval = can_bms_cycle_time_tsacmainboardbalancingset,
    },

    // ROUTINE TASKS

    {
        .task_id = TASK_READ_DIGITAL_FEEDBACKS,
        .function = prv_tasks_read_digital_feedbacks,
        .interval = FEEDBACK_CYCLE_TIME_MS,
    },
    {
        .task_id = TASK_START_ANALOG_CONVERSION_FEEDBACKS,
        .function = prv_tasks_start_analog_conversion_feedbacks,
        .interval = FEEDBACK_CYCLE_TIME_MS,
    },
    {
        .task_id = TASK_UPDATE_FEEDBACKS_STATUS,
        .function = prv_tasks_update_feedbacks_status,
        .interval = FEEDBACK_CYCLE_TIME_MS,
    },
    {
        .task_id = TASK_CHECK_TEMPERATURE,
        .function = prv_tasks_check_temperature,
        .interval = TEMP_CHECK_PERIOD_MS,
    },
    {
        .task_id = TASK_START_INTERNAL_VOLTAGE_CONVERSION,
        .function = prv_tasks_start_internal_voltage_conversion,
        .interval = INTERNAL_VOLTAGE_CYCLE_TIME_MS,
    },
};

static_assert(TASKS_COUNT <= MAX_TASKS, "The number of mainboard tasks exceeds the maximum number of tasks of the scheduler");

/*!
 * \brief Convert a return code of the tasks library to the mainboard one
 * \param rc The return code of the tasks library
 * \return The corresponding mainboard return code
 */
EAGLETRT_STATIC enum MainboardTasksReturnCode prv_tasks_convert_rc(const enum TasksReturnCode rc) {
    switch (rc) {
        case TASKS_RC_OK:
            return MAINBOARD_TASKS_RC_OK;
        case TASKS_RC_INVALID_ID:
            return MAINBOARD_TASKS_RC_INVALID_ID;
        default:
            return MAINBOARD_TASKS_RC_ERROR;
    }
}

enum MainboardTasksReturnCode mainboard_tasks_init(task_tick_callback tick_callback) {

    // Enable and stagger all tasks by default

    uint16_t stagger = 0;

    for (int i = 0; i < TASKS_COUNT; i++) {
        task_list[i].state = TASKS_STATE_ENABLED;
        task_list[i].repeats = 0; // Infinite repeats
        task_list[i].start = stagger;
        stagger += 3; // Stagger by 3ms for each task
    }

    memset(&mainboard_tasks_handler, 0, sizeof(mainboard_tasks_handler));

    mainboard_tasks_handler.suppress_primary_messages = false;

    enum TasksReturnCode rc = tasks_api_init(&tasks_handler, task_list, TASKS_COUNT, tick_callback);

    if (rc != TASKS_RC_OK) {
        return MAINBOARD_TASKS_RC_ERROR;
    }

    return MAINBOARD_TASKS_RC_OK;
}

enum MainboardTasksReturnCode mainboard_tasks_routine(void) {
    enum TasksReturnCode rc = tasks_api_routine(&tasks_handler);

    if (rc != TASKS_RC_OK) {
        return MAINBOARD_TASKS_RC_ERROR;
    }

    return MAINBOARD_TASKS_RC_OK;
}

enum MainboardTasksReturnCode mainboard_tasks_enable_task(uint8_t task_id) {
    return prv_tasks_convert_rc(tasks_api_enable_task(&tasks_handler, task_id));
}

enum MainboardTasksReturnCode mainboard_tasks_disable_task(uint8_t task_id) {
    return prv_tasks_convert_rc(tasks_api_disable_task(&tasks_handler, task_id));
}

enum MainboardTasksReturnCode mainboard_tasks_enable_all_tasks(void) {
    for (int i = 0; i < TASKS_COUNT; i++) {
        const enum MainboardTasksReturnCode rc = prv_tasks_convert_rc(tasks_api_enable_task(&tasks_handler, i));

        if (rc != MAINBOARD_TASKS_RC_OK) {
            return rc;
        }
    }

    return MAINBOARD_TASKS_RC_OK;
}

enum MainboardTasksReturnCode mainboard_tasks_suppress_primary_messages(bool suppress) {
    mainboard_tasks_handler.suppress_primary_messages = suppress;
    return MAINBOARD_TASKS_RC_OK;
}

#endif // CONF_TASKS_MODULE_ENABLE