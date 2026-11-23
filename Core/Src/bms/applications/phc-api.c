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
#include "internal-voltages.h"
#include "mainboard-conf.h"
#include "mainboard-def.h"
#include "mainboard-status-api.h"
#include "mainboard-status.h"
#include "mainboard-watchdogs-api.h"

#include "phc.h"
#include "voltages-api.h"
#include "temperatures-api.h"
#include "watchdogs.h"

#include <stdint.h>
#include <string.h>

EAGLETRT_STATIC struct PHCHandler phc_handler;

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_cell_under_voltage(void) {
    if (!voltages_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (voltages_api_get_pack_info().min <= PHC_MIN_CELL_VOLTAGE_V) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_cell_over_voltage(void) {
    if (!voltages_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (voltages_api_get_pack_info().max >= PHC_MAX_CELL_VOLTAGE_V) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_cell_over_temperature(void) {
    if (!temperatures_api_is_cell_temperatures_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (temperatures_api_get_pack_cell_temperatures_info().max >= PHC_MAX_CELL_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_cell_under_temperature(void) {
    if (!temperatures_api_is_cell_temperatures_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (temperatures_api_get_pack_cell_temperatures_info().min <= PHC_MIN_CELL_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_discharge_over_temperature(void) {
    if (!temperatures_api_is_discharge_temperatures_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (temperatures_api_get_pack_discharge_temperatures_info().max >= PHC_MAX_DISCHARGE_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_discharge_under_temperature(void) {
    if (!temperatures_api_is_discharge_temperatures_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (temperatures_api_get_pack_discharge_temperatures_info().min <= PHC_MIN_DISCHARGE_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_cooling_over_temperature(void) {
    if (!internal_temperatures_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    for (enum InternalTemperaturesIndex index = INTERNAL_TEMPERATURES_INDEX_COOLING_INLET; index < INTERNAL_TEMPERATURES_INDEX_COUNT; index++) {
        if (internal_temperatures_api_get_value(index) >= PHC_MAX_COOLING_TEMPERATURE_C) {
            EAGLETRT_API_NOP();
            return PHC_CHECK_RESULT_FAILED;
        }
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR) >= PHC_MAX_LINEAR_REGULATOR_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_PRECHARGE) >= PHC_MAX_PRECHARGE_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_cooling_under_temperature(void) {
    if (!internal_temperatures_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    for (enum InternalTemperaturesIndex index = INTERNAL_TEMPERATURES_INDEX_COOLING_INLET; index < INTERNAL_TEMPERATURES_INDEX_COUNT; index++) {
        if (internal_temperatures_api_get_value(index) <= PHC_MIN_COOLING_TEMPERATURE_C) {
            EAGLETRT_API_NOP();
            return PHC_CHECK_RESULT_FAILED;
        }
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR) <= PHC_MIN_LINEAR_REGULATOR_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    if (internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_PRECHARGE) <= PHC_MIN_PRECHARGE_TEMPERATURE_C) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_over_current(void) {
    if (!internal_voltages_api_is_current_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    const ampere current = internal_voltages_api_get_current();
    if (current >= PHC_MAX_CURRENT_A || current <= PHC_MIN_CURRENT_A) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_over_power(void) {
    // The power is the current times the TS voltage, both are needed
    if (!internal_voltages_api_is_current_ready() || !internal_voltages_api_is_voltages_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (internal_voltages_api_get_power() >= PHC_MAX_POWER_KW) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_cellboard_fault(void) {
    // Before every cellboard has reported, the missing ones look like they are initializing with no errors
    if (!cellboards_monitor_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }

    bool all_ok = true;

    for (uint8_t i = 0; i < CELLBOARD_COUNT; i++) {
        uint32_t errors = cellboards_monitor_api_get_errors(i);
        if (errors != 0) {
            all_ok = false;
        }

        uint8_t error_idx = 0;

        while (errors != 0) {
            if (errors & 0x1) {
                EAGLETRT_API_NOP();
            }
            errors >>= 1;
            error_idx++;
        }

        uint32_t open_wires = cellboards_monitor_api_get_open_wires(i);
        if (open_wires != 0) {
            all_ok = false;
        }

        uint8_t open_wire_idx = 0;

        while (open_wires != 0) {
            if (open_wires & 0x1) {
                EAGLETRT_API_NOP();
            }
            open_wires >>= 1;
            open_wire_idx++;
        }

        if (cellboards_monitor_api_get_state(i) == CELLBOARDS_MONITOR_STATE_ERROR) {
            all_ok = false;
        }
    }

    if (!all_ok) {
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_imd_fault(void) {
    // The conditions are meaningful only once the feedback filters are full
    if (!feedback_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (!mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_IMD_OK)) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_voltage_indicator_disconnected(void) {
    // The conditions are meaningful only once the feedback filters are full
    if (!feedback_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (!mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_VOLTAGE_INDICATOR_CONNECTED)) {
        EAGLETRT_API_NOP();
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_pack_balanced(void) {
    // The pack delta is computed from the voltage information of every cellboard
    if (!voltages_api_is_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (!bal_control_api_is_pack_balanced()) {
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC enum PhcCheckResult prv_phc_check_precharge_complete(void) {
    // Before the TS voltage arrives it reads 0V, which must not count as a voltage reading
    if (!internal_voltages_api_is_voltages_ready()) {
        return PHC_CHECK_RESULT_NOT_READY;
    }
    if (internal_voltages_api_get_value(INTERNAL_VOLTAGES_INDEX_TS) < PHC_PRECHARGE_VOLTAGE_V) {
        return PHC_CHECK_RESULT_FAILED;
    }
    return PHC_CHECK_RESULT_OK;
}

EAGLETRT_STATIC const struct PHCCheck phc_checks[] = {
    { PHC_CONTROL_CELL_UNDER_VOLTAGE,             PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_CELL_UNDER_VOLTAGE },             prv_phc_check_cell_under_voltage             },
    { PHC_CONTROL_CELL_OVER_VOLTAGE,              PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_CELL_OVER_VOLTAGE },              prv_phc_check_cell_over_voltage              },
    { PHC_CONTROL_CELL_UNDER_TEMPERATURE,         PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_CELL_UNDER_TEMPERATURE },         prv_phc_check_cell_under_temperature         },
    { PHC_CONTROL_CELL_OVER_TEMPERATURE,          PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_CELL_OVER_TEMPERATURE },          prv_phc_check_cell_over_temperature          },
    { PHC_CONTROL_DISCHARGE_UNDER_TEMPERATURE,    PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_DISCHARGE_UNDER_TEMPERATURE },    prv_phc_check_discharge_under_temperature    },
    { PHC_CONTROL_DISCHARGE_OVER_TEMPERATURE,     PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_DISCHARGE_OVER_TEMPERATURE },     prv_phc_check_discharge_over_temperature     },
    { PHC_CONTROL_OVER_CURRENT,                   PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_OVER_CURRENT },                   prv_phc_check_over_current                   },
    { PHC_CONTROL_OVER_POWER,                     PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_OVER_POWER },                     prv_phc_check_over_power                     },
    { PHC_CONTROL_COOLING_UNDER_TEMPERATURE,      PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_COOLING_UNDER_TEMPERATURE },      prv_phc_check_cooling_under_temperature      },
    { PHC_CONTROL_COOLING_OVER_TEMPERATURE,       PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_COOLING_OVER_TEMPERATURE },       prv_phc_check_cooling_over_temperature       },
    { PHC_CONTROL_CELLBOARD_FAULT,                PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_CELLBOARD_FAULT },                prv_phc_check_cellboard_fault                },
    { PHC_CONTROL_IMD_FAULT,                      PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_IMD_FAULT },                      prv_phc_check_imd_fault                      },
    { PHC_CONTROL_VOLTAGE_INDICATOR_DISCONNECTED, PHC_CHECK_KIND_FAULT,     { .fault = MAINBOARD_STATUS_FAULT_VOLTAGE_INDICATOR_DISCONNECTED }, prv_phc_check_voltage_indicator_disconnected },
    { PHC_CONTROL_PACK_BALANCED,                  PHC_CHECK_KIND_CONDITION, { .condition = MAINBOARD_STATUS_CONDITION_PACK_BALANCED },          prv_phc_check_pack_balanced                  },
    { PHC_CONTROL_PRECHARGE_COMPLETE,             PHC_CHECK_KIND_CONDITION, { .condition = MAINBOARD_STATUS_CONDITION_PRECHARGE_COMPLETE },     prv_phc_check_precharge_complete             },
};

// Every control needs exactly one entry; the entries are looked up by control, so their order does not matter
static_assert(sizeof(phc_checks) / sizeof(phc_checks[0]) == PHC_CONTROL_COUNT, "Every PHC control needs exactly one check");

/*!
 * \brief Find the check of a control in \ref phc_checks
 *
 * \param[in] control The control to look up
 *
 * \return The check of the control, \c NULL if the table has no entry for it
 */
EAGLETRT_STATIC const struct PHCCheck *prv_phc_find_check(enum PHCControl control) {
    for (size_t i = 0U; i < sizeof(phc_checks) / sizeof(phc_checks[0]); ++i) {
        if (phc_checks[i].control == control) {
            return &phc_checks[i];
        }
    }
    return NULL;
}

/*!
 * \brief Called when a check has not run within PHC_MAX_CHECK_INTERVAL_MS since its last run
 *
 * \details The watchdog is left timed out: the fault is never cleared, so it
 *          does not need to fire again
 *
 * \param[in] context Unused
 */
EAGLETRT_STATIC void prv_phc_handle_check_timeout(void *context) {
    EAGLETRT_API_UNUSED(context);
    EAGLETRT_API_UNUSED(mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_PHC_FAULT));
}

/*!
 * \brief Called PHC_MAX_STARTUP_INTERVAL_MS after init, sets the PHC fault if a check has never run
 *
 * \details A check that has run has its watchdog running or timed out, the
 *          watchdog of a check that has never run is still idle
 *
 * \param[in] context Unused
 */
EAGLETRT_STATIC void prv_phc_handle_startup_timeout(void *context) {
    EAGLETRT_API_UNUSED(context);

    for (enum PHCControl control = 0; control < PHC_CONTROL_COUNT; ++control) {
        struct Watchdog *const watchdog = &phc_handler.check_watchdogs[control];
        if (!mainboard_watchdogs_api_is_running(watchdog) && !mainboard_watchdogs_api_is_timed_out(watchdog)) {
            EAGLETRT_API_UNUSED(mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_PHC_FAULT));
            return;
        }
    }
}

enum PHCReturnCode phc_api_init(void) {
    // Zeroing the handler would corrupt the watchdog pool, which still holds the running watchdogs
    if (phc_handler.initialized) {
        return PHC_RC_ERROR;
    }

    memset(&phc_handler, 0, sizeof(phc_handler));

    for (enum PHCControl control = 0; control < PHC_CONTROL_COUNT; ++control) {
        if (mainboard_watchdogs_api_init_watchdog(&phc_handler.check_watchdogs[control], PHC_MAX_CHECK_INTERVAL_MS, prv_phc_handle_check_timeout, NULL) != WATCHDOG_RC_OK) {
            return PHC_RC_ERROR;
        }
    }

    if (mainboard_watchdogs_api_init_watchdog(&phc_handler.startup_watchdog, PHC_MAX_STARTUP_INTERVAL_MS, prv_phc_handle_startup_timeout, NULL) != WATCHDOG_RC_OK) {
        return PHC_RC_ERROR;
    }

    if (mainboard_watchdogs_api_start(&phc_handler.startup_watchdog) != WATCHDOG_RC_OK) {
        return PHC_RC_ERROR;
    }

    phc_handler.initialized = true;
    return PHC_RC_OK;
}

enum PHCReturnCode phc_api_check(enum PHCControl control) {
    if (control >= PHC_CONTROL_COUNT) {
        return PHC_RC_ERROR;
    }

    const struct PHCCheck *check = prv_phc_find_check(control);
    if (check == NULL || check->check_function == NULL) {
        return PHC_RC_ERROR;
    }

    const enum PhcCheckResult check_result = check->check_function();

    // Without data the check did not run: its watchdog is not touched, so the
    // PHC fault is set if the data never arrives or stops arriving
    if (check_result == PHC_CHECK_RESULT_NOT_READY) {
        return PHC_RC_NOT_READY;
    }

    if (check->kind == PHC_CHECK_KIND_CONDITION) {
        mainboard_status_api_set_condition(check->condition, check_result == PHC_CHECK_RESULT_OK);
    } else if (check_result != PHC_CHECK_RESULT_OK) {
        // Anything that is not an explicit OK sets the fault
        mainboard_status_api_set_fault(check->fault);
    }

    // The watchdogs are not initialized yet: touching them would latch the watchdogs fault
    if (phc_handler.initialized) {
        struct Watchdog *const watchdog = &phc_handler.check_watchdogs[control];

        // The first run starts the watchdog, a timed out one stays timed out since the fault is latched
        if (mainboard_watchdogs_api_pet(watchdog) == WATCHDOG_RC_NOT_RUNNING) {
            EAGLETRT_API_UNUSED(mainboard_watchdogs_api_start(watchdog));
        }
    }

    return PHC_RC_OK;
}
