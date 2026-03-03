# timebase.c
Timebase scheduler used to execute periodic tasks and watchdog timeouts.

This module is the orchestration core:
- tracks current tick/time
- schedules tasks using a min-heap
- schedules watchdog deadlines using another min-heap
- executes callbacks when deadlines are reached

## Structures, enums and macros


```TimebaseReturnCode```: primary return code for timebase functions
- ```TIMEBASE_OK```
- ```TIMEBASE_NULL_POINTER```
- ```TIMEBASE_DISABLED```
- ```TIMEBASE_BUSY```
- ```TIMEBASE_WATCHDOG_NOT_REGISTERED```
- ```TIMEBASE_WATCHDOG_UNAVAILABLE```

```TimebaseScheduledTask```
- ```.t```: due tick.
- ```.task```: pointer to ```Task```.

```TimebaseScheduledWatchdog```
- ```.t```: timeout tick.
- ```.watchdog```: pointer to ```Watchdog```.

```_TimebaseHandler``` (internal global state).
- ```.enabled```: scheduler enable flag.
- ```.resolution```: milliseconds per tick.
- ```.t```: current tick counter (volatile).
- ```.scheduled_tasks```: min-heap of scheduled tasks.
- ```.scheduled_watchdogs```: min-heap of scheduled watchdogs.

## Scheduling model

The module uses two independent min-heaps:

1) Task heap
- Contains one entry per active periodic/one-shot task.
- Ordered by due tick.
- Earliest task is always at heap top.

2) Watchdog heap
- Contains one entry per running watchdog.
- Ordered by timeout tick.
- Earliest watchdog timeout is always at heap top.

## Functions

### Compare functions
These functions are used in the min heap to keep it ordered, if element ```a<b``` return -1 else 1, the only way to return 0 is if both pointer and time is equal.

```_timebase_task_compare(a, b)```
- Primary key: due tick (```t```).
- Equality requires same tick and same task pointer.
- If ticks equal but pointers differ, returns 1 by design to avoid unnecessary swaps.

```_timebase_watchdog_compare(a, b)```
- If watchdog pointers equal -> equal (```0```).
- Otherwise compares by timeout tick.
- If timeout ticks equal and pointers differ, current code can still return ```0``` (important caveat).


```timebase_init(resolution_ms)```
- Zero-initializes handler.
- Sets ```.enabled = false```.
- Sets resolution to ```1``` if argument is ```0```, otherwise argument value.
- Calls ```tasks_init(resolution_ms)```.
- Initializes task heap and inserts all tasks with ```t = tasks_get_start(i)```.
- Initializes empty watchdog heap.
- Returns ```TIMEBASE_OK```.

```timebase_set_enable(enabled)```
- Enables/disables scheduler processing.
- Called in post to enable the timebase.

```timebase_inc_tick()```
- If disabled: returns ```TIMEBASE_DISABLED```.
- If enabled: increments tick counter and returns ```TIMEBASE_OK```.
- Called inside ```HAL_TIM_PeriodElapsedCallback``` with a timer period of 1 ms.

```timebase_get_tick()```
- Returns current tick counter.

```timebase_get_time()```
- Returns current elapsed time in ms (```ticks * resolution```).

```timebase_get_resolution()```
- Returns configured tick resolution in ms.

```timebase_register_watchdog(watchdog)```
- Builds temp key and checks if already present in watchdog heap (```TIMEBASE_BUSY```).
- Sets deadline to ```current_tick + watchdog->timeout```.
- Inserts into heap.
- Returns ```TIMEBASE_WATCHDOG_UNAVAILABLE``` if heap is full.

```timebase_unregister_watchdog(watchdog)```
- Finds watchdog in heap.
- Removes entry if found.
- Returns ```TIMEBASE_WATCHDOG_NOT_REGISTERED``` if not found.

```timebase_is_registered_watchdog(watchdog)```
- Returns whether watchdog exists in heap.

```timebase_update_watchdog(watchdog)```
- Validates pointer.
- Finds existing heap entry.
- Removes old entry.
- Computes new deadline as ```current_tick + timeout```.
- Reinserts with new deadline.
- If reinsertion fails, watchdog stays unregistered and function returns ```TIMEBASE_WATCHDOG_UNAVAILABLE```.

```timebase_routine()```
- Returns ```TIMEBASE_DISABLED``` if scheduler disabled.
- Otherwise runs two loops:

1) Task execution loop
- While heap top task exists and ```task_due_tick <= current_tick```:
  - remove top item
  - copy current tick into local ```t``` (to avoid interrupt shenanigans)
  - compute next due tick as ```t + interval```
  - execute callback only if task is enabled
  - reinsert only if ```interval > 0``` (periodic task)

2) Watchdog timeout loop
- While heap top watchdog exists and ```wdg_due_tick <= current_tick```:
  - remove top item
  - call ```watchdog_timeout(wdg.watchdog)```

Returns ```TIMEBASE_OK``` when done.
