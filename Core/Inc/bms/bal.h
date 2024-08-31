/**
 * @file bal.h
 * @date 2024-08-02
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Balancing handling functions
 */

#ifndef BAL_H
#define BAL_H

#include <stdbool.h>

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "watchdog.h"
#include "fsm.h"

#include "primary_network.h"
#include "bms_network.h"

/** @brief Balancing threshold range in V */
#define BAL_THRESHOLD_MIN_V (0.005f)
#define BAL_THRESHOLD_MAX_V (0.200f)

/** @brief Balancing target range in V */
#define BAL_TARGET_MIN_V (2.8f)
#define BAL_TARGET_MAX_V (4.2f)

/** @brief Balancing timeouts in ms */
#define BAL_TIMEOUT_MS (3000U)

/**
 * @brief Return code for the balancing module functions
 *
 * @details
 *     - BAL_OK the function executed succesfully
 *     - BAL_NULL_POINTER a NULL pointer was given to a function
 *     - BAL_WATCHDOG_ERROR there was an error during a balancing watchdog operation
 */
typedef enum {
    BAL_OK,
    BAL_NULL_POINTER,
    BAL_WATCHDOG_ERROR
} BalReturnCode;

/**
 * @brief Definition of the balancing parameters
 *
 * @param target The minimum allowed voltage that should be reached while discharging in V
 * @param threshold The minimum allowed difference between the minimum and maximum cell voltages in V
 */
typedef struct {
    volt_t target;
    volt_t threshold;
} BalParams;

/**
 * @brief Type definition for the balancing handler structure
 *
 * @attention This structure should not be used outside of this module
 *
 * @param event The FSM event data
 * @param set_status_can_payload The set balancing status message canlib payload
 * @param status_can_payload The balancing status message canlib payload
 * @param watchdog The watchdog that stops the balancing procedure when timed out
 * @param active True if the balancing is active, false otherwise
 * @param params The balancing parameters
 */
typedef struct {
    fsm_event_data_t event;
    bms_cellboard_set_balancing_status_converted_t set_status_can_payload;
    primary_hv_balancing_status_converted_t status_can_payload;
    Watchdog watchdog;

    bool active;
    BalParams params;
} _BalHandler;

#ifdef CONF_BALANCING_MODULE_ENABLE

/**
 * @brief Intiialize the internal balancing handler structure
 *
 * @return BalReturnCode
 *     - BAL_OK
 */
BalReturnCode bal_init(void);

/**
 * @brief Check if the balancing is active
 *
 * @return bool True if the balancing procedure is running, false otherwise
 */
bool bal_is_active(void);

/**
 * @brief Start the balancing mechanism
 *
 * @return BalReturnCode
 *     - BAL_WATCHDOG_ERROR the interal watchdog could not be started
 *     - BAL_OK otherwise
 */
BalReturnCode bal_start(void);

/**
 * @brief Stop the balancing mechanism
 *
 * @return BalReturnCode
 *     - BAL_OK
 */
BalReturnCode bal_stop(void);

/**
 * @brief Handle the received set balancing status message sent from the steering wheel
 *
 * @param payload A pointer to the canlib paylod
 */
void bal_set_balancing_state_from_steering_wheel_handle(primary_hv_set_balancing_status_steering_wheel_converted_t * const payload);

/**
 * @brief Handle the received set balancing status message sent from the steering wheel
 *
 * @param payload A pointer to the canlib paylod
 */
void bal_set_balancing_state_from_handcart_handle(primary_hv_set_balancing_status_handcart_converted_t * const payload);

/**
 * @brief Handle the received balancing status message sent from the cellboards
 *
 * @param payload A pointer to the canlib paylod
 */
void bal_cellboard_balancing_status_handle(bms_cellboard_balancing_status_converted_t * const payload);

/**
 * @brief Get a pointer to the set balancing status message canlib payload
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return bms_cellboard_set_balancing_status_converted_t* A pointer to the payload or NULL if the id is not valid
 */
bms_cellboard_set_balancing_status_converted_t * bal_get_set_status_canlib_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the balancing status message canlib payload
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_balancing_status_converted_t* A pointer to the payload or NULL if the id is not valid
 */
primary_hv_balancing_status_converted_t * bal_get_status_canlib_payload(size_t * const byte_size);

#else  // CONF_BALANCING_MODULE_ENABLE

#define bal_init() (BAL_OK)
#define bal_is_active() (false)
#define bal_start() (BAL_OK)
#define bal_stop() (BAL_OK)
#define bal_set_balancing_state_from_steering_wheel_handle(payload) MAINBOARD_NOPE()
#define bal_set_balancing_state_from_handcart_handle(payload) MAINBOARD_NOPE()
#define bal_get_set_status_canlib_payload(byte_size) (NULL)
#define bal_get_status_canlib_payload(byte_size) (NULL)

#endif // CONF_BALANCING_MODULE_ENABLE

#endif  // BAL_H
