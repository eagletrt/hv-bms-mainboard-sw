# current.c
Module that recieves the current from a sensor on the bms can network checks the values and relays it.

## Structures and enums

`_CurrentHandler`: the main handler for the module.
- current: the current in ampere.
- sensor_wdg: the pointer to the watchdog.
- current_can_payload: the can payload for the current.
- power_can_payload: the can payload for the power.

## Functions

`_current_sensor_communcation_timeout`: sets the error flag for the current sensor communication, this is called by the watchdog, if this error repeats more than once the FSM goes into the fatal state.

`_current_check_value`: this funciton checks the current value:
if it is outside the correct range it sets the `ERROR_GROUP_OVER_CURRENT` (tresh 2) and if the power is over the max range `ERROR_GROUP_OVER_POWER` (tresh 2).

`current_init`: this function clears the handler and starts the watchdog (timeout 200ms).

`current_get_current`: get the current from the structure.

`current_get_power`: get the current power calculated with the internal voltage. (there is a mismatch between units of measure, why is the current in ampere, the voltages in volt but then the funciton still divides by 1000???)
 
`current_start_sensor_communication_watchdog`: gets called in post after a delay time to let it start `CURRENT_SENSOR_STARTUP_TIME_MS` to start the watchdog.
 
`current_handle`: this is the callback that updates the internal handle with the new can data and resets the watchdog.
 
`current_get_current_canlib_payload`: the function called periodically in `_tasks_send_hv_current` every 50ms to send current data in can.

`current_get_power_canlib_payload`: the function called periodically in `_tasks_send_hv_power` every 50ms to send power data in can.

