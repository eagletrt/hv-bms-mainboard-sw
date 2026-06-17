/*!
 * \file max22530-api.h
 * \date 2024-07-01
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * 
 * \brief MAX22530AWE+ external ADC driver
 */

#ifndef MAX22530_API_H
#define MAX22530_API_H

#include "max22530.h"

/*!
 * \brief Initialize the MAX22530 internal handler structure
 *
 * \attention By default CRC is disabled so every command is 24 bit long instead of 36 bit,
 * but it can be enabled through spi in the control register of the MAX22530
 *
 * \param handler The ADC handler structure
 * \param send A pointer to the callback used to send data via SPI
 * \param send_receive A pointer to the callback used to send and receive data via SPI
 *
 * \retval MAX22530_RC_NULL_POINTER if any of the parameter is NULL
 * \retval MAX22530_RC_OK otherwise
 */
enum Max22530ReturnCode max22530_api_init(struct Max22530Handler *handler, spi_send_callback_t send, spi_send_receive_callback_t send_receive);

/*!
 * \brief Get the ADC identifier code
 *
 * \param handler A pointer to the ADC handler structure
 *
 * \returns max22530_id The ADC identifier or -1 on error
 */
max22530_id max22530_api_get_id(struct Max22530Handler *handler);

/*!
 * \brief Get the power-on reset code
 *
 * \param handler A pointer to the ADC handler structure
 *
 * \returns enum Max22530PowerOnReset The power-on reset code or -1 on error
 */
enum Max22530PowerOnReset max22530_api_get_power_on_reset(struct Max22530Handler *handler);

/*!
 * \brief Get the revision code
 *
 * \param handler A pointer to the ADC handler structure
 *
 * \returns max22530_revision The revision code or -1 on error
 */
max22530_revision max22530_api_get_revision(struct Max22530Handler *handler);

/*!
 * \brief Read a single channel of the ADC
 *
 * \param handler A pointer to the ADC handler structure
 * \param channel The channel to read from
 * \param filtered Set to true to select the filtered channel
 *
 * \returns volt_t The voltage read from the channel in V
 */
volt_t max22530_api_read_channel(struct Max22530Handler *handler, enum Max22530Channel channel, bool filtered);

/*!
 * \brief Read all the channels of the ADC in a single command
 *
 * \attention The out array has to be big enough to contain all the ADC channels values
 * To declare the array use the MAX22530_CHANNEL_COUNT enum field
 *
 * \param handler A pointer to the ADC handler structure
 * \param filtered Set to true to select the filtered channel
 * \param out[out] A pointer to the array where the data is stored
 * \param interrupt_status[out] A pointer to the variable where the interrupt status is stored (can be NULL)
 *
 * \retval MAX22530_RC_NULL_POINTER if any of the parameters are NULL
 * \retval MAX22530_RC_OK otherwise
 */
enum Max22530ReturnCode max22530_api_read_channels_all(struct Max22530Handler *handler, bool filtered, volt_t *out, uint16_t *interrupt_status);

#endif // MAX22530_API_H
