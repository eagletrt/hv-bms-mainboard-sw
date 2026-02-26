# identity.c
Module that stores the information about the board. Its main job is to send periodically its version and the cellboard versions (its build time).

## Structures and enums

```_IdentityHandler```: standard return code for bal functions, can either return ok. null pointer error or watchdog error.

```BalParams```: the two balancing parameters that can be changed either by the handcart or by the steering wheel.

```_IdentityHandler```: this is the main handler that contains all relevant information for the module.
 - ```.build_time```: contains the UNIX timestamp of the last build time.
 - ```.mainboard_version_payload```: the payload (defined in primary_network) that contains the unix build time of the mainboard firmware and the canlib firmware.
 - ```.cellboard_version_payload```:  an array of payloads (defined in primary_network) that contains the unix build time of the mainboard firmware, the canlib firmware and the cellboard ID.


## Functions

```identity_init```: initializes the mainboard payload.

```identity_get_build_time```: gets the build time (unused).

```identity_get_mainboard_version_payload```: gets called by ```_tasks_send_mainboard_version``` every second to be added to the outgoing primary network mailbox. It contains firmware build time and canlib build time.

```identity_get_cellboard_version_payload```: gets called by ```_tasks_send_cellboard_version``` every second to be added to the outgoing primary network mailbox. It contains firmware build time, canlib build time and cellboard ID.

```identity_cellboard_version_handle```: the callback that is called every time a payload is recieved from a cellboard. Updates the ```.cellboard_version_payload``` array.