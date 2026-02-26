# feedback.c

This module provides all the hardware checks needed to change and mantain states, it handles both digital and analog feedbacks.

## Structures and Enums

```FeedbackId```: this is the definition of all possible digital states (26).
```FeedbackBit```: defines the mapping from the id to the correct bit position in the 32 bit flag, this is the final logical space, so it contains both digital and analog feedbacks so that masks can be created from it.

```FeedbackDigitalBit```: enum representing all digital inputs.
```FeedbackAnalogIndex```: enum representing all analog inputs.


```FeedbackStatus```: the main logic flag, either high or low (or error).

```_FeedbackHandler```: the main module handler, 
- ```read_digital```: the callback to the gpio function to read all digital inputs 
- ```start_conversion```: the callback to start the analog conversion
- ```digital```: the 32bit flag for all the digital inputs
- ```analog```: the voltage array for the analog inputs
- ```status```: the normalized status structure that contains all the logical states of the feedbacks 
- ```status_can_payload```, ```digital_can_payload```, ```analog_can_payload```, ```analog_sd_can_payload```, ```enzomma_can_payload```: the can payloads to be sent to the primary network.

## Functions

```_feedback_get_id_from_digital_bit```: maps from the ```FeedbackDigitalBit``` enum to the ```FeedbackId```.

```_feedback_get_id_from_analog_index```: maps from the ```FeedbackAnalogIndex``` enum to the ```FeedbackId```.

```_feedback_get_analog_status```: from the ```FeedackAnalogIndex``` enum this functions gets the raw voltage from the ```.analog``` array and returns a boolean state depending on the type of feedback (3.3v probing must be inside a range and for the others they must be outside of a range).

```feedback_init```: clears the handler and stores the read digital and start conversion functions.

```feedback_update_digital_feedback_all```: calls the read digital funciton, this is mapped to ```gpio_feedback_read_all``` in gpio through main.c and the fsm post. This already creates the 32bit flag.

```feedback_start_analog_conversion_all```: this is called in the taks ```_tasks_start_analog_conversion_feedbacks``` that is called every millisecond. (the ADC doesn't automatically reload itself right??)
```feedback_get_digital```: returns a boolean of the state of a digital feedback given the bit of the digital bitflag.

```feedback_get_analog```: returns a volt value of the state of an analog feedback given the bit of the analog bitflag.

```feedback_update_status```: updates the main logic status flag with the digital bit flag, updates the main logic status by repeatedly calling the ```_feedback_get_analog_status```.    

```feedback_get_status```: get the status from the ID from the status array.

```feedback_is_digital```: returns true if the feedback ID is digital. 

```feedback_get_digital_bit_from_id```: converts back from ID to bit

```feedback_get_analog_index_from_id```: converts back from ID to bit

```feedback_get_status_payload```: called from a task that every 50 ms sends in can the whole status array.

```feedback_get_digital_payload```: called from a task that every 50 ms sends in can the digital bits.

```feedback_get_analog_payload```: called from a task that every 50 ms sends in can the analog voltages.

```feedback_get_analog_sd_payload```: called from a task that every 50 ms sends in can the shutdown button statuses. (why is the shutdown status not in any checks???)

```feedback_get_enzomma_payload```: this is used after ```feedback_check_values``` to send in can the out value if the check fails.

```feedback_check_values```: this function needs a mask(what feedback to check), a value(what is the expected value) and a pointer to a FeedbackId variable that will be set to the feedback that fails the check. This function will then check the masked feedback with the expected values and return true if all test pass.

## Check masks

### Transition mask IDLE->AIRN_CHECK
Map used when transitioning from the IDLE state to the AIRN_CHECK state.
#### Check high
 - AIRN_OPEN_MEC
 - PRECHARGE_OPEN_MEC
 - AIRP_OPEN_MEC
 - TS_LESS_THAN_60V
 - PLAUSIBLE_STATE_PERSISTED
 - BMS_FAULT_COCKPIT_LED
 - IMD_FAULT_COCKPIT_LED
 - INDICATOR_CONNECTED
 - PLAUSIBLE_STATE_LATCHED
 - BMS_FAULT_LATCHED
 - IMD_FAULT_LATCHED
 - IMD_OK
 - TSAL_GREEN
 - PROBING_3V3
 - SD_END
 - V5_MCU

#### Check low
 - AIRN_OPEN_COM
 - PRECHARGE_OPEN_COM
 - AIRP_OPEN_COM

---

### Transition mask AIRN_CHECK->PRECHARGE
Map used when transitioning from the AIRN_CHECK state to the PRECHARGE state.
#### Check high
 - AIRN_OPEN_COM
 - PRECHARGE_OPEN_MEC
 - AIRP_OPEN_MEC
 - TS_LESS_THAN_60V
 - PLAUSIBLE_STATE_PERSISTED
 - BMS_FAULT_COCKPIT_LED
 - IMD_FAULT_COCKPIT_LED
 - INDICATOR_CONNECTED
 - BMS_FAULT_LATCHED
 - IMD_FAULT_LATCHED
 - PLAUSIBLE_STATE_LATCHED
 - IMD_OK
 - PROBING_3V3
 - SD_END
 - V5_MCU

#### Check low
 - PRECHARGE_OPEN_COM
 - AIRP_OPEN_COM
 - AIRN_OPEN_MEC
 - TSAL_GREEN

---

### Transition mask PRECHARGE->AIRP_CHECK
Map used when transitioning from the PRECHARGE state to the AIRP_CHECK state.
#### Check high
 - AIRN_OPEN_COM
 - PRECHARGE_OPEN_COM
 - AIRP_OPEN_MEC
 - PLAUSIBLE_STATE_PERSISTED
 - IMD_FAULT_COCKPIT_LED
 - BMS_FAULT_COCKPIT_LED
 - INDICATOR_CONNECTED
 - PLAUSIBLE_STATE_LATCHED
 - BMS_FAULT_LATCHED
 - IMD_FAULT_LATCHED
 - IMD_OK
 - PROBING_3V3
 - SD_END
 - V5_MCU

#### Check low
 - AIRP_OPEN_COM
 - AIRN_OPEN_MEC
 - PRECHARGE_OPEN_MEC
 - TS_LESS_THAN_60V
 - TSAL_GREEN

---

### Transition mask AIRP_CHECK->TS_ON
Map used when transitioning from the AIRP_CHECK state to the TS_ON state.
#### Check high
 - AIRN_OPEN_COM
 - PRECHARGE_OPEN_COM
 - AIRP_OPEN_COM
 - PLAUSIBLE_STATE_PERSISTED
 - BMS_FAULT_COCKPIT_LED
 - IMD_FAULT_COCKPIT_LED
 - INDICATOR_CONNECTED
 - PLAUSIBLE_STATE_LATCHED
 - BMS_FAULT_LATCHED
 - IMD_FAULT_LATCHED
 - IMD_OK
 - PROBING_3V3
 - SD_END
 - V5_MCU

#### Check low
 - AIRN_OPEN_MEC
 - PRECHARGE_OPEN_MEC
 - AIRP_OPEN_MEC
 - TS_LESS_THAN_60V
 - TSAL_GREEN

---

### Transition mask TS_ON (persistent)
Map used to maintain the TS_ON state.
#### Check high
 - AIRN_OPEN_COM
 - PRECHARGE_OPEN_COM
 - AIRP_OPEN_COM
 - PLAUSIBLE_STATE_PERSISTED
 - BMS_FAULT_COCKPIT_LED
 - IMD_FAULT_COCKPIT_LED
 - INDICATOR_CONNECTED
 - PLAUSIBLE_STATE_LATCHED
 - BMS_FAULT_LATCHED
 - IMD_FAULT_LATCHED
 - IMD_OK
 - PROBING_3V3
 - SD_END
 - V5_MCU

#### Check low
 - AIRN_OPEN_MEC
 - PRECHARGE_OPEN_MEC
 - AIRP_OPEN_MEC
 - TS_LESS_THAN_60V
 - TSAL_GREEN