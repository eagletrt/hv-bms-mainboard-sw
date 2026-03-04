# tasks.c
Tasks definition and task callback implementation used by the timebase scheduler.

This module has two responsibilities:
1) define all periodic jobs in one place (using an X-macro list)
2) expose initialization and query APIs so `timebase.c` can schedule and execute them

## Structures and enums

`TASKS_COUNT`: total number of tasks, equal to `TASKS_ID_COUNT`.

`TASKS_X_LIST`: canonical task definition list (X-macro).
- Every row defines:
  - `NAME`: symbolic task name
  - `ENABLED`: default enable state
  - `START`: first execution tick
  - `INTERVAL`: period in ms (later converted to ticks)
  - `EXEC`: callback function pointer
- This list is reused to generate enum IDs and initialize task entries.

`TASKS_NAME_TO_ID(NAME)`: converts a symbolic task name into `TasksId` (`TASKS_ID_##NAME`).

`TasksId`: enum of all task IDs plus `TASKS_ID_COUNT`.
- Generated from `TASKS_X_LIST`.
- Used as index in the internal task array.

`tasks_callback`: callback signature for task handlers (`void (*)(void)`).

`Task`: runtime definition of a single task.
- `.enabled`: whether callback can run
- `.id`: stable task ID
- `.start`: first execution tick
- `.interval`: execution period in ticks (`0` means one-shot)
- `.exec`: callback pointer

`TasksReturnCode`: return code enum.
- `TASKS_INVALID_ID`
- `TASKS_OK`

`_TaskHandler`: internal handler.
- `.tasks[TASKS_COUNT]`: dense array indexed by `TasksId`

## Task list (`TASKS_X_LIST`)

The current list defines these tasks (exact order is important because it also defines enum ordering):

- `SEND_MAINBOARD_VERSION`
- `SEND_CELLBOARD_0_VERSION`
- `SEND_CELLBOARD_1_VERSION`
- `SEND_CELLBOARD_2_VERSION`
- `SEND_CELLBOARD_3_VERSION`
- `SEND_CELLBOARD_4_VERSION`
- `SEND_CELLBOARD_5_VERSION`
- `SEND_STATUS`
- `SEND_BALANCING_STATUS`
- `SEND_CURRENT`
- `SEND_POWER`
- `SEND_TS_VOLTAGE`
- `SEND_CELLS_VOLTAGE`
- `START_CELLS_VOLTAGE_STATS`
- `SEND_CELLS_TEMPERATURE`
- `START_CELLS_TEMPERATURE_STATS`
- `SEND_COOLING_TEMPERATURE`
- `SEND_FEEDBACK_STATUS`
- `SEND_FEEDBACK_DIGITAL`
- `SEND_FEEDBACK_ANALOG`
- `SEND_FEEDBACK_ANALOG_SD`
- `SEND_IMD_STATUS`
- `SEND_CELLBOARD_SET_BALANCING_STATUS`
- `SEND_ERRORS`
- `READ_DIGITAL_FEEDBACKS`
- `START_ANALOG_CONVERSION_FEEDBACKS`
- `UPDATE_FEEDBACKS_STATUS`
- `START_INTERNAL_VOLTAGE_CONVERSION`

### Notes about start/interval semantics

- `start` is in ticks, not milliseconds.
- `interval` is provided in ms in the X-list and converted at init with `TIMEBASE_TIME_TO_TICKS(interval_ms, resolution_ms)`.
- `interval == 0` means one-shot task (executed once at `start`, then never rescheduled).

## Functions

### Internal callbacks

Most internal callbacks in `tasks.c` package data from another module and enqueue a CAN frame via `can_comm_tx_add(...)`.

Main groups:
- Identity/version:
  - `_tasks_send_mainboard_version`
  - `_tasks_send_cellboard_version` + wrappers for cellboards 0..5
- FSM / status:
  - `_tasks_send_hv_status`
  - `_tasks_send_hv_balancing_status`
  - `_tasks_send_hv_imd_status`
- Electrical values:
  - `_tasks_send_hv_current`
  - `_tasks_send_hv_power`
  - `_tasks_send_hv_ts_voltage`
  - `_tasks_send_hv_cells_voltage`
  - `_tasks_send_hv_cells_voltage_stats`
  - `_tasks_send_hv_cells_temperature`
  - `_tasks_send_hv_cells_temperature_stats`
  - `_tasks_send_hv_cooling_temperature`
- Feedback data:
  - `_tasks_send_hv_feedback_status`
  - `_tasks_send_hv_feedback_digital`
  - `_tasks_send_hv_feedback_analog`
  - `_tasks_send_hv_feedback_analog_sd`
- Balancing/error commands:
  - `_tasks_send_cellboard_set_balancing_status`
  - `_tasks_send_errors`
- Sensor pipeline control:
  - `_tasks_read_digital_feedbacks`
  - `_tasks_start_analog_conversion_feedbacks`
  - `_tasks_update_feedbacks_status`
  - `_tasks_start_internal_voltage_conversion`

### Public API

`tasks_init(resolution)`
- If `resolution == 0`, forces it to `1`.
- Expands `TASKS_X_LIST` and fills every `Task` in `htasks.tasks[]`.
- Converts each task interval from ms to ticks with `TIMEBASE_TIME_TO_TICKS`.
- Returns `TASKS_OK`.

`tasks_set_enable(id, enabled)`
- Validates `id < TASKS_ID_COUNT`.
- Sets `htasks.tasks[id].enabled`.
- Returns `TASKS_INVALID_ID` on invalid ID, otherwise `TASKS_OK`.

`tasks_is_enabled(id)`
- Returns `false` on invalid ID.
- Otherwise returns task enabled flag.

`tasks_get_task(id)`
- Returns pointer to task entry on valid ID.
- Returns `NULL` on invalid ID.

`tasks_get_start(id)`
- Returns start tick for valid ID.
- Returns `0` on invalid ID.

`tasks_get_interval(id)`
- Returns interval ticks for valid ID.
- Returns `0` on invalid ID.

`tasks_get_callback(id)`
- Returns callback pointer for valid ID.
- Returns null pointer for invalid ID.

## Runtime logic and integration with timebase

`tasks.c` itself does not execute tasks. It only stores metadata and callbacks.
Execution is done by `timebase_routine()`:
1) it pops due scheduled task entries from the min-heap
2) checks `task->enabled`
3) calls `task->exec()` if enabled
4) reschedules if `interval > 0`

So `tasks_set_enable()` toggles whether a callback can run, but does not remove/reinsert entries from the scheduler heap.

## Conditional compilation behavior

If `CONF_TASKS_MODULE_ENABLE` is disabled, API calls are replaced with no-op/default macros:
- `tasks_init` returns `TASKS_OK`
- setters/getters return neutral defaults

This keeps call sites buildable but disables runtime behavior.

## Important implementation notes

- X-macro pattern ensures enum IDs and initialization table stay in sync.
- Interval conversion is integer division; if `interval_ms < resolution_ms`, resulting ticks can be `0` (one-shot behavior).
- `tasks` data is held in a static internal handler (`_STATIC _TaskHandler htasks`), so there is one global instance.

## Strings section

When `CONF_TASKS_STRINGS_ENABLE` is enabled, additional arrays are compiled:
- module name string
- return code names and descriptions
- task ID names generated with X-macro

Current source has minor issues in this section (for maintainers):
- missing comma in `tasks_return_code_description`
- `TASKS_X` macro signature used for names does not match `TASKS_X_LIST` parameter count
- typo in end comment: `CONF_TASKS_STRINGS_ENALBE`

These do not affect runtime logic unless the strings block is compiled and used.
