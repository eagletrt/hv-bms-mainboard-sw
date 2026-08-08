/*!
 * \file tasks.c
 * \date 2024-05-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Tasks that have to be executed at a certain interval
 */

#include "tasks.h"

#include <stdint.h>

#include "can-communication.h"
#include "can-communication-api.h"
#include "current-api.h"
#include "eagletrt.h"
#include "eagletrt-api.h"
#include "can-primary-api.h"
#include "can-primary.h"
#include "can-bms-api.h"
#include "can-bms.h"

#include "timebase.h"
#include "feedback-api.h"
#include "volt-api.h"
#include "temp-api.h"
#include "internal-voltage-api.h"
#include "fsm.h"
#include "imd-api.h"
#include "identity-api.h"
#include "error-api.h"
#include "bal-api.h"

#ifdef CONF_TASKS_MODULE_ENABLE

EAGLETRT_STATIC struct TaskHandler task_handler;

/*! \brief Send the mainboard and cellboard FSM status via CAN */
void prv_tasks_send_hv_status(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACSTATUS
    };

    union CanPrimaryMessages *message = fsm_get_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the mainboard version via CAN */
void prv_tasks_send_mainboard_version(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVERSION
    };

    union CanPrimaryMessages *message = identity_api_get_mainboard_version_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the mainboard version info via CAN */
void prv_tasks_send_mainboard_version_info(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVERSIONINFO
    };

    union CanPrimaryMessages *message = identity_api_get_mainboard_version_info_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the mainboard libcan version via CAN */
void prv_tasks_send_mainboard_libcan_version(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDLIBCANVERSION
    };

    union CanPrimaryMessages *message = identity_api_get_mainboard_libcan_version_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the mainboard libcan version info via CAN */
void prv_tasks_send_mainboard_libcan_version_info(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDLIBCANVERSIONINFO
    };

    union CanPrimaryMessages *message = identity_api_get_mainboard_libcan_version_info_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the current via CAN */
void prv_tasks_send_hv_current(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCURRENTINFO
    };

    union CanPrimaryMessages *message = current_api_get_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the Tractive System voltages info via CAN */
void prv_tasks_send_hv_ts_voltage(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVOLTAGEINFO
    };

    union CanPrimaryMessages *message = internal_voltage_api_get_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells voltages via CAN */
void prv_tasks_send_hv_cellboard1_voltage(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE
    };

    union CanPrimaryMessages *message = volt_api_get_cellboard1_voltage_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells voltages via CAN */
void prv_tasks_send_hv_cellboard2_voltage(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE
    };

    union CanPrimaryMessages *message = volt_api_get_cellboard2_voltage_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells voltages via CAN */
void prv_tasks_send_hv_cellboard3_voltage(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE
    };

    union CanPrimaryMessages *message = volt_api_get_cellboard3_voltage_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells voltages via CAN */
void prv_tasks_send_hv_cellboard4_voltage(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE
    };

    union CanPrimaryMessages *message = volt_api_get_cellboard4_voltage_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells voltages via CAN */
void prv_tasks_send_hv_cellboard5_voltage(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE
    };

    union CanPrimaryMessages *message = volt_api_get_cellboard5_voltage_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells voltages via CAN */
void prv_tasks_send_hv_cellboard6_voltage(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE
    };

    union CanPrimaryMessages *message = volt_api_get_cellboard6_voltage_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the BMS cells temperature info via CAN */
void prv_tasks_send_hv_temperature_info(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDTEMPERATUREINFO
    };

    union CanPrimaryMessages *message = temp_api_get_cells_temperature_info_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells temperatures via CAN */
void prv_tasks_send_hv_cellboard1_temperature(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE
    };

    union CanPrimaryMessages *message = temp_api_get_cellboard1_temperature_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells temperatures via CAN */
void prv_tasks_send_hv_cellboard2_temperature(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE
    };

    union CanPrimaryMessages *message = temp_api_get_cellboard2_temperature_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells temperatures via CAN */
void prv_tasks_send_hv_cellboard3_temperature(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE
    };

    union CanPrimaryMessages *message = temp_api_get_cellboard3_temperature_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells temperatures via CAN */
void prv_tasks_send_hv_cellboard4_temperature(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE
    };

    union CanPrimaryMessages *message = temp_api_get_cellboard4_temperature_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells temperatures via CAN */
void prv_tasks_send_hv_cellboard5_temperature(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE
    };

    union CanPrimaryMessages *message = temp_api_get_cellboard5_temperature_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells temperatures via CAN */
void prv_tasks_send_hv_cellboard6_temperature(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE
    };

    union CanPrimaryMessages *message = temp_api_get_cellboard6_temperature_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells balancings via CAN */
void prv_tasks_send_hv_cellboard1_balancing(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING
    };

    union CanPrimaryMessages *message = bal_api_get_cellboard1_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells balancings via CAN */
void prv_tasks_send_hv_cellboard2_balancing(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING
    };

    union CanPrimaryMessages *message = bal_api_get_cellboard2_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells balancings via CAN */
void prv_tasks_send_hv_cellboard3_balancing(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING
    };

    union CanPrimaryMessages *message = bal_api_get_cellboard3_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells balancings via CAN */
void prv_tasks_send_hv_cellboard4_balancing(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING
    };

    union CanPrimaryMessages *message = bal_api_get_cellboard4_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells balancings via CAN */
void prv_tasks_send_hv_cellboard5_balancing(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING
    };

    union CanPrimaryMessages *message = bal_api_get_cellboard5_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the cells balancings via CAN */
void prv_tasks_send_hv_cellboard6_balancing(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING
    };

    union CanPrimaryMessages *message = bal_api_get_cellboard6_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the IMD status via CAN */
void prv_tasks_send_hv_imd_status(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDIMD
    };

    union CanPrimaryMessages *message = imd_api_get_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the feedbacks values via CAN */
void prv_tasks_send_hv_feedback(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDFEEDBACK
    };

    union CanPrimaryMessages *message = feedback_api_get_feedaback_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the shutdown values via CAN */
void prv_tasks_send_hv_shutdown(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDSHUTDOWN
    };

    union CanPrimaryMessages *message = feedback_api_get_shutdown_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the feedbacks shutdown values via CAN */
void prv_tasks_send_hv_feedback_shutdown(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDFEEDBACKSHUTDOWN
    };

    union CanPrimaryMessages *message = feedback_api_get_feedback_shutdown_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the errors status via CAN if an error occoured */
void prv_tasks_send_errors(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDERROR
    };

    union CanPrimaryMessages *message = error_api_get_canlib_payload(NULL);
    int byte_size = can_primary_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
}

/*! \brief Send the set balancing status command via CAN */
void prv_tasks_send_cellboard_set_balancing_status(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDBALANCINGSET
    };

    union CanBmsMessages *message = bal_api_get_balancing_set_canlib_payload(NULL);
    int byte_size = can_bms_api_serialize_from_id(
        frame.id,
        message,
        frame.data);

    // TODO: Notify error?
    if (byte_size >= 0) {
        frame.length = byte_size;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_BMS, &frame));
    }
}

// /*! \brief Send the BSM balancing status via CAN */
// void prv_tasks_send_hv_balancing_status(void) {
//     size_t byte_size = 0U;
//     uint8_t *const payload = (uint8_t *const)bal_api_get_status_canlib_payload(&byte_size);
//     can_comm_tx_add(
//         CAN_NETWORK_PRIMARY,
//         PRIMARY_HV_BALANCING_STATUS_INDEX,
//         CAN_FRAME_TYPE_DATA,
//         payload,
//         byte_size);
// }

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
