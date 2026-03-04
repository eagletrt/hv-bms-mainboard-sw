# bal.c
This module handles all the funcitons for the balancing of the batteries load. It does not make itself the calculations but it enters in balancing mode if the FSM enters from idle to balancing. Its main purpose is to enable communication between the handcart/steering wheel and cellboards.

## Structures and enums

`BalReturnCode`: standard return code for bal functions, can either return ok. null pointer error or watchdog error.

`BalParams`: the two balancing parameters that can be changed either by the handcart or by the steering wheel.

`_BalHandler`: this is the main handler that contains all relevant information for the class.
 - `.event.type`: contains the FSM events when changed by the SW or handcart, functionally unused.
 - `set_status_can_payload`: the payload (defined in bms_network) that contains the balancing status (1-0), the target voltage and the threshold voltage. This is the payload that is sent to the cellboards when balancing is active.
 - `status_can_payload`: the payload (defined in primary_network) that is relayed by the mainboard to the main network through the `_tasks_send_hv_balancing_status`, it is updated with the `bal_cellboard_balancing_status_handle`. It containes the boolean discharging value of 24 cells, the overall status and a cellboard id (WHAT DOES IT MEEEEEAN -_- :P).
 - `watchdog`: contains the pointer to the watchdog that handles the balance timeout, it is initialized at initialization and started and stopped in the respective functions.
 - `active`: the boolean status of the balancing process.
 - `params`: the set parameters that are updated through either the handcart or steering wheel callbacks and are relayed periodically to the cellboard when balancing. They are target voltage and threshold.


## Functions

`_bal_timeout`: if the cellboards are balancing and the mainboard doesn't recieve positive feedback every 3 seconds from either the handcart or the steering wheel this sends the `FSM_EVENT_TYPE_BALANCING_STOP` event trigger.

`bal_init`: initializes the module, stops by default the balancing, sets the hardcoded default for target and threshold, initializes the watchdog (off).

`bal_is_active`: returns the status of the balancing

`bal_start`: called in the transition function between idle and balancing, this gets called if the fsm_fired_event is `FSM_EVENT_TYPE_BALANCING_START`, this flag can be set by either the callbacks from the steering wheel or the handcart, it enables the task that every 50ms sends the balancing status to the cellboards.

`bal_stop`: stops the watchdog and stops the mainboard from sending any more data to the cellboards. It is called either when a fatal event occurs or when returning to idle. Transition that is called either when the 3 seconds timeout expires or if one of the two callbacks from SW or hancart return an inactive balancing status. (how do the cellboards know that the balancing has stopped?? do they just get killed by their own timeout??) (RIGUARDAREEEEEE)

`bal_set_balancing_state_from_steering_wheel_handle`: callback called from the can routine if there is a can message pending from the steering wheel. This will then update the values of the target and threshold for the cellboards, reset the watchdog (why is a watchdog needed??? why does balancing need constant positive feedback to continue?? why 3 seconds, it seems a lot ) and send an fsm trigger event to start/stop the balancing.

`bal_set_balancing_state_from_handcart_handle`:callback called from the can routine if there is a can message pending from the handcart. This will then update the values of the target and threshold for the cellboards, reset the watchdog and send an fsm trigger event to start/stop the balancing.

`bal_cellboard_balancing_status_handle`: callback called from the can routine if there is a can message pending from the cellboards to then relay the info to the main network every time the `_tasks_send_hv_balancing_status` is called, this task is enabled by default. 
(why does it handle one cellboard at a time?? is it just relaying the data sent by it??)

`bal_get_set_status_canlib_payload`: gets a pointer to the payload to send to the cellboards from the `_tasks_send_cellboard_set_balancing_status` task, this task is started once `bal_start` is called and stopped with `bal_stop`. The message is relayed every 200ms.

`bal_get_status_canlib_payload`: gets a pointer to the payload to send to the primary network from the `_tasks_send_hv_balancing_status` task, this task is enabled by default and runs every 50ms.

