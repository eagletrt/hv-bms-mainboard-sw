# pcu.c
Pack control unit, handles state transitions and precharge/startup procedures and watchdogs.
## Structures and enums

```PcuReturnCode```: standard return code, either ok or null pointer.

```PcuPinStatus```: status of a given pin (either low or high).

```PcuPin```: the enumerator.
 - ```PCU_PIN_AIR_NEGATIVE```: pin connected to the AIR- (the relay connected to the negative side of the battery pack).
 - ```PCU_PIN_AIR_POSITIVE```: pin connected to the AIR+ (the relay connected to the positive side of the battery pack).
 - ```PCU_PIN_PRECHARGE```: pin connected to the precharge circuit (WHAT DOES THIS DO??).
 - ```PCU_PIN_AMS```: pin connected to the ams circuit (WHAT DOES THIS DO??).

```_PcuHandler```: this is the main handler that contains all relevant information for the class.
 - ```.set```: callback to the function to set a pin.
 - ```.toggle```: callback to the function to togglo a pin.
 - ```.event```: 
 - ```.timeout_event```: 
 - ```.airn_watchdog```: the pointer to the watchdog for the AIR-.
 - ```.precharge_watchdog```: the pointer to the watchdog for the precharge procedure.
  - ```.airp_watchdog```: the pointer to the watchdog for the AIR+.


## Functions

```_pcu_airn_timeout```: callback for the AIR- watchdog timeout, triggers the ```FSM_EVENT_TYPE_AIRN_TIMEOUT``` event.

```_pcu_airp_timeout```: callback for the AIR+ watchdog timeout, triggers the ```FSM_EVENT_TYPE_AIRP_TIMEOUT``` event.

```_pcu_precharge_timeout```: callback for the AIR+ watchdog timeout, triggers the ```FSM_EVENT_TYPE_PRECHARGE_TIMEOUT``` event.

```_pcu_init_watchdogs```: sets up all the watchdogs:
- AIR-: 1000ms
- AIR+: 1000ms
- precharge: 15000ms

```pcu_reset_all```: resets all the pins to their respective defaults (all high???) and deinits and reinits the watchdogs.

```pcu_airn_open```: stops the AIR- watchdog and opens the relay.

```pcu_airn_close```: starts the AIR- watchdog and closes the relay.

```pcu_airn_stop_watchdog```: stops the AIR- watchdog.

```pcu_airp_open```: stops the AIR+ watchdog and opens the relay.

```pcu_airp_close```: starts the AIR+ watchdog and closes the relay.

```pcu_airp_stop_watchdog```: stops the AIR+ watchdog.

```pcu_precharge_start```: starts the precharge watchdog and closes the precharge relay.

```pcu_precharge_stop```: stops the precharge watchdog and opens the precharge relay

```pcu_precharge_stop_watchdog```: stops the precharge watchdog.

```pcu_ams_activate```: sets the AMS pin low.

```pcu_ams_deactivate```: sets the AMS pin high.

```pcu_get_precharge_percentage```: gets the difference between the tractive system voltage and the battery pack voltage in %.

```pcu_is_precharge_complete```: checks if the precharge percentage is above the threshold (95%).

```pcu_set_state_from_ecu_handle```: can callback from the ECU that triggers the TS_ON/TS_OFF event in the fsm.

```pcu_set_state_from_handcart_handle```: can callback from the handcart that triggers the TS_ON/TS_OFF event in the fsm.

(for a detailed explanation of how these functions are used check [power_on](power_on.md))