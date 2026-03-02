# post.c
This module handles the Power-On Self Test (POST) for the mainboard. Its job is to initialize all BMS modules in a strict order and run the required startup setup so the system can safely enter normal operation.

## Structures and enums

```PostReturnCode```: standard return code for post functions.
- ```POST_OK```: function executed successfully.
- ```POST_UNINITIALIZED```: at least one module did not initialize correctly.
- ```POST_SETUP_ERROR```: a setup step failed after initialization.
- ```POST_NULL_POINTER```: one or more required callbacks were NULL.

```PostInitData```: structure containing all callback pointers required by the modules initialized in POST.
- ```.system_reset```: MCU reset callback used by programmer module.
- ```.cs_enter```, ```.cs_exit```: critical section callbacks (stored in init data, THESE FUNCTIONS ARE UNUSED BUT THEY SHOULD BE USED WITH THE RING BUFFER, WHY AREN'T THEY??).
- ```.can_send```: CAN transmit callback used by CAN communication.
- ```.led_set```, ```.led_toggle```: LED control callbacks.
- ```.imd_start```: callback to start IMD PWM measurements.
- ```.pcu_set```, ```.pcu_toggle```: PCU pin control callbacks.
- ```.feedback_read_all```, ```.feedback_start_conversion```: feedback acquisition callbacks.
- ```.display_set```, ```.display_toggle```: 7-segment display callbacks.
- ```.spi_send```, ```.spi_send_receive```: SPI callbacks for internal voltage module.

## Functions

```_post_modules_init```: internal helper that initializes all modules. It always initializes ```error``` and ```identity``` first. Then it initializes the remaining modules in a fixed order (timebase, pcu, volt, current, can, programmer, led, imd, feedback, display, internal-voltage, bal). If ```error_init``` fails, it returns ```POST_UNINITIALIZED```.

```_post_module_setup```: internal helper that configures modules after initialization.
- Resets all PCU outputs.
- Enables timebase and CAN communication.
- Waits for current sensor startup time and starts current communication watchdog.
- Returns ```POST_SETUP_ERROR``` if watchdog startup fails.

```post_run```: public call to run POST.
- Validates all required callbacks from ```PostInitData```.
- Returns ```POST_NULL_POINTER``` if any mandatory callback is NULL.
- Calls ```_post_modules_init``` and stops immediately if it fails.
- Calls ```_post_module_setup``` and returns its result.

## Note
This module does not have tests as there is no underlying logic to test if not for null pointer validation, just be careful.
