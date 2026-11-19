/*!
 * \file phc-api.c
 * \date 2026-11-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Health Controller (PHC): checks the values against their limits and
 *        sets the faults and conditions in the mainboard status
 */

#include "phc-api.h"

#include "bal-control-api.h"
#include "cellboards-monitor-api.h"
#include "cellboards-monitor.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "feedback-api.h"
#include "internal-temperatures-api.h"
#include "internal-temperatures.h"
#include "internal-voltages-api.h"
#include "mainboard-conf.h"
#include "mainboard-def.h"
#include "mainboard-status-api.h"
#include "mainboard-status.h"

#include "voltages-api.h"
#include "temperatures-api.h"
#include <stdint.h>
#include <string.h>

EAGLETRT_STATIC struct PHCHandler phc_handler;

EAGLETRT_STATIC bool prv_phc_check_cell_under_voltage(void) {
    if (voltages_api_get_pack_info().min <= PHC_MIN_CELL_VOLTAGE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_cell_over_voltage(void) {
    if (voltages_api_get_pack_info().max >= PHC_MAX_CELL_VOLTAGE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_cell_over_temperature(void) {
    if (temperatures_api_get_pack_cell_temperatures_info().max >= PHC_MAX_CELL_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_cell_under_temperature(void) {
    if (temperatures_api_get_pack_cell_temperatures_info().min <= PHC_MIN_CELL_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_discharge_over_temperature(void) {
    if (temperatures_api_get_pack_discharge_temperatures_info().max >= PHC_MAX_DISCHARGE_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_discharge_under_temperature(void) {
    if (temperatures_api_get_pack_discharge_temperatures_info().min <= PHC_MIN_DISCHARGE_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_cooling_over_temperature(void) {
    for (enum InternalTemperaturesIndex index = INTERNAL_TEMPERATURES_INDEX_COOLING_INLET; index < INTERNAL_TEMPERATURES_INDEX_COUNT; index++) {
        if (internal_temperatures_api_get_value(index) >= PHC_MAX_COOLING_TEMPERATURE) {
            EAGLETRT_API_NOP();
            return false;
        }
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR) >= PHC_MAX_LINEAR_REGULATOR_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_PRECHARGE) >= PHC_MAX_PRECHARGE_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_cooling_under_temperature(void) {
    for (enum InternalTemperaturesIndex index = INTERNAL_TEMPERATURES_INDEX_COOLING_INLET; index < INTERNAL_TEMPERATURES_INDEX_COUNT; index++) {
        if (internal_temperatures_api_get_value(index) <= PHC_MIN_COOLING_TEMPERATURE) {
            EAGLETRT_API_NOP();
            return false;
        }
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR) <= PHC_MIN_LINEAR_REGULATOR_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_PRECHARGE) <= PHC_MIN_PRECHARGE_TEMPERATURE) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_over_current(void) {
    if (internal_voltages_api_get_current() >= PHC_MAX_CURRENT) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_over_power(void) {
    if (internal_voltages_api_get_power() >= PHC_MAX_POWER) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_cellboard_fault(void) {

    bool all_ok = true;

    for (uint8_t i = 0; i < CELLBOARD_COUNT; i++) {
        uint32_t errors = cellboards_monitor_api_get_errors(i);
        if (errors != 0) {
            all_ok = false;
        }

        uint8_t err = 0;

        while (errors != 0) {
            if (errors & 0x1) {
                EAGLETRT_API_NOP();
            }
            errors >>= 1;
            err++;
        }

        uint32_t open_wires = cellboards_monitor_api_get_open_wires(i);
        if (open_wires != 0) {
            all_ok = false;
        }

        uint8_t open_wire_indx = 0;

        while (open_wires != 0) {
            if (open_wires & 0x1) {
                EAGLETRT_API_NOP();
            }
            open_wires >>= 1;
            open_wire_indx++;
        }

        if (cellboards_monitor_api_get_state(i) == CELLBOARDS_MONITOR_STATE_ERROR) {
            all_ok = false;
        }
    }

    return all_ok;
}

EAGLETRT_STATIC bool prv_phc_check_imd_fault(void) {
    // The conditions are meaningful only once the feedback filters are full
    if (!feedback_api_is_ready()) {
        return true;
    }
    if (!mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_IMD_OK)) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_voltage_indicator_disconnected(void) {
    // The conditions are meaningful only once the feedback filters are full
    if (!feedback_api_is_ready()) {
        return true;
    }
    if (!mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_VOLTAGE_INDICATOR_CONNECTED)) {
        EAGLETRT_API_NOP();
        return false;
    }
    return true;
}

EAGLETRT_STATIC bool prv_phc_check_pack_balanced(void) {
    return bal_control_api_is_pack_balanced();
}

#define PHC_FAULT_CHECK(FAULT, FUNCTION) \
    { .kind = PHC_CHECK_KIND_FAULT, .fault = (FAULT), .check_function = (FUNCTION) }
#define PHC_CONDITION_CHECK(CONDITION, FUNCTION) \
    { .kind = PHC_CHECK_KIND_CONDITION, .condition = (CONDITION), .check_function = (FUNCTION) }

EAGLETRT_STATIC const struct PHCCheck phc_checks[PHC_CONTROL_COUNT] = {
    [PHC_CONTROL_CELL_UNDER_VOLTAGE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_CELL_UNDER_VOLTAGE, prv_phc_check_cell_under_voltage),
    [PHC_CONTROL_CELL_OVER_VOLTAGE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_CELL_OVER_VOLTAGE, prv_phc_check_cell_over_voltage),
    [PHC_CONTROL_CELL_UNDER_TEMPERATURE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_CELL_UNDER_TEMPERATURE, prv_phc_check_cell_under_temperature),
    [PHC_CONTROL_CELL_OVER_TEMPERATURE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_CELL_OVER_TEMPERATURE, prv_phc_check_cell_over_temperature),
    [PHC_CONTROL_DISCHARGE_UNDER_TEMPERATURE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_DISCHARGE_UNDER_TEMPERATURE, prv_phc_check_discharge_under_temperature),
    [PHC_CONTROL_DISCHARGE_OVER_TEMPERATURE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_DISCHARGE_OVER_TEMPERATURE, prv_phc_check_discharge_over_temperature),
    [PHC_CONTROL_OVER_CURRENT] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_OVER_CURRENT, prv_phc_check_over_current),
    [PHC_CONTROL_OVER_POWER] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_OVER_POWER, prv_phc_check_over_power),
    [PHC_CONTROL_COOLING_UNDER_TEMPERATURE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_COOLING_UNDER_TEMPERATURE, prv_phc_check_cooling_under_temperature),
    [PHC_CONTROL_COOLING_OVER_TEMPERATURE] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_COOLING_OVER_TEMPERATURE, prv_phc_check_cooling_over_temperature),
    [PHC_CONTROL_CELLBOARD_FAULT] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_CELLBOARD_FAULT, prv_phc_check_cellboard_fault),
    [PHC_CONTROL_IMD_FAULT] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_IMD_FAULT, prv_phc_check_imd_fault),
    [PHC_CONTROL_VOLTAGE_INDICATOR_DISCONNECTED] = PHC_FAULT_CHECK(MAINBOARD_STATUS_FAULT_VOLTAGE_INDICATOR_DISCONNECTED, prv_phc_check_voltage_indicator_disconnected),
    [PHC_CONTROL_PACK_BALANCED] = PHC_CONDITION_CHECK(MAINBOARD_STATUS_CONDITION_PACK_BALANCED, prv_phc_check_pack_balanced),
};

#undef PHC_FAULT_CHECK
#undef PHC_CONDITION_CHECK

enum PHCReturnCode phc_api_init(void) {
    memset(&phc_handler, 0, sizeof(phc_handler));
    return PHC_RC_OK;
}

enum PHCReturnCode phc_api_check(enum PHCControl control) {
    if (control >= PHC_CONTROL_COUNT) {
        return PHC_RC_ERROR;
    }

    const struct PHCCheck *check = &phc_checks[control];

    const bool check_result = check->check_function();

    if (check->kind == PHC_CHECK_KIND_CONDITION) {
        mainboard_status_api_set_condition(check->condition, check_result);
    } else if (!check_result) {
        mainboard_status_api_set_fault(check->fault);
    }

    phc_handler.last_check_time[control] = pcu_api_get_tick();
    phc_handler.last_check_updated[control] = true;

    return PHC_RC_OK;
}

enum PHCReturnCode phc_api_check_phc(void) {
    for (enum PHCControl control = 0; control < PHC_CONTROL_COUNT; ++control) {
        if (phc_handler.last_check_updated[control]) {
            if ((pcu_api_get_tick() - phc_handler.last_check_time[control]) > PHC_MAX_CHECK_INTERVAL_MS) {
                mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_PHC_FAULT);
            }
        } else if (pcu_api_get_tick() > PHC_MAX_STARTUP_INTERVAL_MS) {
            mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_PHC_FAULT);
        }
    }
    return PHC_RC_OK;
}