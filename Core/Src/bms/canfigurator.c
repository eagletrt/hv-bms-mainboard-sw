/**
 * @file canfigurator.c
 * @date 2024-10-03
 * @author Riccardo Segala [riccardo.segala@icloud.com]
 *
 * @brief Parameters management
 */

#include "canfigurator.h"
#include "mainboard-def.h"

#ifdef CONF_CANFIGURATOR_MODULE_ENABLE

_STATIC_INLINE _CanfiguratorHandler hcanfigurator; 

typedef void (*_canfigurator_msg_handler*)(primary_canfigurator_command_converted_t*) 

void _canfigurator_set_max_volt(primary_canfigurator_command_converted_t* payload) {
    hcanfigurator.max_volt = payload->over_voltage;
}

void _canfigurator_set_min_volt(primary_canfigurator_command_converted_t* payload) {
    hcanfigurator.max_volt = payload->under_voltage;
}

void _canfigurator_set_max_temp(primary_canfigurator_command_converted_t* payload) {
    hcanfigurator.max_volt = payload->over_temperature;
}

void _canfigurator_set_min_temp(primary_canfigurator_command_converted_t* payload) {
    hcanfigurator.min_volt = payload->under_temperature;
}

_STATIC_INLINE _canfigurator_param_handler param_handlers[] = {
    [PRIMARY_CANFIGURATOR_COMMAND_SEND_MUX_SET_OVER_VOLTAGE_CHOICE] = _canfigurator_set_max_volt,
    [PRIMARY_CANFIGURATOR_COMMAND_SEND_MUX_SET_UNDER_VOLTAGE_CHOICE] = _canfigurator_set_min_volt,
    [PRIMARY_CANFIGURATOR_COMMAND_SEND_MUX_SET_OVER_TEMPERATURE_CHOICE] = _canfigurator_set_max_temp,
    [PRIMARY_CANFIGURATOR_COMMAND_SEND_MUX_SET_UNDER_TEMPERATURE_CHOICE] = _canfigurator_set_min_temp,
};

volt_t canfigurator_get_max_volt() {
    return hcanfigurator.volt_max_v;
}

volt_t canfigurator_get_min_volt() {
    return hcanfigurator.volt_min_v;
}

celsius_t canfigurator_get_max_temp() {
    return hcanfigurator.temp_max_c;
}

celsius_t canfigurator_get_min_temp() {
    return hcanfigurator.temp_min_c;
}

void canfigurator_canfig_handle(primary_canfigurator_command_converted_t * const payload) {

    if(payload->send_mux >= sizeof(param_handlers)/sizeof(_canfigurator_param_handler))
        return;

    param_handlers[payload->send_mux];
}

#endif // CONF_CANFIGURATOR_MODULE_ENABLE
