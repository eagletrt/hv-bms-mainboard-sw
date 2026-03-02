# internal-voltage.c
Module that controls internal voltage measurements.

## Structures and enums

```InternalVoltageReturnCode```: either ok or null pointer error.

```InternalVoltageChannel```: all the channels of the max22530 module:
- ```INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE```: TS voltage.
- ```INTERNAL_VOLTAGE_CHANNEL_PACK_VOLTAGE```: battery pack voltage.
- ```INTERNAL_VOLTAGE_CHANNEL_IMD_TS_CONNECTED```: Feedback on the TS connection to the IMD (NOT IMPLEMENTED)
- ```INTERNAL_VOLTAGE_CHANNEL_PRECHARGE_TEMPERATURE```: Temperature of the precharge resistors heatsink (NOT IMPLEMENTED)

```_InternalVoltageHandler```: this is the main handler that contains all relevant information for the module.
 - ```.max22530```: handler for the max22530 module.
 - ```.ts```: ts voltage.
 - ```.pack```:  pack voltage.
 - ```.ts_voltage_can_payload```: can payload for voltage.


## Functions

```internal_voltage_init```: initializes the max22530 module with the proper spi callbacks.

```internal_voltage_read_all```: reads the ts and pack voltages, TODO update the imd ts connected feedback and precharge temps.

```internal_voltage_get_ts```: gets the ts voltage.

```internal_voltage_get_pack```: gets the pack voltage.

```internal_voltage_get_ts_voltage_canlib_payload```: updates and returns the canlib status.