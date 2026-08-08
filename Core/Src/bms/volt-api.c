/*!
 * \file volt.c
 * \date 2024-04-20
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Voltage measurment and control
 */

#include "volt-api.h"

#include <stddef.h>
#include <string.h>

#include "can-primary.h"
#include "error-api.h"
#include "eagletrt-api.h"
#include "mainboard-def.h"

#ifdef CONF_VOLTAGE_MODULE_ENABLE

EAGLETRT_STATIC struct VoltHandler volt_handler;

/*!
 * \brief Check if the voltage values are in range otherwise set an error
 *
 * \param id The cellboard identifier
 * \param offset The cell offset of the segment
 * \param value The voltage value in V
 */
EAGLETRT_STATIC_INLINE void prv_volt_check_value(const CellboardId cellboard_id, const size_t offset, const volt_t value) {
    size_t index = (cellboard_id * CELLBOARD_SEGMENT_SERIES_COUNT) + offset;
    if (value < VOLT_MIN_V) {
        error_api_set(ERROR_GROUP_UNDER_VOLTAGE, (error_instance)index);
    } else {
        error_api_reset(ERROR_GROUP_UNDER_VOLTAGE, (error_instance)index);
    }

    if (value > VOLT_MAX_V) {
        error_api_set(ERROR_GROUP_OVER_VOLTAGE, (error_instance)index);
    } else {
        error_api_reset(ERROR_GROUP_OVER_VOLTAGE, (error_instance)index);
    }
}

enum VoltReturnCode volt_api_init(void) {
    memset(&volt_handler, 0U, sizeof(volt_handler));
    /*
     * Set the initial value of the voltages as maximum to avoid
     * problems during the balancing procedure
     */
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t cell = 0U; cell < CELLBOARD_SEGMENT_SERIES_COUNT; ++cell) {
            volt_handler.voltages[id][cell] = VOLT_MAX_V;
        }
    }
    return VOLT_RC_OK;
}

const cells_voltage *volt_api_get_values(void) {
    return &volt_handler.voltages;
}

void volt_api_set_value(const CellboardId cellboard, const uint8_t index, const volt_t voltage) {
    if (cellboard >= CELLBOARD_ID_COUNT || index >= CELLBOARD_SEGMENT_SERIES_COUNT) {
        return;
    }
    volt_handler.voltages[cellboard][index] = voltage;
}

volt_t volt_api_get_min(void) {
    volt_t min = volt_handler.min[0];
    for (CellboardId cellboard = 1; cellboard < CELLBOARD_ID_COUNT; ++cellboard) {
        min = EAGLETRT_API_MIN(volt_handler.min[cellboard], min);
    }
    return min;
}

volt_t volt_api_get_max(void) {
    volt_t max = volt_handler.max[0];
    for (CellboardId cellboard = 1; cellboard < CELLBOARD_ID_COUNT; ++cellboard) {
        max = EAGLETRT_API_MAX(volt_handler.max[cellboard], max);
    }
    return max;
}

volt_t volt_api_get_sum(void) {
    volt_t sum = 0.F;
    for (CellboardId cellboard = 0; cellboard < CELLBOARD_ID_COUNT; ++cellboard) {
        sum += volt_handler.sum[cellboard];
    }
    return sum;
}

volt_t volt_api_get_avg(void) {
    volt_t average = 0;
    for (CellboardId cellboard = 0; cellboard < CELLBOARD_ID_COUNT; ++cellboard) {
        average += volt_handler.average[cellboard] * CELLBOARD_SEGMENT_SERIES_COUNT;
    }
    return average / (float)CELLBOARD_SERIES_COUNT;
}

union CanPrimaryMessages *volt_api_get_cellboard1_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard1voltage;
    }

    struct CanPrimaryTsaccellboard1voltage *payload = &volt_handler.libcan_message_cellboard1.tsaccellboard1voltage;
    const volt_t *const volts = volt_handler.voltages[CELLBOARD_ID_0];
    payload->group = (payload->group >= 3) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = volts[0];
            payload->group_payload.mux_0.cell2 = volts[1];
            payload->group_payload.mux_0.cell3 = volts[2];
            payload->group_payload.mux_0.cell4 = volts[3];
            payload->group_payload.mux_0.cell5 = volts[4];
            payload->group_payload.mux_0.cell6 = volts[5];
            break;
        case 1:
            payload->group_payload.mux_1.cell7 = volts[6];
            payload->group_payload.mux_1.cell8 = volts[7];
            payload->group_payload.mux_1.cell9 = volts[8];
            payload->group_payload.mux_1.cell10 = volts[9];
            payload->group_payload.mux_1.cell11 = volts[10];
            payload->group_payload.mux_1.cell12 = volts[11];
            break;
        case 2:
            payload->group_payload.mux_2.cell13 = volts[12];
            payload->group_payload.mux_2.cell14 = volts[13];
            payload->group_payload.mux_2.cell15 = volts[14];
            payload->group_payload.mux_2.cell16 = volts[15];
            payload->group_payload.mux_2.cell17 = volts[16];
            payload->group_payload.mux_2.cell18 = volts[17];
            break;
        case 3:
            payload->group_payload.mux_3.cell19 = volts[18];
            payload->group_payload.mux_3.cell20 = volts[19];
            payload->group_payload.mux_3.cell21 = volts[20];
            payload->group_payload.mux_3.cell22 = volts[21];
            payload->group_payload.mux_3.cell23 = volts[22];
            payload->group_payload.mux_3.cell24 = volts[23];
            break;
        default:
            break;
    }
    return &volt_handler.libcan_message_cellboard1;
}

union CanPrimaryMessages *volt_api_get_cellboard2_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard2voltage;
    }

    struct CanPrimaryTsaccellboard2voltage *payload = &volt_handler.libcan_message_cellboard2.tsaccellboard2voltage;
    const volt_t *const volts = volt_handler.voltages[CELLBOARD_ID_1];
    payload->group = (payload->group >= 3) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = volts[0];
            payload->group_payload.mux_0.cell2 = volts[1];
            payload->group_payload.mux_0.cell3 = volts[2];
            payload->group_payload.mux_0.cell4 = volts[3];
            payload->group_payload.mux_0.cell5 = volts[4];
            payload->group_payload.mux_0.cell6 = volts[5];
            break;
        case 1:
            payload->group_payload.mux_1.cell7 = volts[6];
            payload->group_payload.mux_1.cell8 = volts[7];
            payload->group_payload.mux_1.cell9 = volts[8];
            payload->group_payload.mux_1.cell10 = volts[9];
            payload->group_payload.mux_1.cell11 = volts[10];
            payload->group_payload.mux_1.cell12 = volts[11];
            break;
        case 2:
            payload->group_payload.mux_2.cell13 = volts[12];
            payload->group_payload.mux_2.cell14 = volts[13];
            payload->group_payload.mux_2.cell15 = volts[14];
            payload->group_payload.mux_2.cell16 = volts[15];
            payload->group_payload.mux_2.cell17 = volts[16];
            payload->group_payload.mux_2.cell18 = volts[17];
            break;
        case 3:
            payload->group_payload.mux_3.cell19 = volts[18];
            payload->group_payload.mux_3.cell20 = volts[19];
            payload->group_payload.mux_3.cell21 = volts[20];
            payload->group_payload.mux_3.cell22 = volts[21];
            payload->group_payload.mux_3.cell23 = volts[22];
            payload->group_payload.mux_3.cell24 = volts[23];
            break;
        default:
            break;
    }
    return &volt_handler.libcan_message_cellboard2;
}

union CanPrimaryMessages *volt_api_get_cellboard3_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard3voltage;
    }

    struct CanPrimaryTsaccellboard3voltage *payload = &volt_handler.libcan_message_cellboard3.tsaccellboard3voltage;
    const volt_t *const volts = volt_handler.voltages[CELLBOARD_ID_2];
    payload->group = (payload->group >= 3) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = volts[0];
            payload->group_payload.mux_0.cell2 = volts[1];
            payload->group_payload.mux_0.cell3 = volts[2];
            payload->group_payload.mux_0.cell4 = volts[3];
            payload->group_payload.mux_0.cell5 = volts[4];
            payload->group_payload.mux_0.cell6 = volts[5];
            break;
        case 1:
            payload->group_payload.mux_1.cell7 = volts[6];
            payload->group_payload.mux_1.cell8 = volts[7];
            payload->group_payload.mux_1.cell9 = volts[8];
            payload->group_payload.mux_1.cell10 = volts[9];
            payload->group_payload.mux_1.cell11 = volts[10];
            payload->group_payload.mux_1.cell12 = volts[11];
            break;
        case 2:
            payload->group_payload.mux_2.cell13 = volts[12];
            payload->group_payload.mux_2.cell14 = volts[13];
            payload->group_payload.mux_2.cell15 = volts[14];
            payload->group_payload.mux_2.cell16 = volts[15];
            payload->group_payload.mux_2.cell17 = volts[16];
            payload->group_payload.mux_2.cell18 = volts[17];
            break;
        case 3:
            payload->group_payload.mux_3.cell19 = volts[18];
            payload->group_payload.mux_3.cell20 = volts[19];
            payload->group_payload.mux_3.cell21 = volts[20];
            payload->group_payload.mux_3.cell22 = volts[21];
            payload->group_payload.mux_3.cell23 = volts[22];
            payload->group_payload.mux_3.cell24 = volts[23];
            break;
        default:
            break;
    }
    return &volt_handler.libcan_message_cellboard3;
}

union CanPrimaryMessages *volt_api_get_cellboard4_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard4voltage;
    }

    struct CanPrimaryTsaccellboard4voltage *payload = &volt_handler.libcan_message_cellboard4.tsaccellboard4voltage;
    const volt_t *const volts = volt_handler.voltages[CELLBOARD_ID_3];
    payload->group = (payload->group >= 3) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = volts[0];
            payload->group_payload.mux_0.cell2 = volts[1];
            payload->group_payload.mux_0.cell3 = volts[2];
            payload->group_payload.mux_0.cell4 = volts[3];
            payload->group_payload.mux_0.cell5 = volts[4];
            payload->group_payload.mux_0.cell6 = volts[5];
            break;
        case 1:
            payload->group_payload.mux_1.cell7 = volts[6];
            payload->group_payload.mux_1.cell8 = volts[7];
            payload->group_payload.mux_1.cell9 = volts[8];
            payload->group_payload.mux_1.cell10 = volts[9];
            payload->group_payload.mux_1.cell11 = volts[10];
            payload->group_payload.mux_1.cell12 = volts[11];
            break;
        case 2:
            payload->group_payload.mux_2.cell13 = volts[12];
            payload->group_payload.mux_2.cell14 = volts[13];
            payload->group_payload.mux_2.cell15 = volts[14];
            payload->group_payload.mux_2.cell16 = volts[15];
            payload->group_payload.mux_2.cell17 = volts[16];
            payload->group_payload.mux_2.cell18 = volts[17];
            break;
        case 3:
            payload->group_payload.mux_3.cell19 = volts[18];
            payload->group_payload.mux_3.cell20 = volts[19];
            payload->group_payload.mux_3.cell21 = volts[20];
            payload->group_payload.mux_3.cell22 = volts[21];
            payload->group_payload.mux_3.cell23 = volts[22];
            payload->group_payload.mux_3.cell24 = volts[23];
            break;
        default:
            break;
    }
    return &volt_handler.libcan_message_cellboard4;
}

union CanPrimaryMessages *volt_api_get_cellboard5_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard5voltage;
    }

    struct CanPrimaryTsaccellboard5voltage *payload = &volt_handler.libcan_message_cellboard5.tsaccellboard5voltage;
    const volt_t *const volts = volt_handler.voltages[CELLBOARD_ID_4];
    payload->group = (payload->group >= 3) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = volts[0];
            payload->group_payload.mux_0.cell2 = volts[1];
            payload->group_payload.mux_0.cell3 = volts[2];
            payload->group_payload.mux_0.cell4 = volts[3];
            payload->group_payload.mux_0.cell5 = volts[4];
            payload->group_payload.mux_0.cell6 = volts[5];
            break;
        case 1:
            payload->group_payload.mux_1.cell7 = volts[6];
            payload->group_payload.mux_1.cell8 = volts[7];
            payload->group_payload.mux_1.cell9 = volts[8];
            payload->group_payload.mux_1.cell10 = volts[9];
            payload->group_payload.mux_1.cell11 = volts[10];
            payload->group_payload.mux_1.cell12 = volts[11];
            break;
        case 2:
            payload->group_payload.mux_2.cell13 = volts[12];
            payload->group_payload.mux_2.cell14 = volts[13];
            payload->group_payload.mux_2.cell15 = volts[14];
            payload->group_payload.mux_2.cell16 = volts[15];
            payload->group_payload.mux_2.cell17 = volts[16];
            payload->group_payload.mux_2.cell18 = volts[17];
            break;
        case 3:
            payload->group_payload.mux_3.cell19 = volts[18];
            payload->group_payload.mux_3.cell20 = volts[19];
            payload->group_payload.mux_3.cell21 = volts[20];
            payload->group_payload.mux_3.cell22 = volts[21];
            payload->group_payload.mux_3.cell23 = volts[22];
            payload->group_payload.mux_3.cell24 = volts[23];
            break;
        default:
            break;
    }
    return &volt_handler.libcan_message_cellboard5;
}

union CanPrimaryMessages *volt_api_get_cellboard6_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard6voltage;
    }

    struct CanPrimaryTsaccellboard6voltage *payload = &volt_handler.libcan_message_cellboard6.tsaccellboard6voltage;
    const volt_t *const volts = volt_handler.voltages[CELLBOARD_ID_5];
    payload->group = (payload->group >= 3) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = volts[0];
            payload->group_payload.mux_0.cell2 = volts[1];
            payload->group_payload.mux_0.cell3 = volts[2];
            payload->group_payload.mux_0.cell4 = volts[3];
            payload->group_payload.mux_0.cell5 = volts[4];
            payload->group_payload.mux_0.cell6 = volts[5];
            break;
        case 1:
            payload->group_payload.mux_1.cell7 = volts[6];
            payload->group_payload.mux_1.cell8 = volts[7];
            payload->group_payload.mux_1.cell9 = volts[8];
            payload->group_payload.mux_1.cell10 = volts[9];
            payload->group_payload.mux_1.cell11 = volts[10];
            payload->group_payload.mux_1.cell12 = volts[11];
            break;
        case 2:
            payload->group_payload.mux_2.cell13 = volts[12];
            payload->group_payload.mux_2.cell14 = volts[13];
            payload->group_payload.mux_2.cell15 = volts[14];
            payload->group_payload.mux_2.cell16 = volts[15];
            payload->group_payload.mux_2.cell17 = volts[16];
            payload->group_payload.mux_2.cell18 = volts[17];
            break;
        case 3:
            payload->group_payload.mux_3.cell19 = volts[18];
            payload->group_payload.mux_3.cell20 = volts[19];
            payload->group_payload.mux_3.cell21 = volts[20];
            payload->group_payload.mux_3.cell22 = volts[21];
            payload->group_payload.mux_3.cell23 = volts[22];
            payload->group_payload.mux_3.cell24 = volts[23];
            break;
        default:
            break;
    }
    return &volt_handler.libcan_message_cellboard6;
}

void volt_api_cellboard_voltage_info_handle(
    CellboardId cellboard,
    volt_t min,
    volt_t max,
    volt_t average,
    volt_t sum) {
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return;
    }
    volt_handler.min[cellboard] = min;
    volt_handler.max[cellboard] = max;
    volt_handler.average[cellboard] = average;
    volt_handler.sum[cellboard] = sum;
}

#ifdef CONF_VOLTAGE_STRINGS_ENABLE

EAGLETRT_STATIC char *volt_module_name = "voltage";

EAGLETRT_STATIC char *volt_return_code_name[] = {
    [VOLT_RC_OK] = "ok",
    [VOLT_RC_NULL_POINTER] = "null pointer",
    [VOLT_RC_OUT_OF_BOUNDS] = "out of bounds"
};

EAGLETRT_STATIC char *volt_return_code_description[] = {
    [VOLT_RC_OK] = "executed successfully",
    [VOLT_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [VOLT_RC_OUT_OF_BOUNDS] = "attempt to access an invalid memory region"
};

#endif // CONF_VOLTAGE_STRINGS_ENABLE

#endif // CONF_VOLTAGE_MODULE_ENABLE
