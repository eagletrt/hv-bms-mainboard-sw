# can-comm.c
Module that handles communication between the board and the network.

## Structures and enums

```CanCommReturnCode```: the return code for the standart can function.

```CanCommEnableBit```: contains the enabled status of the rx and tx channels.

```CanMessage```: the standard message structure needed for the send function.

```_CanCommHandler```: this is the main handler that contains all relevant information for the class.
 - ```.enabled```: contains the tx and rx enablad flags.
 - ```.tx_busy```: contains an array that contains the presence/absence of a certain message in the queue (use to discard duplicate messages).
 - ```.rx_busy```: same as tx_buisy but with no functionally (unused).
 - ```.tx_buf```: the ring buffer for the transmit queue.
 - ```.rx_buf```: the ring buffer for the recieve queue.
 - ```.send```: the callback of the hardware send function.
  - ```rx_device```: the canlib device. (what this??)
 - ```.rx_raw```: an array for the raw incoming message.
 - ```.rx_conv```: an array for the deserialized incoming message.

## Functions

```can_comm_init```: requires a pointer to the ```can_send``` function that immediately adds the message to the mailbox of the correct network. It initializes both ring buffers for tx and rx, initializes a canlib device (what is this??) and disables all can flags (see flag handlers).

```can_comm_send_immediate```: this function immediately sends a given message to the specified can network. It requires the network (either primary or bms), the index of the message (defined in either bms_network.h or primary_network.h), the frame type (either data or remote, but remote it is yet unused), the size of the message (WARNING: THE CHECK TO CHECK THAT THE MESSAGE ISN'T LONGER THAN THE MAX IS COMMENTED OUT), and the data itself. Then the function checks if the queue is full, if it is it runs the ```can_comm_routine``` to free up space and then pushes the message to the front before rerunning the routine to send the message.

```can_comm_tx_add```: this function adds a message to the transmit queue, it will be sent the next time the ```can_comm_routine``` is run (see ```can_comm_send_immediate``` for parameters). This function also checks if there is a message in the queue with the same index (through the ```tx_busy``` array) and if there is it just returns ```CAN_COMM_OK``` (i don't know if this is good behaviour to have as it can seem that the message has been sent even though in reality it has been trashed...).

```can_comm_rx_add```: this funciton is called by the ```can.h``` hardware interrupt and just adds the incoming message to the recieve queue and sets the ```rx_busy``` for the array (that is unused).

```can_comm_routine```: this is the main routine that is called periodically by the fsm in: idle, fatal, do_balancing, do_airn_check, do_precharge_check, do_airp_check, do_ts_on. 
 - Send cycle: for every message that is in the ring buffer transmit queue the routine pops it, clears the buisy flag, serializes it based on ID and network and then sends it with the ```hcan_comm.send``` funciton.
 - Recieve cycle: for every message that is in the ring buffer recieve queue the routine pops it and then directs it to the correct function calling then the specific handler, the possible destinations are:  

     - ```_can_comm_bms_payload_handle```: the routing for the bms network
     
         - ```BMS_CELLBOARD_CELLS_VOLTAGE_INDEX```->```volt_cells_voltage_handle```: contains the cellboard ID of the sender and 3 voltages (???) that update the volt handler.
         - ```BMS_CELLBOARD_CELLS_TEMPERATURE_INDEX```->```temp_cells_temperature_handle```: contains the cellboard ID of the sender and 4 temperatures that update the temperatures handler.
         - ```BMS_CELLBOARD_FLASH_RESPONSE_INDEX```->```programmer_cellboard_flash_response_handle```: contains a ready flag that signals that the cellboard is ready to be flashed.
         - ```BMS_CELLBOARD_STATUS_INDEX```->```fsm_cellboard_state_handle```: if the status is fatal enter the fsm fatal status.
         - ```BMS_CELLBOARD_VERSION_INDEX```->```identity_cellboard_version_handle```: update the hidentity struct with the cellboard information (canlib and component build time).
         - ```BMS_CELLBOARD_BALANCING_STATUS_INDEX```->```bal_cellboard_balancing_status_handle```: calls the function that updates the struct that gets forwrded constantly in the main network, it contains cellboard ID, status and the discharging status of each cell. (how can all cellboards be forwarded if they get overwritten???)
         - ```BMS_IVT_MSG_RESULT_I_INDEX```->```current_handle```: resets the watchdog for the current and checks the value. (what sends this??)
         - ```BMS_CELLBOARD_ERROR_INDEX```->```error_cellboard_handle```: sets an error and adds to it the cellboard group and ID.
    - ```_can_comm_primary_payload_handle```: the routing for the primary
     
         - ```PRIMARY_HV_FLASH_REQUEST_INDEX```->```programmer_flash_request_handle```: contains the cellboard ID to be flashed, if fsm is in either fatal (even fatal??) or idle restart the programmer watchdog and trigger the flash event.
         - ```PRIMARY_HV_FLASH_INDEX```->```programmer_flash_handle```:this packet either starts or stops the flashing procedure. A flash request is needed before this.
         - ```PRIMARY_HV_SET_STATUS_ECU_INDEX```->```pcu_set_state_from_ecu_handle```: trigger the TS_ON/TS_OFF states (sent from ecu).
         - ```PRIMARY_HV_SET_STATUS_HANDCART_INDEX```->```pcu_set_state_from_handcart_handle```: trigger the TS_ON/TS_OFF states (sent from handcart).
         - ```PRIMARY_HV_SET_BALANCING_STATUS_STEERING_WHEEL_INDEX```->```bal_set_balancing_state_from_steering_wheel_handle```: trigger the start and stop of balancing states from the steering wheel. This message is treated as a heartbeat to keep alive the balancing, if this type of message isn't recieved every 3 seconds then the balancing automatically stops. 
        - ```PRIMARY_HV_SET_BALANCING_STATUS_HANDCART_INDEX```->```bal_set_balancing_state_from_handcart_handle```: trigger the start and stop of balancing states from the handcart. This message is treated as a heartbeat to keep alive the balancing, if this type of message isn't recieved every 3 seconds then the balancing automatically stops. 

### Flag handlers
(this seems extremely overcomplicated and overredundant)
These are the helper functions that through the homonymous macros set the ```hcan_comm.enabled``` bit flag (it is composed by 2 bits ```CAN_COMM_RX_ENABLE_BIT``` and ```CAN_COMM_TX_ENABLE_BIT```). They are all functionally unused, they just act as a check to confirm correct post procedure as at initialization time the flag is set to all disabled and then at the end of the post process it is enabled all (and remains untouched thereafter). 
 - ```can_comm_enable_all```: Enables both the rx and tx flags.
 - ```can_comm_disable_all```: Disables both the rx and tx flags.
 - ```can_comm_is_enabled_all```: Returns true if all flags are set.
 - ```can_comm_enable```: Enables a specific flag.
 - ```can_comm_disable```: Disables a specific flag.
 - ```can_comm_is_enabled```: Returns true if a certain flag is enabled.