# watchdog.c
Generic watchdog abstraction integrated with `timebase`.

A watchdog instance is a software timer with timeout callback:
- when started, it is registered in the timebase watchdog heap
- when reset, its deadline is moved forward
- when deadline is reached, timeout callback is executed

## Structures and enums

`WatchdogReturnCode`
- `WATCHDOG_OK`
- `WATCHDOG_NULL_POINTER`
- `WATCHDOG_BUSY`
- `WATCHDOG_TIMED_OUT`
- `WATCHDOG_NOT_RUNNING`
- `WATCHDOG_UNAVAILABLE`

`watchdog_timeout_callback_t`
- callback type invoked on timeout (`void (*)(void)`).

`Watchdog`
- `.running`: whether currently started
- `.timed_out`: latch indicating timeout has occurred
- `.timeout`: timeout duration in ticks
- `.expire`: callback executed on timeout

## Internal helper

`_watchdog_timeout_dummy()`
- empty callback used in deinit path to avoid null callback dereference.

## Functions

`watchdog_init(watchdog, timeout, expire)`
- Validates pointers (`watchdog` and `expire`).
- Returns `WATCHDOG_BUSY` if watchdog is already marked as running.
- Clears structure with `memset`.
- Sets timeout and callback.
- Returns `WATCHDOG_OK`.

`watchdog_deinit(watchdog)`
- Validates pointer.
- Unregisters from timebase (best effort).
- Clears structure with `memset`.
- Sets callback to dummy handler.
- Returns `WATCHDOG_OK`.

`watchdog_start(watchdog)`
- Validates pointer.
- Fails with `WATCHDOG_BUSY` if already running.
- Fails with `WATCHDOG_TIMED_OUT` if timeout latch is set.
- Registers in timebase (`timebase_register_watchdog`).
- On registration capacity failure, returns `WATCHDOG_UNAVAILABLE`.
- Sets `.running = true` on success.

`watchdog_stop(watchdog)`
- Validates pointer.
- Requires running state; otherwise `WATCHDOG_NOT_RUNNING`.
- Fails if already timed out (`WATCHDOG_TIMED_OUT`).
- Unregisters from timebase and clears running flag.
- Returns `WATCHDOG_OK`.

`watchdog_restart(watchdog)`
- Validates pointer.
- If currently running, unregisters and clears running flag.
- Registers again in timebase regardless of timed-out state.
- Clears timed-out latch.
- Sets running true.
- Returns `WATCHDOG_UNAVAILABLE` if registration fails.

`watchdog_reset(watchdog)`
- Validates pointer.
- Requires running state.
- Fails if timed out.
- Calls `timebase_update_watchdog` to move deadline to `now + timeout`.
- Returns `WATCHDOG_UNAVAILABLE` if update reinsertion fails.

`watchdog_timeout(watchdog)`
- Validates pointer.
- Requires running and not timed-out state.
- Sets `.timed_out = true` and `.running = false`.
- Executes `expire()` callback.
- Returns `WATCHDOG_OK`.

`watchdog_is_timed_out(watchdog)`
- Null-safe false.
- Returns timed-out latch.

## State machine logic

Conceptual states:
- `Idle`: running=false, timed_out=false
- `Running`: running=true, timed_out=false
- `TimedOut`: running=false, timed_out=true

Transitions:
- `watchdog_start`: Idle -> Running
- `watchdog_reset`: Running -> Running (deadline shifted)
- `watchdog_stop`: Running -> Idle
- `watchdog_timeout`: Running -> TimedOut
- `watchdog_restart`: Idle/Running/TimedOut -> Running (timed_out cleared)
- `watchdog_deinit`: any state -> zeroed/deinitialized

## Integration with timebase

`watchdog.c` does not track elapsed ticks itself. It delegates all timing to `timebase`:
- start/restart -> `timebase_register_watchdog`
- stop/deinit -> `timebase_unregister_watchdog`
- reset -> `timebase_update_watchdog`
- timeout execution happens when `timebase_routine` pops due watchdogs and calls `watchdog_timeout`

That means watchdog behavior depends on:
- periodic `timebase_inc_tick()` calls
- regular `timebase_routine()` calls
- active `timebase_set_enable(true)`

## Important behavior details

- A timed-out watchdog cannot be started with `watchdog_start`; use `watchdog_restart`.
- `watchdog_timeout` itself does not call `timebase_unregister_watchdog`.
  - In normal timeout flow this is fine because timebase removes heap entry before calling it.
- Timeout unit is ticks, not milliseconds.
  - Convert from ms to ticks before init if needed.

## Error mapping summary

- null watchdog pointers -> `WATCHDOG_NULL_POINTER`
- illegal lifecycle operation (already running/not running/timed out) -> lifecycle-specific code
- timebase capacity/update constraints -> `WATCHDOG_UNAVAILABLE`

## Conditional compilation behavior

If `CONF_WATCHDOG_MODULE_ENABLE` is disabled, APIs become no-op/default macros.

If `CONF_WATCHDOG_STRINGS_ENABLE` block is compiled in current source, maintainers should note issues:
- duplicate symbol name for return-code string arrays
- missing comma in description array
- typos in text

These do not change core runtime logic when strings are not used.
