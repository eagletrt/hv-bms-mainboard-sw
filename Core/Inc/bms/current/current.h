/*!
 * \file current.h
 * \date 2024-06-08
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Current measurement and control
 */

#ifndef CURRENT_H
#define CURRENT_H

#include "mainboard-conf.h"
#include "mainboard-def.h"

#include "bms_network.h"
#include "primary_network.h"
#include "eagletrt-api.h"

#include "watchdog.h"

/*! \brief Minimum and maximum allowed current value in A */
#define CURRENT_MIN_A (-22.f)
#define CURRENT_MAX_A (130.f)

/*!
 * \brief Maximum allowed power in kW
 *
 * \details FS-Rules 2024 v1.1
 *     Rule EV 2.2.1    The TS power at the outlet of the TSAC must not exceed 80 kW.
 */
#define CURRENT_MAX_POWER_KW (80.f)

/*!
 * \brief Maximum allowed time delta between two messages of the current sensor
 *
 * \details FS-Rules 2024 v1.1
 *     Rule EV 5.8.7    The AMS must switch off the TS via the SDC, if a critical
 *                      voltage, temperature, or current value according to the
 *                      cell manufacturer’s datasheet or these rules persistently
 *                      occurs for more than:
 *                          - 500 ms for voltage and current values
 *                          - 1 s for temperature values
 */
#define CURRENT_SENSOR_COMMUNICATION_TIMEOUT_MS (200U)

/*!
 * \brief Startup time of the current sensor in ms
 */
#define CURRENT_SENSOR_STARTUP_TIME_MS (400U)

/*!
 * \brief Return code for the current module functions
 */
enum CurrentReturnCode {
    CURRENT_RC_OK /*!< Function executed successfully */
};

/*!
 * \brief Current handler structure
 *
 * \attention This structure should not be used outside of this module
 */
struct CurrentHandler {
    ampere_t current; /*!< The current value in A */

    Watchdog sensor_wdg; /*!< Watchdog used to check if the current sensor is connected */

    primary_hv_current_converted_t current_can_payload; /*!< The canlib payload used to send the current value via CAN */
    primary_hv_power_converted_t power_can_payload;     /*!< The canlib payload used to send the power value via CAN */
};

#endif // CURRENT_H
