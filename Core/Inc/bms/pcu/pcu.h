/*!
 * \file pcu.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Control Unit (PCU) used to manage the main logic of the pack
 */

#ifndef PCU_H
#define PCU_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "fsm.h"
#include "watchdog.h"
#include "eagletrt-api.h"

/*! \brief PCU pins timeouts in ms */
#define PCU_AIRN_TIMEOUT_MS (1000U)
#define PCU_PRECHARGE_TIMEOUT_MS (15000U)
#define PCU_AIRP_TIMEOUT_MS (1000U)

/*! \brief Precharge threshold percentage */
#define PCU_PRECHARGE_THRESHOLD_PERCENT (0.95f)

/*!
 * \brief Return code for the PCU module functions
 */
enum PcuReturnCode {
    PCU_RC_OK,          /*!< Function executed successfully */
    PCU_RC_NULL_POINTER /*!< A NULL pointer was given to a function */
};

/*!
 * \brief Status of a PIN of the PCU
 */
enum PcuPinStatus {
    PCU_PIN_STATUS_LOW = 0, /*!< The pin voltage is set low */
    PCU_PIN_STATUS_HIGH     /*!< The pin voltage is set high */
};

/*!
 * \brief Pins used to control the main logic of the BMS
 *
 * \attention The AIR is open if the signal is high, closed if low
 * \attention The AMS is active if the signal is low, inactive if low (inactive means OK)
 */
enum PcuPin {
    PCU_PIN_AIR_NEGATIVE, /*!< The AIR- pin */
    PCU_PIN_AIR_POSITIVE, /*!< The AIR+ pin */
    PCU_PIN_PRECHARGE,    /*!< The precharge pin */
    PCU_PIN_AMS,          /*!< The AMS pin */
    PCU_PIN_COUNT         /*!< The number of PCU pins */
};

/*!
 * \brief Callback used to set the state of the PCU pin
 *
 * \param pcu_pin The pin to select
 * \param state The new state to set
 */
typedef void (*pcu_set_state_callback)(const enum PcuPin pcu_pin, const enum PcuPinStatus state);

/*!
 * \brief Callback used to toggle the state of the PCU pin
 *
 * \param pcu_pin The pin to select
 */
typedef void (*pcu_toggle_state_callback)(const enum PcuPin pcu_pin);

/*!
 * \brief PCU handler structure
 */
struct PcuHandler {
    pcu_set_state_callback set;       /*!< A pointer to the function callback used to set a PCU pin */
    pcu_toggle_state_callback toggle; /*!< A pointer to the function callback used to toggle a PCU pin */

    fsm_event_data_t event;         /*!< The event data used to trigger the state machine of the PCU */
    fsm_event_data_t timeout_event; /*!< The event data used to trigger the timeout of the PCU */

    struct Watchdog airn_watchdog;      /*!< The watchdog for the AIR- pin */
    struct Watchdog precharge_watchdog; /*!< The watchdog for the precharge pin */
    struct Watchdog airp_watchdog;      /*!< The watchdog for the AIR+ pin */
};

#endif // PCU_H
