/*!
 * \file bal-control-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cellboard balancing control: holds the balancing parameters, the discharge
 *        target and the balancing status reported by the cellboards
 */

#include "bal-control-api.h"
#include "bal-control.h"
#include "eagletrt.h"
#include "eagletrt-api.h"
#include "mainboard-def.h"
#include "mainboard-logger-api.h"
#include "voltages-api.h"
#include <stdint.h>
#include <string.h>

EAGLETRT_STATIC struct BalancingControlHandler bal_control_handler;

enum BalancingControlReturnCode bal_control_api_init(void) {
    memset(&bal_control_handler, 0, sizeof(bal_control_handler));
    return BAL_CONTROL_RC_OK;
}

enum BalancingControlReturnCode bal_control_api_set_parameters(const struct BalancingControlParameters parameters) {
    if (parameters.threshold < 0.0F) {
        MAINBOARD_LOGGER_WARNING("Balancing: invalid threshold %.3f V", (double)parameters.threshold);
        return BAL_CONTROL_RC_INVALID_PARAMETERS;
    }
    // The parameters come with every balancing message, only the change is logged
    if (!bal_control_handler.valid_parameters || parameters.threshold != bal_control_handler.parameters.threshold) {
        MAINBOARD_LOGGER_INFO("Balancing: threshold %.3f V", (double)parameters.threshold);
    }
    bal_control_handler.parameters = parameters;
    bal_control_handler.valid_parameters = true;
    return BAL_CONTROL_RC_OK;
}

bool bal_control_api_has_valid_parameters(void) {
    return bal_control_handler.valid_parameters;
}

void bal_control_api_update_target(void) {
    const volt target = voltages_api_get_pack_info().min;
    // Updated at every run of BALANCING_READ, only the change is logged
    if (target != bal_control_handler.target) {
        MAINBOARD_LOGGER_DEBUG("Balancing: target %.3f V", (double)target);
    }
    bal_control_handler.target = target;
}

void bal_control_api_start_discharge(void) {
    if (!bal_control_handler.valid_parameters) {
        return;
    }
    if (!bal_control_handler.discharging) {
        MAINBOARD_LOGGER_INFO("Balancing: discharge started, target %.3f V", (double)bal_control_handler.target);
    }
    bal_control_handler.discharging = true;
}

void bal_control_api_stop_discharge(void) {
    if (bal_control_handler.discharging) {
        MAINBOARD_LOGGER_INFO("Balancing: discharge stopped");
    }
    bal_control_handler.discharging = false;
}

bool bal_control_api_is_discharging(void) {
    return bal_control_handler.discharging;
}

bool bal_control_api_is_pack_balanced(void) {
    if (!bal_control_handler.valid_parameters) {
        return false;
    }
    const struct VoltagesInfo pack_info = voltages_api_get_pack_info();
    const volt delta = pack_info.max - pack_info.min;

    return delta <= bal_control_handler.parameters.threshold;
}

volt bal_control_api_get_target_voltage(void) {
    return bal_control_handler.target;
}

volt bal_control_api_get_threshold(void) {
    return bal_control_handler.parameters.threshold;
}

enum BalancingControlReturnCode bal_control_api_set_balancing_status(bool set, enum CellboardId cellboard, uint8_t cell) {
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return BAL_CONTROL_RC_INVALID_PARAMETERS;
    }
    if (cell >= CELLBOARD_SEGMENT_SERIES_COUNT) {
        return BAL_CONTROL_RC_INVALID_PARAMETERS;
    }

    if (set) {
        bal_control_handler.balancing_cells[cellboard] = EAGLETRT_API_BIT_SET(bal_control_handler.balancing_cells[cellboard], cell);
    } else {
        bal_control_handler.balancing_cells[cellboard] = EAGLETRT_API_BIT_RESET(bal_control_handler.balancing_cells[cellboard], cell);
    }

    return BAL_CONTROL_RC_OK;
}

enum BalancingControlReturnCode bal_control_api_set_balancing_status_flag(enum CellboardId cellboard, uint32_t flag) {
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return BAL_CONTROL_RC_INVALID_PARAMETERS;
    }
    bal_control_handler.balancing_cells[cellboard] = flag;
    return BAL_CONTROL_RC_OK;
}

uint32_t bal_control_api_get_balancing_status(enum CellboardId cellboard) {
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return 0;
    }
    return bal_control_handler.balancing_cells[cellboard];
}
