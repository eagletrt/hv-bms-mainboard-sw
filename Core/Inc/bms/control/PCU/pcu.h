/*!
 * \file pcu.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the Pack Control Unit (PCU)
 */

#ifndef PCU_H
#define PCU_H

#include <stdbool.h>
#include <stdint.h>

#include "mainboard-def.h"
#include "mainboard-status.h"

/*!
 * \brief Return code for the PCU module functions
 */
enum PcuReturnCode : uint8_t {
    PCU_RC_OK,            /*!< Function executed successfully */
    PCU_RC_NULL_POINTER,  /*!< A NULL pointer was given to a function */
    PCU_RC_INVALID_RELAY, /*!< The given relay does not exist */
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
 * \attention The relays are open if the signal is high, closed if low
 * \attention The AMS is active if the signal is low, inactive if high (inactive means OK)
 */
enum PcuPin {
    PCU_PIN_AIR_NEGATIVE, /*!< The AIR- pin */
    PCU_PIN_AIR_POSITIVE, /*!< The AIR+ pin */
    PCU_PIN_PRECHARGE,    /*!< The precharge pin */
    PCU_PIN_AMS,          /*!< The AMS pin */
    PCU_PIN_COUNT         /*!< The number of PCU pins */
};

/*!
 * \brief Relays controlled by the PCU, in the order they are closed to turn the TS on
 */
enum PcuRelay : uint8_t {
    PCU_RELAY_AIR_NEGATIVE, /*!< The AIR- */
    PCU_RELAY_PRECHARGE,    /*!< The precharge relay */
    PCU_RELAY_AIR_POSITIVE, /*!< The AIR+ */
    PCU_RELAY_COUNT         /*!< The number of relays */
};

/*!
 * \brief Callback used to set the state of the PCU pin
 *
 * \param pcu_pin The pin to select
 * \param state The new state to set
 */
typedef void (*pcu_set_state_callback)(const enum PcuPin pcu_pin, const enum PcuPinStatus state);

/*!
 * \brief Fixed description of a relay
 */
struct PcuRelayInfo {
    enum PcuPin pin;                         /*!< The pin that drives the relay */
    enum MainboardStatusCondition commanded; /*!< The condition that mirrors the close command, owned by the PCU */
};

/*!
 * \brief PCU handler structure
 */
struct PcuHandler {
    pcu_set_state_callback set;  /*!< The callback used to set a PCU pin */
    system_reset_callback reset; /*!< The callback used to reset the microcontroller */

    bool closed[PCU_RELAY_COUNT]; /*!< Whether each relay is commanded closed */
};

#endif // PCU_H
