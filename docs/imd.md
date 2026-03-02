# imd.c
Insulation monitoring device handling functions. It uses an [input capture timer](https://community.st.com/t5/stm32-mcus/how-to-use-the-input-capture-feature/ta-p/704161) to keep the ir1553204 driver updated.

## Structures and enums

```ImdReturnCode```: standard return code for imd functions, can either return ok. null pointer error or invalid data.

```ImdStatus```: the possible status of the IMD, they are directly related to the frequenct of the pwm signal:
- ```IMD_STATUS_UNKNOWN```: unknown Imd status.
- ```IMD_STATUS_SHORT_CIRCUIT```: short circuit detected.
- ```IMD_STATUS_SHORT_NORMAL```: normal operational state.
- ```IMD_STATUS_UNDER_VOLTAGE```: detected voltage is too low.
- ```IMD_STATUS_START_MEASURE```: the IMD has started the measure.
- ```IMD_STATUS_DEVICE_ERROR```: general device error.
- ```IMD_STATUS_EARTH_FAULT```: earth fault detected.


```_ImdHandler```: this is the main handler that contains all relevant information for the module.
 - ```.start```: contains the funciton to start the timer and the callback.
 - ```.ir1153204```: the handler for the drivers.
 - ```.status_can_payload```:  the payload containing status, frequency, duty cycle and the feedback statuses of the cockpit leds.


## Functions

```imd_init```: initializes the driver and starts the timer.

```imd_get_duty_cycle```: gets the duty cycle.

```imd_get_frequency```: gets the frequency.

```imd_get_period```: gets the period.

```imd_get_status```: gets the status.

```imd_update```: function called by the ```HAL_TIM_IC_CaptureCallback``` to update the values.

```imd_get_status_canlib_payload```: updates and returns the status payload for the canlib.