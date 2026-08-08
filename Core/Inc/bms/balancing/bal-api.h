/*!
 * \file bal-api.h
 * \date 2024-08-02
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Balancing handling functions
 */

#ifndef BAL_API_H
#define BAL_API_H

#include "bal.h"
#include "can-bms.h"
#include "mainboard-conf.h"

#ifdef CONF_BALANCING_MODULE_ENABLE

/*!
 * \brief Intiialize the internal balancing handler structure
 *
 * \retval BAL_OK
 */
enum BalReturnCode bal_api_init(void);

/*!
 * \brief Check if the balancing is active
 *
 * \returns bool True if the balancing procedure is running, false otherwise
 */
bool bal_api_is_active(void);

/*!
 * \brief Start the balancing mechanism
 *
 * \retval BAL_OK
 * \retval BAL_WATCHDOG_ERROR the interal watchdog could not be started
 */
enum BalReturnCode bal_api_start(void);

/*!
 * \brief Stop the balancing mechanism
 *
 * \retval BAL_OK
 */
enum BalReturnCode bal_api_stop(void);

/*!
 * \brief Get a pointer to the set balancing status message canlib payload
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer of the payload or NULL if the id is not valid
 */
union CanBmsMessages *bal_api_get_balancing_set_canlib_payload(size_t *byte_size);

/*!
 * \brief Handle the received set balancing status message sent from the steering wheel
 *
 * \param[in] balancing True if balancing start is requested, false if balancing stop is requested
 * \param[in] threshold Voltage delta to reach to complete the balancing
 */
void bal_api_set_balancing_state_handle(bool balancing, volt_t threshold);

/*!
 * \brief Handle the received balancing status message sent from the cellboards
 *
 * \param payload A pointer to the canlib paylod
 */
// void bal_api_cellboard_balancing_status_handle(bms_cellboard_balancing_status_converted_t *payload);

/*!
 * \brief Get a pointer to the balancing status message canlib payload
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_balancing_status_converted_t* A pointer to the payload or NULL if the id is not valid
 */
// primary_hv_balancing_status_converted_t *bal_api_get_status_canlib_payload(size_t *byte_size);

#else // CONF_BALANCING_MODULE_ENABLE

#define bal_api_init() (BAL_OK)
#define bal_api_is_active() (false)
#define bal_api_start() (BAL_OK)
#define bal_api_stop() (BAL_OK)
#define bal_api_set_balancing_state_from_steering_wheel_handle(payload) EAGLETRT_API_NOP()
#define bal_api_set_balancing_state_from_handcart_handle(payload) EAGLETRT_API_NOP()
#define bal_api_get_set_status_canlib_payload(byte_size) (NULL)
#define bal_api_get_status_canlib_payload(byte_size) (NULL)

#endif // CONF_BALANCING_MODULE_ENABLE

#endif // BAL_API_H
