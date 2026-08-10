/*!
 * \file pcu-api.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Control Unit (PCU) used to manage the main logic of the pack
 */

#ifndef PCU_API_H
#define PCU_API_H

#include "pcu.h"
#include "mainboard-conf.h"

#ifdef CONF_PCU_MODULE_ENABLE

/*!
 * \brief Initialize the internal PCU handler
 *
 * \param set The callback used to set the status of the PCU pins
 * \param toggle The callback used to toggle the status of the PCU pins
 *
 * \retval PCU_NULL_POINTER if the set or toggle callbacks are NULL
 * \retval PCU_OK otherwise
 */
enum PcuReturnCode pcu_api_init(pcu_set_state_callback set, pcu_toggle_state_callback toggle);

/*! \brief Reset all the pins to their initial states */
void pcu_api_reset_all(void);

/*! \brief Open the AIR- */
void pcu_api_airn_open(void);

/*! \brief Close the AIR- */
void pcu_api_airn_close(void);

/*! \brief Stop the internal watchdog of the AIR- */
void pcu_api_airn_stop_watchdog(void);

/*! \brief Open the AIR+ */
void pcu_api_airp_open(void);

/*! \brief Close the AIR+ */
void pcu_api_airp_close(void);

/*! \brief Stop the internal watchdog of the AIR+ */
void pcu_api_airp_stop_watchdog(void);

/*! \brief Start precharge procedure */
void pcu_api_precharge_start(void);

/*! \brief Stop precharge procedure */
void pcu_api_precharge_stop(void);

/*! \brief Stop the internal watchdog of the precharge */
void pcu_api_precharge_stop_watchdog(void);

/*! \brief Activate the AMS */
void pcu_api_ams_activate(void);

/*! \brief Deactivate the AMS */
void pcu_api_ams_deactivate(void);

/*!
 * \brief Get the current percentage of the precharge from 0 to 1
 *
 * \returns precise_percentage_t The precharge percentage
 */
precise_percentage_t pcu_api_get_precharge_percentage(void);

/*!
 * \brief Check if the precharge has ended
 *
 * \returns bool True if the precharge is completed, false otherwise
 */
bool pcu_api_is_precharge_complete(void);

/*!
 * \brief Handle the received set status message
 *
 * \param[in] tson True if TSON is requested, false if TSOFF is requested
 */
void pcu_api_bms_set_handle(bool tson);

#else // CONF_PCU_MODULE_ENABLE

#define pcu_api_init(set, toggle) (PCU_RC_OK)
#define pcu_api_reset_all() EAGLETRT_API_NOP()
#define pcu_api_airn_open() EAGLETRT_API_NOP()
#define pcu_api_airn_close() EAGLETRT_API_NOP()
#define pcu_api_airn_stop_watchdog() EAGLETRT_API_NOP()
#define pcu_api_airp_open() EAGLETRT_API_NOP()
#define pcu_api_airp_close() EAGLETRT_API_NOP()
#define pcu_api_airp_stop_watchdog() EAGLETRT_API_NOP()
#define pcu_api_precharge_start() EAGLETRT_API_NOP()
#define pcu_api_precharge_stop() EAGLETRT_API_NOP()
#define pcu_api_precharge_stop_watchdog() EAGLETRT_API_NOP()
#define pcu_api_ams_activate() EAGLETRT_API_NOP()
#define pcu_api_ams_deactivate() EAGLETRT_API_NOP()
#define pcu_api_get_precharge_percentage() (0.f)
#define pcu_api_is_precharge_complete() (false)
#define pcu_api_bms_set_handle(tson) EAGLETRT_API_NOP()

#endif // CONF_PCU_MODULE_ENABLE

#endif // PCU_API_H
