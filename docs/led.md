# led.c
Module that controls the on-board LEDs.

## Structures and enums

`LedStatus`: status of a single LED.
- `LED_STATUS_OFF`: LED is turned off.
- `LED_STATUS_ON`: LED is turned on.

`LedId`: identifier of the available LEDs.
- `LED_ID_1`: first LED.
- `LED_ID_2`: second LED.

`led_set_state_callback_t`: callback used to set the state of a selected LED.

`led_toggle_state_callback_t`: callback used to toggle the state of a selected LED.

`LedReturnCode`: standard return code for LED module functions.
- `LED_OK`: function executed successfully.
- `LED_NULL_POINTER`: NULL callback pointer passed to init.
- `LED_INVALID_ID`: selected LED identifier is invalid.
- `LED_INVALID_STATUS`: selected LED status is invalid.

`_LedHandler`: internal handler structure for the module.
 - `.set`: callback used to set LED state.
 - `.toggle`: callback used to toggle LED state.

## Functions

`led_init`: initializes the internal LED handler, clears its state, and stores the set/toggle callbacks.

`led_set_status`: validates LED id and status, then sets the selected LED status through the configured callback.

`led_toggle_status`: validates LED id, then toggles the selected LED status through the configured callback.
