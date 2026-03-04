# temp.c
This module handles pack temperature acquisition, aggregation, range checking, and CAN payload preparation for both per-cell temperatures and pack temperature statistics.

## Structures and enums

`TempReturnCode`: standard return code for temperature functions.
- `TEMP_OK`: function executed successfully.
- `TEMP_NULL_POINTER`: at least one required pointer is NULL.
- `TEMP_BUSY`: module is busy and cannot serve the request.
- `TEMP_OUT_OF_BOUNDS`: attempted access is outside valid bounds.

`cells_temp_t`: 2D matrix containing all temperature values.
- First dimension is the cellboard index.
- Second dimension is the segment temperature sensor index.

`_TempHandler`: internal module state.
- `.temperatures`: cached matrix of all cell temperatures.
- `.cellboard_id`: rolling cellboard index used for periodic CAN serialization.
- `.offset`: rolling offset (step = 4 temperatures) for periodic CAN serialization.
- `.temp_can_payload`: reusable payload buffer for `primary_hv_cells_temperature` CAN message.
- `.temp_stats_can_payload`: reusable payload buffer for `primary_hv_cells_temp_stats` CAN message.

## Functions

`_temp_cell_position_from_index`: maps an in-memory temperature index to its physical sensor position, this is needed because in the cellboard the ADC is connected to a multiplexer that is not connected in increasing order.

`_temp_check_value`: validates one temperature sample and updates error flags, they have a threshold of 5 consecutive triggers.
- Sets/resets `ERROR_GROUP_UNDER_TEMPERATURE` depending on `TEMP_MIN_C`.
- Sets/resets `ERROR_GROUP_OVER_TEMPERATURE` depending on `TEMP_MAX_C`.

`temp_init`: resets all internal state and payload buffers.

`temp_get_values`: returns the pointer to the full cached temperature matrix.

`temp_get_min`: scans all cached values and returns the minimum pack temperature.

`temp_get_max`: scans all cached values and returns the maximum pack temperature.

`temp_get_sum`: scans all cached values and returns the total sum of pack temperatures.

`temp_get_avg`: returns the arithmetic mean over all sensors (`temp_get_sum() / CELLBOARD_TEMP_SENSOR_COUNT`).

`temp_cells_temperature_handle`: handler for one cellboard temperature frame (4 temperatures).
- Validates pointer, `cellboard_id`, and offset bounds.
- Writes 4 temperatures into the cache.
- Runs range checks (`_temp_check_value`) for each updated sample.

`temp_get_cells_temperature_canlib_payload`: prepares and returns next per-cell temperature CAN payload chunk, this is sent every 50ms.
- Adds 4 temperatures to the payload using current `cellboard_id` and `offset`.
- Converts corresponding physical temperature IDs via the index map and adds matching IDs to the payload.
- Advances `offset` by `TEMP_NUM_TEMP_CAN_MESSAGE` and wraps both `offset` and `cellboard_id` when needed.

`temp_get_cells_temperature_stats_canlib_payload`: prepares and returns aggregate temperature stats payload, sent every 50ms. Computes and fills max, min, and avg from cached temperatures.
