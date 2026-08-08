/*!
 * \file temp.c
 * \date 2024-08-30
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Temperature measurment and control
 */

#include "temp-api.h"

#include <string.h>

#include "eagletrt-api.h"
#include "error-api.h"
#include "can-primary.h"

#ifdef CONF_TEMPERATURE_MODULE_ENABLE

EAGLETRT_STATIC struct TempHandler temp_handler;

// clang-format off

// Array to map cells index in memory to phisical positions
EAGLETRT_STATIC constexpr size_t temp_cell_position_index_map[] = {
    63U, 65U, 15U, 61U,  6U, 39U, 46U,  7U,
    40U, 31U,  8U, 41U, 64U, 36U, 54U,  1U,
    51U, 42U, 32U, 37U, 55U, 48U, 52U, 43U,
    49U, 38U, 72U,  2U,  9U, 44U, 33U, 67U,
    45U,  3U, 10U, 75U, 34U, 11U, 57U,  4U,
    12U, 70U, 35U, 13U, 69U,  5U, 14U, 73U
};

// clang-format on

/*!
 * \brief Get the phisical cell position from its index in memory
 *
 * \param index The cell index in memory
 *
 * \return int32_t The cell phisical position if index is valid, otherwise -1
 */
EAGLETRT_STATIC_INLINE int32_t prv_temp_cell_position_from_index(size_t index) {
    if (index < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        return (int32_t)temp_cell_position_index_map[index];
    }
    return -1;
}

/*!
 * \brief Check if the temperature values are in range otherwise set an error
 *
 * \param cellboard_id The cellboard identifier
 * \param offset The cell offset of the segment
 * \param value The temperature value in °C
 */
EAGLETRT_STATIC_INLINE void prv_temp_check_value(const CellboardId cellboard_id, const size_t offset, const celsius_t value) {
    size_t index = (cellboard_id * CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) + offset;
    if (value < TEMP_MIN_C) {
        error_api_set(ERROR_GROUP_UNDER_TEMPERATURE, (error_instance)index);
    } else {
        error_api_reset(ERROR_GROUP_UNDER_TEMPERATURE, (error_instance)index);
    }

    if (value > TEMP_MAX_C) {
        error_api_set(ERROR_GROUP_OVER_TEMPERATURE, (error_instance)index);
    } else {
        error_api_reset(ERROR_GROUP_OVER_TEMPERATURE, (error_instance)index);
    }
}

enum TempReturnCode temp_api_init(void) {
    memset(&temp_handler, 0U, sizeof(temp_handler));
    return TEMP_RC_OK;
}

const cells_temp *temp_api_get_values(void) {
    return &temp_handler.temperatures;
}

celsius_t temp_api_get_min(void) {
    celsius_t min = temp_handler.temperatures[0][0];
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            min = EAGLETRT_API_MIN(min, temp_handler.temperatures[i][j]);
        }
    }
    return min;
}

celsius_t temp_api_get_max(void) {
    celsius_t max = temp_handler.temperatures[0][0];
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            max = EAGLETRT_API_MAX(max, temp_handler.temperatures[i][j]);
        }
    }
    return max;
}

celsius_t temp_api_get_sum(void) {
    celsius_t sum = 0U;
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            sum += temp_handler.temperatures[i][j];
        }
    }
    return sum;
}

celsius_t temp_api_get_avg(void) {
    return temp_api_get_sum() / CELLBOARD_TEMP_SENSOR_COUNT;
}

union CanPrimaryMessages *temp_api_get_cells_temperature_info_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardtemperatureinfo;
    }

    struct CanPrimaryTsacmainboardtemperatureinfo *info = &temp_handler.libcan_message_temperature_info.tsacmainboardtemperatureinfo;
    info->max = temp_api_get_max();
    info->min = temp_api_get_min();
    info->average = temp_api_get_avg();
    return &temp_handler.libcan_message_temperature_info;
}

union CanPrimaryMessages *temp_api_get_cellboard1_temperature_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard1temperature;
    }

    struct CanPrimaryTsaccellboard1temperature *payload = &temp_handler.libcan_message_cellboard1.tsaccellboard1temperature;
    const celsius_t *const temps = temp_handler.temperatures[CELLBOARD_ID_0];
    payload->group = (payload->group >= 9) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = temps[0];
            payload->group_payload.mux_0.cell2 = temps[1];
            payload->group_payload.mux_0.cell3 = temps[2];
            payload->group_payload.mux_0.cell4 = temps[3];
            payload->group_payload.mux_0.cell5 = temps[4];
            break;
        case 1:
            payload->group_payload.mux_1.cell6 = temps[5];
            payload->group_payload.mux_1.cell7 = temps[6];
            payload->group_payload.mux_1.cell8 = temps[7];
            payload->group_payload.mux_1.cell9 = temps[8];
            payload->group_payload.mux_1.cell10 = temps[9];
            break;
        case 2:
            payload->group_payload.mux_2.cell11 = temps[10];
            payload->group_payload.mux_2.cell12 = temps[11];
            payload->group_payload.mux_2.cell13 = temps[12];
            payload->group_payload.mux_2.cell14 = temps[13];
            payload->group_payload.mux_2.cell15 = temps[14];
            break;
        case 3:
            payload->group_payload.mux_3.cell16 = temps[15];
            payload->group_payload.mux_3.cell17 = temps[16];
            payload->group_payload.mux_3.cell18 = temps[17];
            payload->group_payload.mux_3.cell19 = temps[18];
            payload->group_payload.mux_3.cell20 = temps[19];
            break;
        case 4:
            payload->group_payload.mux_4.cell21 = temps[20];
            payload->group_payload.mux_4.cell22 = temps[21];
            payload->group_payload.mux_4.cell23 = temps[22];
            payload->group_payload.mux_4.cell24 = temps[23];
            payload->group_payload.mux_4.cell25 = temps[24];
            break;
        case 5:
            payload->group_payload.mux_5.cell26 = temps[25];
            payload->group_payload.mux_5.cell27 = temps[26];
            payload->group_payload.mux_5.cell28 = temps[27];
            payload->group_payload.mux_5.cell29 = temps[28];
            payload->group_payload.mux_5.cell30 = temps[29];
            break;
        case 6:
            payload->group_payload.mux_6.cell31 = temps[30];
            payload->group_payload.mux_6.cell32 = temps[31];
            payload->group_payload.mux_6.cell33 = temps[32];
            payload->group_payload.mux_6.cell34 = temps[33];
            payload->group_payload.mux_6.cell35 = temps[34];
            break;
        case 7:
            payload->group_payload.mux_7.cell36 = temps[35];
            payload->group_payload.mux_7.cell37 = temps[36];
            payload->group_payload.mux_7.cell38 = temps[37];
            payload->group_payload.mux_7.cell39 = temps[38];
            payload->group_payload.mux_7.cell40 = temps[39];
            break;
        case 8:
            payload->group_payload.mux_8.cell41 = temps[40];
            payload->group_payload.mux_8.cell42 = temps[41];
            payload->group_payload.mux_8.cell43 = temps[42];
            payload->group_payload.mux_8.cell44 = temps[43];
            payload->group_payload.mux_8.cell45 = temps[44];
            break;
        case 9:
            payload->group_payload.mux_9.cell46 = temps[45];
            payload->group_payload.mux_9.cell47 = temps[46];
            payload->group_payload.mux_9.cell48 = temps[47];
            break;
        default:
            break;
    }
    return &temp_handler.libcan_message_cellboard1;
}

union CanPrimaryMessages *temp_api_get_cellboard2_temperature_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard2temperature;
    }

    struct CanPrimaryTsaccellboard2temperature *payload = &temp_handler.libcan_message_cellboard2.tsaccellboard2temperature;
    const celsius_t *const temps = temp_handler.temperatures[CELLBOARD_ID_1];
    payload->group = (payload->group >= 9) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = temps[0];
            payload->group_payload.mux_0.cell2 = temps[1];
            payload->group_payload.mux_0.cell3 = temps[2];
            payload->group_payload.mux_0.cell4 = temps[3];
            payload->group_payload.mux_0.cell5 = temps[4];
            break;
        case 1:
            payload->group_payload.mux_1.cell6 = temps[5];
            payload->group_payload.mux_1.cell7 = temps[6];
            payload->group_payload.mux_1.cell8 = temps[7];
            payload->group_payload.mux_1.cell9 = temps[8];
            payload->group_payload.mux_1.cell10 = temps[9];
            break;
        case 2:
            payload->group_payload.mux_2.cell11 = temps[10];
            payload->group_payload.mux_2.cell12 = temps[11];
            payload->group_payload.mux_2.cell13 = temps[12];
            payload->group_payload.mux_2.cell14 = temps[13];
            payload->group_payload.mux_2.cell15 = temps[14];
            break;
        case 3:
            payload->group_payload.mux_3.cell16 = temps[15];
            payload->group_payload.mux_3.cell17 = temps[16];
            payload->group_payload.mux_3.cell18 = temps[17];
            payload->group_payload.mux_3.cell19 = temps[18];
            payload->group_payload.mux_3.cell20 = temps[19];
            break;
        case 4:
            payload->group_payload.mux_4.cell21 = temps[20];
            payload->group_payload.mux_4.cell22 = temps[21];
            payload->group_payload.mux_4.cell23 = temps[22];
            payload->group_payload.mux_4.cell24 = temps[23];
            payload->group_payload.mux_4.cell25 = temps[24];
            break;
        case 5:
            payload->group_payload.mux_5.cell26 = temps[25];
            payload->group_payload.mux_5.cell27 = temps[26];
            payload->group_payload.mux_5.cell28 = temps[27];
            payload->group_payload.mux_5.cell29 = temps[28];
            payload->group_payload.mux_5.cell30 = temps[29];
            break;
        case 6:
            payload->group_payload.mux_6.cell31 = temps[30];
            payload->group_payload.mux_6.cell32 = temps[31];
            payload->group_payload.mux_6.cell33 = temps[32];
            payload->group_payload.mux_6.cell34 = temps[33];
            payload->group_payload.mux_6.cell35 = temps[34];
            break;
        case 7:
            payload->group_payload.mux_7.cell36 = temps[35];
            payload->group_payload.mux_7.cell37 = temps[36];
            payload->group_payload.mux_7.cell38 = temps[37];
            payload->group_payload.mux_7.cell39 = temps[38];
            payload->group_payload.mux_7.cell40 = temps[39];
            break;
        case 8:
            payload->group_payload.mux_8.cell41 = temps[40];
            payload->group_payload.mux_8.cell42 = temps[41];
            payload->group_payload.mux_8.cell43 = temps[42];
            payload->group_payload.mux_8.cell44 = temps[43];
            payload->group_payload.mux_8.cell45 = temps[44];
            break;
        case 9:
            payload->group_payload.mux_9.cell46 = temps[45];
            payload->group_payload.mux_9.cell47 = temps[46];
            payload->group_payload.mux_9.cell48 = temps[47];
            break;
        default:
            break;
    }
    return &temp_handler.libcan_message_cellboard2;
}

union CanPrimaryMessages *temp_api_get_cellboard3_temperature_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard3temperature;
    }

    struct CanPrimaryTsaccellboard3temperature *payload = &temp_handler.libcan_message_cellboard3.tsaccellboard3temperature;
    const celsius_t *const temps = temp_handler.temperatures[CELLBOARD_ID_2];
    payload->group = (payload->group >= 9) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = temps[0];
            payload->group_payload.mux_0.cell2 = temps[1];
            payload->group_payload.mux_0.cell3 = temps[2];
            payload->group_payload.mux_0.cell4 = temps[3];
            payload->group_payload.mux_0.cell5 = temps[4];
            break;
        case 1:
            payload->group_payload.mux_1.cell6 = temps[5];
            payload->group_payload.mux_1.cell7 = temps[6];
            payload->group_payload.mux_1.cell8 = temps[7];
            payload->group_payload.mux_1.cell9 = temps[8];
            payload->group_payload.mux_1.cell10 = temps[9];
            break;
        case 2:
            payload->group_payload.mux_2.cell11 = temps[10];
            payload->group_payload.mux_2.cell12 = temps[11];
            payload->group_payload.mux_2.cell13 = temps[12];
            payload->group_payload.mux_2.cell14 = temps[13];
            payload->group_payload.mux_2.cell15 = temps[14];
            break;
        case 3:
            payload->group_payload.mux_3.cell16 = temps[15];
            payload->group_payload.mux_3.cell17 = temps[16];
            payload->group_payload.mux_3.cell18 = temps[17];
            payload->group_payload.mux_3.cell19 = temps[18];
            payload->group_payload.mux_3.cell20 = temps[19];
            break;
        case 4:
            payload->group_payload.mux_4.cell21 = temps[20];
            payload->group_payload.mux_4.cell22 = temps[21];
            payload->group_payload.mux_4.cell23 = temps[22];
            payload->group_payload.mux_4.cell24 = temps[23];
            payload->group_payload.mux_4.cell25 = temps[24];
            break;
        case 5:
            payload->group_payload.mux_5.cell26 = temps[25];
            payload->group_payload.mux_5.cell27 = temps[26];
            payload->group_payload.mux_5.cell28 = temps[27];
            payload->group_payload.mux_5.cell29 = temps[28];
            payload->group_payload.mux_5.cell30 = temps[29];
            break;
        case 6:
            payload->group_payload.mux_6.cell31 = temps[30];
            payload->group_payload.mux_6.cell32 = temps[31];
            payload->group_payload.mux_6.cell33 = temps[32];
            payload->group_payload.mux_6.cell34 = temps[33];
            payload->group_payload.mux_6.cell35 = temps[34];
            break;
        case 7:
            payload->group_payload.mux_7.cell36 = temps[35];
            payload->group_payload.mux_7.cell37 = temps[36];
            payload->group_payload.mux_7.cell38 = temps[37];
            payload->group_payload.mux_7.cell39 = temps[38];
            payload->group_payload.mux_7.cell40 = temps[39];
            break;
        case 8:
            payload->group_payload.mux_8.cell41 = temps[40];
            payload->group_payload.mux_8.cell42 = temps[41];
            payload->group_payload.mux_8.cell43 = temps[42];
            payload->group_payload.mux_8.cell44 = temps[43];
            payload->group_payload.mux_8.cell45 = temps[44];
            break;
        case 9:
            payload->group_payload.mux_9.cell46 = temps[45];
            payload->group_payload.mux_9.cell47 = temps[46];
            payload->group_payload.mux_9.cell48 = temps[47];
            break;
        default:
            break;
    }
    return &temp_handler.libcan_message_cellboard3;
}

union CanPrimaryMessages *temp_api_get_cellboard4_temperature_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard4temperature;
    }

    struct CanPrimaryTsaccellboard4temperature *payload = &temp_handler.libcan_message_cellboard4.tsaccellboard4temperature;
    const celsius_t *const temps = temp_handler.temperatures[CELLBOARD_ID_3];
    payload->group = (payload->group >= 9) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = temps[0];
            payload->group_payload.mux_0.cell2 = temps[1];
            payload->group_payload.mux_0.cell3 = temps[2];
            payload->group_payload.mux_0.cell4 = temps[3];
            payload->group_payload.mux_0.cell5 = temps[4];
            break;
        case 1:
            payload->group_payload.mux_1.cell6 = temps[5];
            payload->group_payload.mux_1.cell7 = temps[6];
            payload->group_payload.mux_1.cell8 = temps[7];
            payload->group_payload.mux_1.cell9 = temps[8];
            payload->group_payload.mux_1.cell10 = temps[9];
            break;
        case 2:
            payload->group_payload.mux_2.cell11 = temps[10];
            payload->group_payload.mux_2.cell12 = temps[11];
            payload->group_payload.mux_2.cell13 = temps[12];
            payload->group_payload.mux_2.cell14 = temps[13];
            payload->group_payload.mux_2.cell15 = temps[14];
            break;
        case 3:
            payload->group_payload.mux_3.cell16 = temps[15];
            payload->group_payload.mux_3.cell17 = temps[16];
            payload->group_payload.mux_3.cell18 = temps[17];
            payload->group_payload.mux_3.cell19 = temps[18];
            payload->group_payload.mux_3.cell20 = temps[19];
            break;
        case 4:
            payload->group_payload.mux_4.cell21 = temps[20];
            payload->group_payload.mux_4.cell22 = temps[21];
            payload->group_payload.mux_4.cell23 = temps[22];
            payload->group_payload.mux_4.cell24 = temps[23];
            payload->group_payload.mux_4.cell25 = temps[24];
            break;
        case 5:
            payload->group_payload.mux_5.cell26 = temps[25];
            payload->group_payload.mux_5.cell27 = temps[26];
            payload->group_payload.mux_5.cell28 = temps[27];
            payload->group_payload.mux_5.cell29 = temps[28];
            payload->group_payload.mux_5.cell30 = temps[29];
            break;
        case 6:
            payload->group_payload.mux_6.cell31 = temps[30];
            payload->group_payload.mux_6.cell32 = temps[31];
            payload->group_payload.mux_6.cell33 = temps[32];
            payload->group_payload.mux_6.cell34 = temps[33];
            payload->group_payload.mux_6.cell35 = temps[34];
            break;
        case 7:
            payload->group_payload.mux_7.cell36 = temps[35];
            payload->group_payload.mux_7.cell37 = temps[36];
            payload->group_payload.mux_7.cell38 = temps[37];
            payload->group_payload.mux_7.cell39 = temps[38];
            payload->group_payload.mux_7.cell40 = temps[39];
            break;
        case 8:
            payload->group_payload.mux_8.cell41 = temps[40];
            payload->group_payload.mux_8.cell42 = temps[41];
            payload->group_payload.mux_8.cell43 = temps[42];
            payload->group_payload.mux_8.cell44 = temps[43];
            payload->group_payload.mux_8.cell45 = temps[44];
            break;
        case 9:
            payload->group_payload.mux_9.cell46 = temps[45];
            payload->group_payload.mux_9.cell47 = temps[46];
            payload->group_payload.mux_9.cell48 = temps[47];
            break;
        default:
            break;
    }
    return &temp_handler.libcan_message_cellboard4;
}

union CanPrimaryMessages *temp_api_get_cellboard5_temperature_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard5temperature;
    }

    struct CanPrimaryTsaccellboard5temperature *payload = &temp_handler.libcan_message_cellboard5.tsaccellboard5temperature;
    const celsius_t *const temps = temp_handler.temperatures[CELLBOARD_ID_4];
    payload->group = (payload->group >= 9) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = temps[0];
            payload->group_payload.mux_0.cell2 = temps[1];
            payload->group_payload.mux_0.cell3 = temps[2];
            payload->group_payload.mux_0.cell4 = temps[3];
            payload->group_payload.mux_0.cell5 = temps[4];
            break;
        case 1:
            payload->group_payload.mux_1.cell6 = temps[5];
            payload->group_payload.mux_1.cell7 = temps[6];
            payload->group_payload.mux_1.cell8 = temps[7];
            payload->group_payload.mux_1.cell9 = temps[8];
            payload->group_payload.mux_1.cell10 = temps[9];
            break;
        case 2:
            payload->group_payload.mux_2.cell11 = temps[10];
            payload->group_payload.mux_2.cell12 = temps[11];
            payload->group_payload.mux_2.cell13 = temps[12];
            payload->group_payload.mux_2.cell14 = temps[13];
            payload->group_payload.mux_2.cell15 = temps[14];
            break;
        case 3:
            payload->group_payload.mux_3.cell16 = temps[15];
            payload->group_payload.mux_3.cell17 = temps[16];
            payload->group_payload.mux_3.cell18 = temps[17];
            payload->group_payload.mux_3.cell19 = temps[18];
            payload->group_payload.mux_3.cell20 = temps[19];
            break;
        case 4:
            payload->group_payload.mux_4.cell21 = temps[20];
            payload->group_payload.mux_4.cell22 = temps[21];
            payload->group_payload.mux_4.cell23 = temps[22];
            payload->group_payload.mux_4.cell24 = temps[23];
            payload->group_payload.mux_4.cell25 = temps[24];
            break;
        case 5:
            payload->group_payload.mux_5.cell26 = temps[25];
            payload->group_payload.mux_5.cell27 = temps[26];
            payload->group_payload.mux_5.cell28 = temps[27];
            payload->group_payload.mux_5.cell29 = temps[28];
            payload->group_payload.mux_5.cell30 = temps[29];
            break;
        case 6:
            payload->group_payload.mux_6.cell31 = temps[30];
            payload->group_payload.mux_6.cell32 = temps[31];
            payload->group_payload.mux_6.cell33 = temps[32];
            payload->group_payload.mux_6.cell34 = temps[33];
            payload->group_payload.mux_6.cell35 = temps[34];
            break;
        case 7:
            payload->group_payload.mux_7.cell36 = temps[35];
            payload->group_payload.mux_7.cell37 = temps[36];
            payload->group_payload.mux_7.cell38 = temps[37];
            payload->group_payload.mux_7.cell39 = temps[38];
            payload->group_payload.mux_7.cell40 = temps[39];
            break;
        case 8:
            payload->group_payload.mux_8.cell41 = temps[40];
            payload->group_payload.mux_8.cell42 = temps[41];
            payload->group_payload.mux_8.cell43 = temps[42];
            payload->group_payload.mux_8.cell44 = temps[43];
            payload->group_payload.mux_8.cell45 = temps[44];
            break;
        case 9:
            payload->group_payload.mux_9.cell46 = temps[45];
            payload->group_payload.mux_9.cell47 = temps[46];
            payload->group_payload.mux_9.cell48 = temps[47];
            break;
        default:
            break;
    }
    return &temp_handler.libcan_message_cellboard5;
}

union CanPrimaryMessages *temp_api_get_cellboard6_temperature_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsaccellboard6temperature;
    }

    struct CanPrimaryTsaccellboard6temperature *payload = &temp_handler.libcan_message_cellboard6.tsaccellboard6temperature;
    const celsius_t *const temps = temp_handler.temperatures[CELLBOARD_ID_5];
    payload->group = (payload->group >= 9) ? 0 : payload->group + 1;
    switch (payload->group) {
        case 0:
            payload->group_payload.mux_0.cell1 = temps[0];
            payload->group_payload.mux_0.cell2 = temps[1];
            payload->group_payload.mux_0.cell3 = temps[2];
            payload->group_payload.mux_0.cell4 = temps[3];
            payload->group_payload.mux_0.cell5 = temps[4];
            break;
        case 1:
            payload->group_payload.mux_1.cell6 = temps[5];
            payload->group_payload.mux_1.cell7 = temps[6];
            payload->group_payload.mux_1.cell8 = temps[7];
            payload->group_payload.mux_1.cell9 = temps[8];
            payload->group_payload.mux_1.cell10 = temps[9];
            break;
        case 2:
            payload->group_payload.mux_2.cell11 = temps[10];
            payload->group_payload.mux_2.cell12 = temps[11];
            payload->group_payload.mux_2.cell13 = temps[12];
            payload->group_payload.mux_2.cell14 = temps[13];
            payload->group_payload.mux_2.cell15 = temps[14];
            break;
        case 3:
            payload->group_payload.mux_3.cell16 = temps[15];
            payload->group_payload.mux_3.cell17 = temps[16];
            payload->group_payload.mux_3.cell18 = temps[17];
            payload->group_payload.mux_3.cell19 = temps[18];
            payload->group_payload.mux_3.cell20 = temps[19];
            break;
        case 4:
            payload->group_payload.mux_4.cell21 = temps[20];
            payload->group_payload.mux_4.cell22 = temps[21];
            payload->group_payload.mux_4.cell23 = temps[22];
            payload->group_payload.mux_4.cell24 = temps[23];
            payload->group_payload.mux_4.cell25 = temps[24];
            break;
        case 5:
            payload->group_payload.mux_5.cell26 = temps[25];
            payload->group_payload.mux_5.cell27 = temps[26];
            payload->group_payload.mux_5.cell28 = temps[27];
            payload->group_payload.mux_5.cell29 = temps[28];
            payload->group_payload.mux_5.cell30 = temps[29];
            break;
        case 6:
            payload->group_payload.mux_6.cell31 = temps[30];
            payload->group_payload.mux_6.cell32 = temps[31];
            payload->group_payload.mux_6.cell33 = temps[32];
            payload->group_payload.mux_6.cell34 = temps[33];
            payload->group_payload.mux_6.cell35 = temps[34];
            break;
        case 7:
            payload->group_payload.mux_7.cell36 = temps[35];
            payload->group_payload.mux_7.cell37 = temps[36];
            payload->group_payload.mux_7.cell38 = temps[37];
            payload->group_payload.mux_7.cell39 = temps[38];
            payload->group_payload.mux_7.cell40 = temps[39];
            break;
        case 8:
            payload->group_payload.mux_8.cell41 = temps[40];
            payload->group_payload.mux_8.cell42 = temps[41];
            payload->group_payload.mux_8.cell43 = temps[42];
            payload->group_payload.mux_8.cell44 = temps[43];
            payload->group_payload.mux_8.cell45 = temps[44];
            break;
        case 9:
            payload->group_payload.mux_9.cell46 = temps[45];
            payload->group_payload.mux_9.cell47 = temps[46];
            payload->group_payload.mux_9.cell48 = temps[47];
            break;
        default:
            break;
    }
    return &temp_handler.libcan_message_cellboard6;
}

// void temp_api_cells_temperature_handle(bms_cellboard_cells_temperature_converted_t *const payload) {
//     const size_t size = 4U;
//     if (payload == NULL ||
//         (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
//         payload->offset + size > CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
//         return;
//     }
//
//     // Update temperatures
//     const size_t offset = payload->offset;
//     celsius_t *const temperatures = temp_handler.temperatures[payload->cellboard_id];
//     temperatures[offset] = payload->temperature_0;
//     temperatures[offset + 1U] = payload->temperature_1;
//     temperatures[offset + 2U] = payload->temperature_2;
//     temperatures[offset + 3U] = payload->temperature_3;
//     for (size_t i = 0U; i < size; ++i) {
//         prv_temp_check_value((CellboardId)payload->cellboard_id, offset + i, temperatures[offset + i]);
//     }
// }

#ifdef CONF_TEMPERATURE_STRINGS_ENABLE

EAGLETRT_STATIC char *temp_module_name = "temperature";

EAGLETRT_STATIC char *temp_return_code_name[] = {
    [TEMP_RC_OK] = "ok",
    [TEMP_RC_NULL_POINTER] = "null pointer",
    [TEMP_RC_OUT_OF_BOUNDS] = "out of bounds"
};

EAGLETRT_STATIC char *temp_return_code_description[] = {
    [TEMP_RC_OK] = "executed successfully",
    [TEMP_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [TEMP_RC_OUT_OF_BOUNDS] = "attempt to access an invalid memory region"
};

#endif //  CONF_TEMPERATURE_STRINGS_ENABLE

#endif //  CONF_TEMPERATURE_MODULE_ENABLE
