/**
 * @file current.h
 * @date 2024-06-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Current measurement and control
 */

#ifndef CURRENT_H
#define CURRENT_H

#include "mainboard-conf.h"
#include "mainboard-def.h"

#include "bms_network.h"
#include "primary_network.h"

/** @brief Minimum and maximum allowed current value in A */
#define CURRENT_MIN_A (-22.f)
#define CURRENT_MAX_A (130.f)

/**
 * @brief Return code for the current module functions
 *
 * @details
 *     - CURRENT_OK the function executed succefully
 */
typedef enum {
    CURRENT_OK
} CurrentReturnCode;

/**
 * @brief Current handler structure
 *
 * @attention This structure should not be used outside of this module
 *
 * @param current The current value in A
 * @param current_can_payload The canlib payload used to send the current value via CAN
 * @param power_can_payload The canlib payload used to send the power value via CAN
 */
typedef struct {
    ampere_t current;    

    primary_hv_current_converted_t current_can_payload;
    primary_hv_power_converted_t power_can_payload;
} _CurrentHandler;

#ifdef CONF_CURRENT_MODULE_ENABLE

/**
 * @brief Initialize the internal current structure handler
 *
 * @return CurrentReturnCode
 *     - CURRENT_OK
 */
CurrentReturnCode current_init(void);

/**
 * @brief Handle the received response from the current sensor
 *
 * @param payload A pointer to the canlib payload of the response
 */
void current_handle(bms_ivt_msg_result_i_t * const payload);

/**
 * @brief Get the supplied current in A
 *
 * @return ampere_t The current in A
 */
ampere_t current_get_current(void);

/**
 * @brief Get the calculated power value in kW
 *
 * @return kilowatt_t The power in kW
 */
kilowatt_t current_get_power(void);

/**
 * @brief Get a pointer to the CAN payload of the current
 * 
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_current_converted_t* A pointer to the payload
 */
primary_hv_current_converted_t * current_get_current_canlib_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload of the power
 * 
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_power_converted_t* A pointer to the payload
 */
primary_hv_power_converted_t * current_get_power_canlib_payload(size_t * const byte_size);

#else  // CONF_CURRENT_MODULE_ENABLE

#define current_init() (CURRENT_OK)
#define current_handle(payload) CELLBOARD_NOPE()
#define current_get_current() (0.f)
#define current_get_power() (0.f)
#define current_get_current_canlib_payload(byte_size) (NULL)
#define current_get_power_canlib_payload(byte_size) (NULL)

#endif // CONF_CURRENT_MODULE_ENABLE

#endif  // CURRENT_H
