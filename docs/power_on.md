# Power on procedure

<img src="../assets/fsm/fsm.png">

## Idle state
In idle state wait for a ```FSM_EVENT_TYPE_TS_ON``` fired event that comes from from either the ECU or the handcart.ù
If the [```FEEDBACK_IDLE_TO_AIRN_CHECK_MASK```](feedback.md/#transition-mask-idle-airn_check) checks out correctly then the fsm transitions on, else it sends an enzomma payload with the failed check in can and stays in Idle.

### Idle to AIRN_CHECK
Calls ```pcu_airn_close``` that closes the AIR- relay and starts the AIR- watchdog.

## AIRN_CHECK
Possible paths:
- The AIR- watchdog times out: check what values prevented the switch to precharge, send the enzomma payload and fallback to IDLE state (reset all watchdogs).
- The [```FEEDBACK_AIRN_CHECK_TO_PRECHARGE_MASK```](feedback.md/#transition-mask-airn_check-precharge) checks out and the fsm switches to the precharge step.

### AIRN_CHECK to PRECHARGE_CHECK
Calls ```pcu_airn_stop_watchdog``` and ```pcu_precharge_start``` that starts the precharge watchdog and closes the precharge relay.

## Precharge check
Shows the precharge percentage on the 7 segment display.
Possible paths:
- The precharge watchdog times out: check what values prevented the switch to airp check, send the enzomma payload and fallback to IDLE state (reset all watchdogs and relays).
- If the [```FEEDBACK_PRECHARGE_TO_AIRP_CHECK_MASK```](feedback.md/#transition-mask-precharge-airp_check) checks out and the precharge percentage is above the treshold then the fsm switches to the precharge step.

### Precharge to AIRP_CHECK
Calls ```pcu_precharge_stop_watchdog``` and ```pcu_airp_close``` that closes the AIR+ relay and starts its watchdog.

## AIRP_CHECK
Possible paths:
- The AIR+ watchdog times out: check what values prevented the switch to TS_ON, send the enzomma payload and fallback to IDLE state (reset all watchdogs and relays).
- The [```FEEDBACK_AIRP_CHECK_TO_TS_ON_MASK```](feedback.md/#transition-mask-airp_check-ts_on) checks out and the fsm switches to TS_ON.

### AIRP_CHECK to TS_ON
Calls ```pcu_airp_stop_watchdog``` stops the AIR+ watchdog.

## TS_ON
To mantain the TS_ON status the [```FEEDBACK_TS_ON_MASK```](feedback.md/#transition-mask-ts_on-persistent) is checked in perpetuity, if the mask doesn't check out send the enzomma payload and fallback to idle.
Either the ECU or the handcart can send a pcu_set_handle payload that can call the TS_OFF event that makes the fsm go to its IDLE state resetting all watchdog and relays.

P.S. All states can go to fatal if there are errors detected or if the cellboards go into any error state.