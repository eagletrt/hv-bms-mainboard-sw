# programmer.c
This module handles the flashing through can of both the mainboard and the cellboards.

## Structures and enums

```PostReturnCode```: standard return code for programmer functions.
- ```PROGRAMMER_OK```: the function executed succesfully.
- ```PROGRAMMER_BUSY```: the flash procedure is ongoing.
- ```PROGRAMMER_TIMEOUT```: the procedure has timed out.

```_ProgrammerHandler```: structure containing all callback pointers required by the modules initialized in POST.
- ```.reset```: MCU reset callback used if the mainboard is the target.
- ```.flash_event```: FSM event to be called while flashing.
- ```.programmer_can_payload```: primary network response for flash request.
- ```.target```: Cellboard ID of the target.
- ```.flash_request```: flag signaling that there has been a flash request
- ```.flashing```: flag set while flashing.
- ```.flash_stop```: flag signaling that flashing has stopped.
- ```.cellboard_ready```: bit flag in which every bit represents a cellboard.
- ```.watchdog```: pointer to the flashing watchdog.

## Flashing procedure
First the transmitting controller sends a ```primary_hv_flash_request_t``` with either the mainboard flag or the cellboard ID.
If the ID is not the mainboard the mainboard starts the watchdog (todo) sends in the bms network a message to the cellboards (todo) and then tells the controller that it is ready. Once it is ready it either resets itself (and recieves the new firmware) or it serves as a relay between the primary and secondary can until the controller sends a flash requets that stops the process and returns to idle state.  


## Functions

```_programmer_cellboard_ready_all```: helper function to check if all cellboards are ready.

```_programmer_flash_timeout```: function called when the watchdog times out.

```_programmer_flash_stop```: function called when flashing stops.

```_programmer_flash_reset_flags```: resets all handler flags.

```programmer_init```: resets all flags and initializes the watchdog.

```programmer_flash_request_handle```: handler of the flash request coming from the main network, sets the target, the flash request flag, resets the watchdog and triggers the idle event.

```programmer_cellboard_flash_response_handle```: handler of the ack messages of the cellboards.

```programmer_flash_handle```: handler for the start and the end of the data stream

```programmer_routine```: routine that keeps the fsm in the flash state until flashing is concluded, also resets the mainboard if it is the target.

## Note
This module is very unfinished.