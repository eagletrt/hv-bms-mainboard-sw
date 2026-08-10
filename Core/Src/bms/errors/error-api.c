/*!
 * \file error.c
 * \date 2024-07-12
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Simple wrapper for the error handler generated code
 */

#include "error-api.h"

#include <stdint.h>
#include <string.h>
#include "can-primary.h"
#include "can-bms.h"
#include "eagletrt.h"
#include "error.h"
#include "mainboard-def.h"
#include "can-communication.h"

#ifdef CONF_ERROR_MODULE_ENABLE

EAGLETRT_STATIC ErrorLibHandler herror;
EAGLETRT_STATIC union CanPrimaryMessages libcan_message_error;

EAGLETRT_STATIC union CanPrimaryMessages libcan_message_cellboard_errors[3];

/*! \brief Total number of instances for each group */
const size_t instances[] = {
    [ERROR_GROUP_POST] = ERROR_POST_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_CURRENT] = ERROR_OVER_CURRENT_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_POWER] = ERROR_OVER_POWER_INSTANCE_COUNT,
    [ERROR_GROUP_UNDER_VOLTAGE] = ERROR_UNDER_VOLTAGE_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_VOLTAGE] = ERROR_OVER_VOLTAGE_INSTANCE_COUNT,
    [ERROR_GROUP_UNDER_TEMPERATURE] = ERROR_UNDER_TEMPERATURE_INSTANCE_COUNT,
    [ERROR_GROUP_OVER_TEMPERATURE] = ERROR_OVER_TEMPERATURE_INSTANCE_COUNT,
    [ERROR_GROUP_CAN_COMMUNICATION] = ERROR_CAN_COMMUNICATION_INSTANCE_COUNT,
    [ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION] = ERROR_CURRENT_SENSOR_COMMUNICATION_INSTANCE_COUNT,
    [ERROR_GROUP_COOLING_UNDER_TEMPERATURE] = ERROR_COOLING_UNDER_TEMPERATURE_INSTANCE_COUNT,
    [ERROR_GROUP_COOLING_OVER_TEMPERATURE] = ERROR_COOLING_OVER_TEMPERATURE_INSTANCE_COUNT,
    [ERROR_GROUP_CELLBOARD_ERROR] = ERROR_CELLBOARD_ERROR_INSTANCE_COUNT,
    [ERROR_GROUP_CONNECTOR_DISCONNECTED] = ERROR_CONNECTOR_DISCONNECTED_ERROR_INSTANCE_COUNT,
    [ERROR_GROUP_IMD] = ERROR_IMD_ERROR_INSTANCE_COUNT
};

/*!
 * \brief Error thresholds for each group
 *
 * \details The values are arbitrary and shuold not be too much high
 */
const size_t thresholds[] = {
    [ERROR_GROUP_POST] = 1U,
    [ERROR_GROUP_OVER_CURRENT] = 2U,
    [ERROR_GROUP_OVER_POWER] = 2U,
    [ERROR_GROUP_UNDER_VOLTAGE] = 3U,
    [ERROR_GROUP_OVER_VOLTAGE] = 3U,
    [ERROR_GROUP_UNDER_TEMPERATURE] = 5U,
    [ERROR_GROUP_OVER_TEMPERATURE] = 5U,
    [ERROR_GROUP_CAN_COMMUNICATION] = 50U,
    [ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION] = 1U,
    [ERROR_GROUP_COOLING_UNDER_TEMPERATURE] = 5U,
    [ERROR_GROUP_COOLING_OVER_TEMPERATURE] = 5U,
    [ERROR_GROUP_CELLBOARD_ERROR] = 2U,
    [ERROR_GROUP_CONNECTOR_DISCONNECTED] = 1U,
    [ERROR_GROUP_IMD] = 1U
};

int32_t error_post_instances[ERROR_POST_INSTANCE_COUNT];
int32_t error_over_current_instances[ERROR_OVER_CURRENT_INSTANCE_COUNT];
int32_t error_over_power_instances[ERROR_OVER_POWER_INSTANCE_COUNT];
int32_t error_under_voltage_instances[ERROR_UNDER_VOLTAGE_INSTANCE_COUNT];
int32_t error_over_voltage_instances[ERROR_OVER_VOLTAGE_INSTANCE_COUNT];
int32_t error_under_temperature_instances[ERROR_UNDER_TEMPERATURE_INSTANCE_COUNT];
int32_t error_over_temperature_instances[ERROR_OVER_TEMPERATURE_INSTANCE_COUNT];
int32_t error_can_communication_instances[ERROR_CAN_COMMUNICATION_INSTANCE_COUNT];
int32_t error_current_sensor_communication_instances[ERROR_CURRENT_SENSOR_COMMUNICATION_INSTANCE_COUNT];
int32_t error_cooling_under_temperature_instances[ERROR_COOLING_UNDER_TEMPERATURE_INSTANCE_COUNT];
int32_t error_cooling_over_temperature_instances[ERROR_COOLING_OVER_TEMPERATURE_INSTANCE_COUNT];
int32_t error_cellboard_error_instances[ERROR_CELLBOARD_ERROR_INSTANCE_COUNT];
int32_t error_connector_disconnected_error_instances[ERROR_CONNECTOR_DISCONNECTED_ERROR_INSTANCE_COUNT];
int32_t error_imd_error_instances[ERROR_IMD_ERROR_INSTANCE_COUNT];
int32_t *error[] = {
    [ERROR_GROUP_POST] = error_post_instances,
    [ERROR_GROUP_OVER_CURRENT] = error_over_current_instances,
    [ERROR_GROUP_OVER_POWER] = error_over_power_instances,
    [ERROR_GROUP_UNDER_VOLTAGE] = error_under_voltage_instances,
    [ERROR_GROUP_OVER_VOLTAGE] = error_over_voltage_instances,
    [ERROR_GROUP_UNDER_TEMPERATURE] = error_under_temperature_instances,
    [ERROR_GROUP_OVER_TEMPERATURE] = error_over_temperature_instances,
    [ERROR_GROUP_CAN_COMMUNICATION] = error_can_communication_instances,
    [ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION] = error_current_sensor_communication_instances,
    [ERROR_GROUP_COOLING_UNDER_TEMPERATURE] = error_cooling_under_temperature_instances,
    [ERROR_GROUP_COOLING_OVER_TEMPERATURE] = error_cooling_over_temperature_instances,
    [ERROR_GROUP_CELLBOARD_ERROR] = error_cellboard_error_instances,
    [ERROR_GROUP_CONNECTOR_DISCONNECTED] = error_connector_disconnected_error_instances,
    [ERROR_GROUP_IMD] = error_imd_error_instances
};

enum ErrorReturnCode error_api_init(void) {
    if (errorlib_init(&herror,
                      error,
                      instances,
                      thresholds,
                      ERROR_GROUP_COUNT) != ERRORLIB_OK) {
        return ERROR_RC_UNKNOWN;
    }
    return ERROR_RC_OK;
}

enum ErrorReturnCode error_api_set(const enum ErrorGroup group, const error_instance instance) {
    ErrorLibReturnCode ret = errorlib_error_set(&herror, (errorlib_error_group_t)group, instance);
    return ret != ERRORLIB_OK ? ERROR_RC_UNKNOWN : ERROR_RC_OK;
}

enum ErrorReturnCode error_api_reset(const enum ErrorGroup group, const error_instance instance) {
    if (errorlib_error_reset(&herror, (errorlib_error_group_t)group, instance) != ERRORLIB_OK) {
        return ERROR_RC_UNKNOWN;
    }
    return ERROR_RC_OK;
}

size_t error_api_get_expired(void) {
    return errorlib_get_expired(&herror);
}

ErrorInfo error_api_get_expired_info(void) {
    return errorlib_get_expired_info(&herror);
}

union CanPrimaryMessages *error_api_get_canlib_payload(size_t *byte_size) {
    if (byte_size == NULL) {
        *byte_size = can_primary_byte_size_tsacmainboarderror;
    }
    bool undervoltage = false;
    bool overvoltage = false;
    for (uint16_t i = 0; i < CELLBOARD_SERIES_COUNT; ++i) {
        undervoltage = undervoltage || error_under_voltage_instances[i];
        overvoltage = overvoltage || error_over_voltage_instances[i];
    }
    bool undertemperature = false;
    bool overtemperature = false;
    for (uint16_t i = 0; i < CELLBOARD_TEMP_SENSOR_COUNT; ++i) {
        undertemperature = undertemperature || error_under_temperature_instances[i];
        overtemperature = overtemperature || error_over_temperature_instances[i];
    }
    bool cooling_undertemperature = false;
    bool cooling_overtemperature = false;
    for (uint16_t i = 0; i < CELLBOARD_TEMP_SENSOR_COUNT; ++i) {
        cooling_undertemperature = cooling_undertemperature || error_cooling_under_temperature_instances[i];
        cooling_overtemperature = cooling_overtemperature || error_cooling_over_temperature_instances[i];
    }
    bool internal = false;
    for (CellboardId cellboard = 0; cellboard < CELLBOARD_ID_COUNT; ++cellboard) {
        internal = internal || error_cellboard_error_instances[cellboard];
    }

    struct CanPrimaryTsacmainboarderror *payload = &libcan_message_error.tsacmainboarderror;
    payload->post = error_post_instances[0];
    payload->overcurrent = error_over_current_instances[0];
    payload->overpower = error_over_power_instances[0];
    payload->undervoltage = undervoltage;
    payload->overvoltage = overvoltage;
    payload->undertemperature = undertemperature;
    payload->overtemperature = overtemperature;
    payload->cancommunication = error_can_communication_instances[CAN_COMMUNICATION_NETWORK_BMS];
    payload->currentsensorcommunication = error_current_sensor_communication_instances[0];
    payload->coolingundertemperature = cooling_undertemperature;
    payload->coolingovertemperature = cooling_overtemperature;
    payload->cellboard = internal;
    payload->connectordisconnected = error_connector_disconnected_error_instances[0];
    payload->imd = error_imd_error_instances[0];
    return &libcan_message_error;
}

union CanPrimaryMessages *error_api_get_cellboard_a_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboarderrora;
    }
    return &libcan_message_cellboard_errors[0];
}

union CanPrimaryMessages *error_api_get_cellboard_b_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboarderrorb;
    }
    return &libcan_message_cellboard_errors[1];
}

union CanPrimaryMessages *error_api_get_cellboard_c_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboarderrorc;
    }
    return &libcan_message_cellboard_errors[2];
}

void error_api_cellboard_handle(
    CellboardId cellboard,
    uint8_t post,
    uint8_t undervoltage,
    uint8_t overvoltage,
    uint8_t undertemperature,
    uint8_t overtemperature,
    uint8_t discharge_undertemperature,
    uint8_t discharge_overtemperature,
    uint8_t can_communication,
    uint8_t flash,
    uint8_t bms_monitor_communication,
    uint8_t openwire1,
    uint8_t openwire2,
    uint8_t openwire3,
    uint8_t openwire4,
    uint8_t openwire5,
    uint8_t openwire6,
    uint8_t openwire7,
    uint8_t openwire8,
    uint8_t openwire9,
    uint8_t openwire10,
    uint8_t openwire11,
    uint8_t openwire12,
    uint8_t openwire13) {
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return;
    }

    const uint8_t cellboard_error = post | undervoltage | overvoltage | undertemperature | overtemperature | discharge_undertemperature | discharge_overtemperature | can_communication | flash | bms_monitor_communication | openwire1 | openwire2 | openwire3 | openwire4 | openwire5 | openwire6 | openwire7 | openwire8 | openwire9 | openwire10 | openwire11 | openwire12 | openwire13;
    if (cellboard_error > 0) {
        error_api_set(ERROR_GROUP_CELLBOARD_ERROR, cellboard);
    } else {
        error_api_reset(ERROR_GROUP_CELLBOARD_ERROR, cellboard);
    }

    union CanPrimaryMessages *payload = &libcan_message_cellboard_errors[cellboard / 2];
    switch (cellboard) {
        case CELLBOARD_ID_0:
            payload->tsaccellboarderrora.cellboard1post = post;
            payload->tsaccellboarderrora.cellboard1cellundervoltage = undervoltage;
            payload->tsaccellboarderrora.cellboard1cellovervoltage = overvoltage;
            payload->tsaccellboarderrora.cellboard1cellundertemperature = undertemperature;
            payload->tsaccellboarderrora.cellboard1cellovertemperature = overtemperature;
            payload->tsaccellboarderrora.cellboard1dischargeresistorundertemperature = discharge_undertemperature;
            payload->tsaccellboarderrora.cellboard1dischargeresistorovertemperature = discharge_overtemperature;
            payload->tsaccellboarderrora.cellboard1cancommunication = can_communication;
            payload->tsaccellboarderrora.cellboard1flash = flash;
            payload->tsaccellboarderrora.cellboard1bmsmonitorcommunication = bms_monitor_communication;
            payload->tsaccellboarderrora.cellboard1openwire1 = openwire1;
            payload->tsaccellboarderrora.cellboard1openwire2 = openwire2;
            payload->tsaccellboarderrora.cellboard1openwire3 = openwire3;
            payload->tsaccellboarderrora.cellboard1openwire4 = openwire4;
            payload->tsaccellboarderrora.cellboard1openwire5 = openwire5;
            payload->tsaccellboarderrora.cellboard1openwire6 = openwire6;
            payload->tsaccellboarderrora.cellboard1openwire7 = openwire7;
            payload->tsaccellboarderrora.cellboard1openwire8 = openwire8;
            payload->tsaccellboarderrora.cellboard1openwire9 = openwire9;
            payload->tsaccellboarderrora.cellboard1openwire10 = openwire10;
            payload->tsaccellboarderrora.cellboard1openwire11 = openwire11;
            payload->tsaccellboarderrora.cellboard1openwire12 = openwire12;
            payload->tsaccellboarderrora.cellboard1openwire13 = openwire13;
            break;
        case CELLBOARD_ID_1:
            payload->tsaccellboarderrora.cellboard2post = post;
            payload->tsaccellboarderrora.cellboard2cellundervoltage = undervoltage;
            payload->tsaccellboarderrora.cellboard2cellovervoltage = overvoltage;
            payload->tsaccellboarderrora.cellboard2cellundertemperature = undertemperature;
            payload->tsaccellboarderrora.cellboard2cellovertemperature = overtemperature;
            payload->tsaccellboarderrora.cellboard2dischargeresistorundertemperature = discharge_undertemperature;
            payload->tsaccellboarderrora.cellboard2dischargeresistorovertemperature = discharge_overtemperature;
            payload->tsaccellboarderrora.cellboard2cancommunication = can_communication;
            payload->tsaccellboarderrora.cellboard2flash = flash;
            payload->tsaccellboarderrora.cellboard2bmsmonitorcommunication = bms_monitor_communication;
            payload->tsaccellboarderrora.cellboard2openwire1 = openwire1;
            payload->tsaccellboarderrora.cellboard2openwire2 = openwire2;
            payload->tsaccellboarderrora.cellboard2openwire3 = openwire3;
            payload->tsaccellboarderrora.cellboard2openwire4 = openwire4;
            payload->tsaccellboarderrora.cellboard2openwire5 = openwire5;
            payload->tsaccellboarderrora.cellboard2openwire6 = openwire6;
            payload->tsaccellboarderrora.cellboard2openwire7 = openwire7;
            payload->tsaccellboarderrora.cellboard2openwire8 = openwire8;
            payload->tsaccellboarderrora.cellboard2openwire9 = openwire9;
            payload->tsaccellboarderrora.cellboard2openwire10 = openwire10;
            payload->tsaccellboarderrora.cellboard2openwire11 = openwire11;
            payload->tsaccellboarderrora.cellboard2openwire12 = openwire12;
            payload->tsaccellboarderrora.cellboard2openwire13 = openwire13;
            break;
        case CELLBOARD_ID_2:
            payload->tsaccellboarderrorb.cellboard3post = post;
            payload->tsaccellboarderrorb.cellboard3cellundervoltage = undervoltage;
            payload->tsaccellboarderrorb.cellboard3cellovervoltage = overvoltage;
            payload->tsaccellboarderrorb.cellboard3cellundertemperature = undertemperature;
            payload->tsaccellboarderrorb.cellboard3cellovertemperature = overtemperature;
            payload->tsaccellboarderrorb.cellboard3dischargeresistorundertemperature = discharge_undertemperature;
            payload->tsaccellboarderrorb.cellboard3dischargeresistorovertemperature = discharge_overtemperature;
            payload->tsaccellboarderrorb.cellboard3cancommunication = can_communication;
            payload->tsaccellboarderrorb.cellboard3flash = flash;
            payload->tsaccellboarderrorb.cellboard3bmsmonitorcommunication = bms_monitor_communication;
            payload->tsaccellboarderrorb.cellboard3openwire1 = openwire1;
            payload->tsaccellboarderrorb.cellboard3openwire2 = openwire2;
            payload->tsaccellboarderrorb.cellboard3openwire3 = openwire3;
            payload->tsaccellboarderrorb.cellboard3openwire4 = openwire4;
            payload->tsaccellboarderrorb.cellboard3openwire5 = openwire5;
            payload->tsaccellboarderrorb.cellboard3openwire6 = openwire6;
            payload->tsaccellboarderrorb.cellboard3openwire7 = openwire7;
            payload->tsaccellboarderrorb.cellboard3openwire8 = openwire8;
            payload->tsaccellboarderrorb.cellboard3openwire9 = openwire9;
            payload->tsaccellboarderrorb.cellboard3openwire10 = openwire10;
            payload->tsaccellboarderrorb.cellboard3openwire11 = openwire11;
            payload->tsaccellboarderrorb.cellboard3openwire12 = openwire12;
            payload->tsaccellboarderrorb.cellboard3openwire13 = openwire13;
            break;
        case CELLBOARD_ID_3:
            payload->tsaccellboarderrorb.cellboard4post = post;
            payload->tsaccellboarderrorb.cellboard4cellundervoltage = undervoltage;
            payload->tsaccellboarderrorb.cellboard4cellovervoltage = overvoltage;
            payload->tsaccellboarderrorb.cellboard4cellundertemperature = undertemperature;
            payload->tsaccellboarderrorb.cellboard4cellovertemperature = overtemperature;
            payload->tsaccellboarderrorb.cellboard4dischargeresistorundertemperature = discharge_undertemperature;
            payload->tsaccellboarderrorb.cellboard4dischargeresistorovertemperature = discharge_overtemperature;
            payload->tsaccellboarderrorb.cellboard4cancommunication = can_communication;
            payload->tsaccellboarderrorb.cellboard4flash = flash;
            payload->tsaccellboarderrorb.cellboard4bmsmonitorcommunication = bms_monitor_communication;
            payload->tsaccellboarderrorb.cellboard4openwire1 = openwire1;
            payload->tsaccellboarderrorb.cellboard4openwire2 = openwire2;
            payload->tsaccellboarderrorb.cellboard4openwire3 = openwire3;
            payload->tsaccellboarderrorb.cellboard4openwire4 = openwire4;
            payload->tsaccellboarderrorb.cellboard4openwire5 = openwire5;
            payload->tsaccellboarderrorb.cellboard4openwire6 = openwire6;
            payload->tsaccellboarderrorb.cellboard4openwire7 = openwire7;
            payload->tsaccellboarderrorb.cellboard4openwire8 = openwire8;
            payload->tsaccellboarderrorb.cellboard4openwire9 = openwire9;
            payload->tsaccellboarderrorb.cellboard4openwire10 = openwire10;
            payload->tsaccellboarderrorb.cellboard4openwire11 = openwire11;
            payload->tsaccellboarderrorb.cellboard4openwire12 = openwire12;
            payload->tsaccellboarderrorb.cellboard4openwire13 = openwire13;
            break;
        case CELLBOARD_ID_4:
            payload->tsaccellboarderrorc.cellboard5post = post;
            payload->tsaccellboarderrorc.cellboard5cellundervoltage = undervoltage;
            payload->tsaccellboarderrorc.cellboard5cellovervoltage = overvoltage;
            payload->tsaccellboarderrorc.cellboard5cellundertemperature = undertemperature;
            payload->tsaccellboarderrorc.cellboard5cellovertemperature = overtemperature;
            payload->tsaccellboarderrorc.cellboard5dischargeresistorundertemperature = discharge_undertemperature;
            payload->tsaccellboarderrorc.cellboard5dischargeresistorovertemperature = discharge_overtemperature;
            payload->tsaccellboarderrorc.cellboard5cancommunication = can_communication;
            payload->tsaccellboarderrorc.cellboard5flash = flash;
            payload->tsaccellboarderrorc.cellboard5bmsmonitorcommunication = bms_monitor_communication;
            payload->tsaccellboarderrorc.cellboard5openwire1 = openwire1;
            payload->tsaccellboarderrorc.cellboard5openwire2 = openwire2;
            payload->tsaccellboarderrorc.cellboard5openwire3 = openwire3;
            payload->tsaccellboarderrorc.cellboard5openwire4 = openwire4;
            payload->tsaccellboarderrorc.cellboard5openwire5 = openwire5;
            payload->tsaccellboarderrorc.cellboard5openwire6 = openwire6;
            payload->tsaccellboarderrorc.cellboard5openwire7 = openwire7;
            payload->tsaccellboarderrorc.cellboard5openwire8 = openwire8;
            payload->tsaccellboarderrorc.cellboard5openwire9 = openwire9;
            payload->tsaccellboarderrorc.cellboard5openwire10 = openwire10;
            payload->tsaccellboarderrorc.cellboard5openwire11 = openwire11;
            payload->tsaccellboarderrorc.cellboard5openwire12 = openwire12;
            payload->tsaccellboarderrorc.cellboard5openwire13 = openwire13;
            break;
        case CELLBOARD_ID_5:
            payload->tsaccellboarderrorc.cellboard6post = post;
            payload->tsaccellboarderrorc.cellboard6cellundervoltage = undervoltage;
            payload->tsaccellboarderrorc.cellboard6cellovervoltage = overvoltage;
            payload->tsaccellboarderrorc.cellboard6cellundertemperature = undertemperature;
            payload->tsaccellboarderrorc.cellboard6cellovertemperature = overtemperature;
            payload->tsaccellboarderrorc.cellboard6dischargeresistorundertemperature = discharge_undertemperature;
            payload->tsaccellboarderrorc.cellboard6dischargeresistorovertemperature = discharge_overtemperature;
            payload->tsaccellboarderrorc.cellboard6cancommunication = can_communication;
            payload->tsaccellboarderrorc.cellboard6flash = flash;
            payload->tsaccellboarderrorc.cellboard6bmsmonitorcommunication = bms_monitor_communication;
            payload->tsaccellboarderrorc.cellboard6openwire1 = openwire1;
            payload->tsaccellboarderrorc.cellboard6openwire2 = openwire2;
            payload->tsaccellboarderrorc.cellboard6openwire3 = openwire3;
            payload->tsaccellboarderrorc.cellboard6openwire4 = openwire4;
            payload->tsaccellboarderrorc.cellboard6openwire5 = openwire5;
            payload->tsaccellboarderrorc.cellboard6openwire6 = openwire6;
            payload->tsaccellboarderrorc.cellboard6openwire7 = openwire7;
            payload->tsaccellboarderrorc.cellboard6openwire8 = openwire8;
            payload->tsaccellboarderrorc.cellboard6openwire9 = openwire9;
            payload->tsaccellboarderrorc.cellboard6openwire10 = openwire10;
            payload->tsaccellboarderrorc.cellboard6openwire11 = openwire11;
            payload->tsaccellboarderrorc.cellboard6openwire12 = openwire12;
            payload->tsaccellboarderrorc.cellboard6openwire13 = openwire13;
            break;
        default:
            break;
    }
}

#ifdef CONF_ERROR_STRINGS_ENABLE

EAGLETRT_STATIC char *error_module_name = "error";

EAGLETRT_STATIC char *error_return_code_name[] = {
    [ERROR_RC_OK] = "ok",
    [ERROR_RC_NULL_POINTER] = "null pointer"
};

EAGLETRT_STATIC char *error_return_code_description[] = {
    [ERROR_RC_OK] = "executed succesfully",
    [ERROR_RC_NULL_POINTER] = "attempt to dereference a null pointer"
};

EAGLETRT_STATIC char *error_group_name[] = {
    [ERROR_GROUP_POST] = "post",
    [ERROR_GROUP_OVER_CURRENT] = "over current",
    [ERROR_GROUP_OVER_POWER] = "over power",
    [ERROR_GROUP_UNDER_VOLTAGE] = "under voltage",
    [ERROR_GROUP_OVER_VOLTAGE] = "over voltage",
    [ERROR_GROUP_UNDER_TEMPERATURE] = "under temperature",
    [ERROR_GROUP_OVER_TEMPERATURE] = "over temperature",
    [ERROR_GROUP_CAN_COMMUNICATION] = "can communication",
    [ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION] = "current sensor communication",
    [ERROR_GROUP_COOLING_UNDER_TEMPERATURE] = "cooling under temperature",
    [ERROR_GROUP_COOLING_OVER_TEMPERATURE] = "cooling over temperature"
};

EAGLETRT_STATIC char *error_get_group_name_string(const enum ErrorGroup group) {
    if (group >= ERROR_GROUP_COUNT)
        return "";
    return error_group_name[group];
}

#endif // CONF_ERROR_STRINGS_ENABLE

#endif // CONF_ERROR_MODULE_ENABLE
