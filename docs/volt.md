# volt.c
This module handles pack voltage acquisition, aggregation, range checking, and CAN payload preparation for both per-cell voltages and pack voltage statistics.

## Structures and enums

```VoltReturnCode```: standard return code for voltage functions.
- ```VOLT_OK```: function executed successfully.
- ```VOLT_NULL_POINTER```: at least one required pointer is NULL.
- ```VOLT_OUT_OF_BOUNDS```: attempted access is outside valid bounds.

```cells_voltage_t```: 2D matrix containing all voltage values.
- First dimension is the cellboard index.
- Second dimension is the segment series cell index.

```_VoltHandler```: internal module state.
- ```.voltages```: cached matrix of all cell voltages.
- ```.cellboard_id```: rolling cellboard index used for periodic CAN serialization.
- ```.offset```: rolling offset (step = 3 voltages) for periodic CAN serialization.
- ```.volt_can_payload```: reusable payload buffer for ```primary_hv_cells_voltage``` CAN message.
- ```.volt_stats_can_payload```: reusable payload buffer for ```primary_hv_cells_voltage_stats``` CAN message.

## Functions

```_volt_check_value```: validates one voltage sample and updates error flags, max 3.
- Sets/resets ```ERROR_GROUP_UNDER_VOLTAGE``` depending on ```VOLT_MIN_V```.
- Sets/resets ```ERROR_GROUP_OVER_VOLTAGE``` depending on ```VOLT_MAX_V```.

```volt_init```: resets all internal state and payload buffers, then pre-fills voltages.
- Clears the full handler with ```memset```.
- Sets all cached voltages to ```VOLT_MAX_V``` to avoid balancing issues at startup.

```volt_get_values```: returns the pointer to the full cached voltage matrix.

```volt_get_min```: scans all cached values and returns the minimum pack voltage.

```volt_get_max```: scans all cached values and returns the maximum pack voltage.

```volt_get_sum```: scans all cached values and returns the total sum of pack voltages.

```volt_get_avg```: returns the arithmetic mean over all cells (```volt_get_sum() / CELLBOARD_SERIES_COUNT```).

```volt_cells_voltage_handle```: ingests one cellboard voltage frame (3 voltages).
- Validates pointer, ```cellboard_id```, and offset bounds.
- Writes 3 voltages into the cache.
- Runs range checks (```_volt_check_value```) for each updated sample.

```volt_get_cells_voltage_canlib_payload```: prepares and returns next per-cell voltage CAN payload chunk.
- Serializes 3 voltages using current ```cellboard_id``` and ```offset```.
- Serializes payload ```offset``` field.
- Advances ```offset``` by 3 and wraps both ```offset``` and ```cellboard_id``` when needed.

```volt_get_cells_voltage_stats_canlib_payload```: prepares and returns aggregate voltage stats payload.
- Optionally writes payload size to ```byte_size```.
- Computes and fills max, min, delta, and avg from cached voltages.

