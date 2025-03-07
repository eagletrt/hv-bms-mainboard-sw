/**
 * @file pcu.h
 * @date 2024-07-09
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Pack Control Unit (PCU) used to manage the main logic of the pack
 */

#ifndef PCU_H
#define PCU_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "fsm.h"
#include "watchdog.h"

/** @brief PCU pins timeouts in ms */
#define PCU_AIRN_TIMEOUT_MS (1000U)
#define PCU_PRECHARGE_TIMEOUT_MS (15000U)
#define PCU_AIRP_TIMEOUT_MS (1000U)

/** @brief Precharge threshold percentage */
#define PCU_PRECHARGE_THRESHOLD_PERCENT (0.95f)

/**
 * @brief Return code for the PCU module functions
 *
 * @details
 *     - PCU_OK the function executed succesfully
 *     - PCU_NULL_POINTER a NULL pointer was given to a function
 */
typedef enum {
    PCU_OK,
    PCU_NULL_POINTER
} PcuReturnCode;

/**
 * @brief Status of a PIN of the PCU
 *
 * @details
 *     - PCU_PIN_STATUS_LOW the pin voltage is set low
 *     - PCU_PIN_STATUS_HIGH the pin voltage is set high
 */
typedef enum {
    PCU_PIN_STATUS_LOW = 0,
    PCU_PIN_STATUS_HIGH
} PcuPinStatus;

/**
 * @brief Pins used to control the main logic of the BMS
 *
 * @attention The AIR is open if the signal is high, closed if low
 * @attention The AMS is active if the signal is low, inactive if low (inactive means OK)
 *
 * @details
 *     - PCU_PIN_AIR_NEGATIVE pin connected to the AIR-
 *     - PCU_PIN_AIR_POSITIVE pin connected to the AIR+
 *     - PCU_PIN_PRECHARGE pin connected to the PRECHARGE circuit
 *     - PCU_PIN_AMS pin connected to the AMS circuit
 */
typedef enum {
    PCU_PIN_AIR_NEGATIVE,
    PCU_PIN_AIR_POSITIVE,
    PCU_PIN_PRECHARGE,
    PCU_PIN_AMS,
    PCU_PIN_COUNT
} PcuPin;

/**
 * @brief Callback used to set the state of the PCU pin
 *
 * @param pcu_pin The pin to select
 * @param state The new state to set
 */
typedef void (* pcu_set_state_callback_t)(const PcuPin pcu_pin, const PcuPinStatus state);

/**
 * @brief Callback used to toggle the state of the PCU pin
 *
 * @param pcu_pin The pin to select
 */
typedef void (* pcu_toggle_state_callback_t)(const PcuPin pcu_pin);

/**
 * @brief PCU handler structure
 *
 * @warning This structure should never be used outside of this file
 *
 * @param set A pointer to the function callback used to set a PCU pin
 * @param toggle A pointer to the function callback used to toggle a PCU pin
 * @param precharge_watchdog Precharge watchdog
 */
typedef struct {
    pcu_set_state_callback_t set;
    pcu_toggle_state_callback_t toggle;

    fsm_event_data_t event;
    fsm_event_data_t timeout_event;

    Watchdog airn_watchdog;
    Watchdog precharge_watchdog;
    Watchdog airp_watchdog;
} _PcuHandler;

#ifdef CONF_PCU_MODULE_ENABLE

/**
 * @brief Initialize the internal PCU handler
 *
 * @param set The callback used to set the status of the PCU pins
 * @param toggle The callback used to toggle the status of the PCU pins
 *
 * @return PcuReturnCode
 *     - PCU_NULL_POINTER if the set or toggle callbacks are NULL
 *     - PCU_OK otherwise
 */
PcuReturnCode pcu_init(const pcu_set_state_callback_t set, const pcu_toggle_state_callback_t toggle);

/** @brief Reset all the pins to their initial states */
void pcu_reset_all(void);

/** @brief Open or close the AIR- */
void pcu_airn_open(void);
void pcu_airn_close(void);
/** @brief Stop the internal watchdog of the AIR- */
void pcu_airn_stop_watchdog(void);

/** @brief Open or close the AIR+ */
void pcu_airp_open(void);
void pcu_airp_close(void);
/** @brief Stop the internal watchdog of the AIR+ */
void pcu_airp_stop_watchdog(void);

/** @brief Start or stop the precharge procedure */
void pcu_precharge_start(void);
void pcu_precharge_stop(void);
/** @brief Stop the internal watchdog of the precharge */
void pcu_precharge_stop_watchdog(void);

/** @brief Activate or deactivate the AMS */
void pcu_ams_activate(void);
void pcu_ams_deactivate(void);

/**
 * @brief Get the current percentage of the precharge from 0 to 1
 *
 * @return precise_percentage_t The precharge percentage
 */
precise_percentage_t pcu_get_precharge_percentage(void);

/**
 * @brief Check if the precharge has ended
 *
 * @return bool True if the precharge is completed, false otherwise
 */
bool pcu_is_precharge_complete(void);

/**
 * @brief Handle the received set status message sent from the ECU
 *
 * @param payload A pointer to the canlib payload of the response
 */
void pcu_set_state_from_ecu_handle(primary_hv_set_status_ecu_converted_t * const payload);

/**
 * @brief Handle the received set status message sent from the handcart
 *
 * @param payload A pointer to the canlib payload of the response
 */
void pcu_set_state_from_handcart_handle(primary_hv_set_status_handcart_converted_t * const payload);

#else  // CONF_PCU_MODULE_ENABLE

#define pcu_init(set, toggle) (PCU_OK)
#define pcu_reset_all() MAINBOARD_NOPE()
#define pcu_airn_open() MAINBOARD_NOPE()
#define pcu_airn_close() MAINBOARD_NOPE()
#define pcu_airn_stop_watchdog() MAINBOARD_NOPE()
#define pcu_airp_open() MAINBOARD_NOPE()
#define pcu_airp_close() MAINBOARD_NOPE()
#define pcu_airp_stop_watchdog() MAINBOARD_NOPE()
#define pcu_precharge_start() MAINBOARD_NOPE()
#define pcu_precharge_stop() MAINBOARD_NOPE()
#define pcu_precharge_stop_watchdog() MAINBOARD_NOPE()
#define pcu_ams_activate() MAINBOARD_NOPE()
#define pcu_ams_deactivate() MAINBOARD_NOPE()
#define pcu_get_precharge_percentage() (0.f)
#define pcu_is_precharge_complete() (false)
#define pcu_set_state_from_ecu_handle(payload) MAINBOARD_NOPE()
#define pcu_set_state_from_handcart_handle(payload) MAINBOARD_NOPE()

#endif // CONF_PCU_MODULE_ENABLE

#endif  // PCU_H
