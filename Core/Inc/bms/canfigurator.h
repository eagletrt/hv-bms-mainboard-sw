/**
 * @file canfigurator.h
 * @date 2024-10-03
 * @author Riccardo Segala [riccardo.segala@icloud.com]
 *
 * @brief Parameters management
 */

#ifndef CANFIGURATOR_H
#define CANFIGURATOR_H

#include "mainboard-def.h"

/**
 * @brief Parameters handler structure
 *
 * @warning This structure should never be used outside of this file
 *
 * @param max_volt_v The maximum voltage value in volt 
 * @param min_volt_v The minimum voltage value in volt 
 * @param max_temp_c The maximum temperature value in celsius 
 * @param min_temp_c The minimum temperature value in celsius 
 */
typedef struct {
    volt_t max_volt_v;
    volt_t min_volt_v;

    celsius_t max_temp_c;
    celsius_t min_temp_c;
} _CanfiguratorHandler;

#ifdef CONF_CANFIGURATOR_MODULE_ENABLE

/**
 * @brief Get the maximum voltage value in volt
 *
 * @return volt_t Voltage max value
 *
 */
volt_t canfigurator_get_max_volt();

/**
 * @brief Get the minimum voltage value in volt
 *
 * @return volt_t Voltage min value
 *
 */
volt_t canfigurator_get_min_volt();

/**
 * @brief Get the maximum temperature value in celsius
 *
 * @return celsius_t Temperature max value
 *
 */
volt_t canfigurator_get_max_temp();

/**
 * @brief Get the minimum temperature value in celsius
 *
 * @return celsius_t Temperature min value
 *
 */
volt_t canfigurator_get_min_temp();

/**
 * @brief Handle the received canfiguration
 *
 * @param payload A pointer to the canlib payload
 */
void canfigurator_canfig_handle(primary_canfigurator_command_converted_t * const payload);

/**
 * @brief Get a pointer to the CAN payload of the canfig
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_...
 */
// primary_canfigurator_command_converted_t canfigurator_get_canfig_canlib_payload(size_t * const byte_size);

#else // CONF_CANFIGURATOR_MODULE_ENABLE

#define canfigurator_get_max_volt() (0.0f)
#define canfigurator_get_min_volt() (0.0f)
#define canfigurator_get_max_temp() (0.0f)
#define canfigurator_get_min_temp() (0.0f)
#define canfigurator_canfig_handle(payload) (NULL)

#endif // CONF_CANFIGURATOR_MODULE_ENABLE

#endif // CANFIGURATOR_H

