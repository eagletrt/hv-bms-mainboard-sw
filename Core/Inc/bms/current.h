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

/** @brief Minimum and maximum allowed current value in mA */
#define CURRENT_MIN_MILLIAMPERE ((milliampere_t)-22000.f)
#define CURRENT_MAX_MILLIAMPERE ((milliampere_t)130000.f)

/** @brief Minimum and maximum allowed current raw values */
#define CURRENT_MIN_VALUE (CURRENT_MILLIAMPERE_TO_VALUE(CURRENT_MIN_MILLIAMPERE))
#define CURRENT_MAX_VALUE (CURRENT_MILLIAMPERE_TO_VALUE(CURRENT_MAX_MILLIAMPERE))

/**
 * @brief Convert a raw current value to milliampere
 *
 * @param value The value to convert
 *
 * @return milliampere_t the converted current in milliampere
 */
#define CURRENT_VALUE_TO_MILLIAMPERE(value) ((milliampere_t)(value))
/**
 * @brief Convert a current in milliampere to the raw current value
 *
 * @param value The value in milliampere to convert
 *
 * @return raw_current_t The raw current value
 */
#define CURRENT_MILLIAMPERE_TO_VALUE(value) ((raw_current_t)(value))

/**
 * @brief Return code for the current module functions
 *
 * @details
 *     - CURRENT_OK the function executed succefully
 */
typedef enum {
    CURRENT_OK
} CurrentReturnCode;

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
void current_handle(bms_ivt_msg_result_i_t * payload);

/**
 * @brief Get a pointer to the CAN payload of the current
 * 
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_current_converted_t* A pointer to the payload
 */
primary_hv_current_converted_t * current_get_canlib_payload(size_t * byte_size);

/**
 * @brief Get the saved raw current value
 *
 * @return raw_current_t The current
 */
raw_current_t current_get_current(void);

#else  // CONF_CURRENT_MODULE_ENABLE

#define current_init() (CURRENT_OK)
#define current_handle(payload) CELLBOARD_NOPE()
#define current_get_current() (0U)

#endif // CONF_CURRENT_MODULE_ENABLE

#endif  // CURRENT_H
