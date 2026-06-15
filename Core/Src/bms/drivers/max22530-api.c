/*!
 * \file max22530-api.c
 * \date 2024-07-01
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief MAX22530AWE+ external ADC driver
 */

#include "max22530-api.h"

#include <stdint.h>
#include <string.h>
#include "eagletrt-api.h"

constexpr uint8_t low_half_mask = 0xff;
constexpr uint16_t high_half_mask = 0xff00;
constexpr uint8_t second_half_shift = 8U;

/*!
 * \brief Write to the ADC registers
 *
 * \param handler A pointer to the handler structure
 * \param address The address of the register to write to
 * \param data The data to write
 *
 * \returns MAX22530_RC_OK
 */
enum Max22530ReturnCode prv_max22530_api_write(struct Max22530Handler *const handler, const max22530_address address, const uint16_t data) {

    uint8_t cmd[MAX22530_COMMAND_BYTE_SIZE] = {
        (address << 2U) | (MAX22530_COMMAND_WRITE << 1U),
        (data & high_half_mask) >> second_half_shift,
        data & low_half_mask
    };
    handler->send(SPI_NETWORK_ADC, cmd, MAX22530_COMMAND_BYTE_SIZE);
    return MAX22530_RC_OK;
}

/*!
 * \brief Read from an ADC registers
 *
 * \param handler A pointer to the handler structure
 * \param address The address of the register to read from
 *
 * \return uint16_t The data of the register
 */
uint16_t prv_max22530_api_read(struct Max22530Handler *const handler, const max22530_address address) {
    uint8_t cmd[MAX22530_COMMAND_BYTE_SIZE];
    cmd[0U] = (address << 2U) |
              (MAX22530_COMMAND_READ << 1U) |
              (MAX22530_BURST_OFF);

    // Fill out every other byte with 1s so that they are ignored
    for (size_t i = 1U; i < MAX22530_COMMAND_BYTE_SIZE; ++i) {
        cmd[i] = MAX22530_BYTE_UNUSED;
    }
    handler->send_receive(SPI_NETWORK_ADC, cmd, &cmd[1U], 1U, MAX22530_COMMAND_BYTE_SIZE - 1U);
    return ((uint16_t)cmd[1U] << second_half_shift) | cmd[2U];
}

/*!
 * \brief Read all the channels data from the ADC
 *
 * \param handler A pointer to the handler structure
 * \param filtered If true reads the filtered values
 * \param out[out] A pointer to the array where the data is stored
 *
 * \return enum Max22530ReturnCode
 *     - MAX22530_RC_OK
 */
enum Max22530ReturnCode prv_max22530_api_burst(
    struct Max22530Handler *const handler,
    const bool filtered,
    uint16_t *const out) {
    const max22530_address address = (int)filtered ? MAX22530_REGISTER_FILTERED_ADC : MAX22530_REGISTER_ADC;

    uint8_t cmd[MAX22530_BURST_BYTE_SIZE];
    cmd[0U] = (address << 2U) | (MAX22530_COMMAND_READ << 1U) | (MAX22530_BURST_ON);

    // Fill out every other byte with 1s so that they are ignored
    for (size_t i = 1U; i < MAX22530_BURST_BYTE_SIZE; ++i) {
        cmd[i] = MAX22530_BYTE_UNUSED;
    }

    handler->send_receive(SPI_NETWORK_ADC, cmd, &cmd[1U], 1U, MAX22530_BURST_BYTE_SIZE - 1U);

    constexpr uint8_t byte_mask = 0x0f;

    for (size_t i = 0U; i < MAX22530_CHANNEL_COUNT + 1U; ++i) {
        out[i] = ((cmd[(i * 2U) + 1U] & byte_mask) << second_half_shift) | cmd[(i * 2U) + 2U];
    }
    return MAX22530_RC_OK;
}

enum Max22530ReturnCode max22530_api_init(struct Max22530Handler *const handler, const spi_send_callback_t send, const spi_send_receive_callback_t send_receive) {
    if (handler == NULL ||
        send == NULL ||
        send_receive == NULL) {
        return MAX22530_RC_NULL_POINTER;
    }
    handler->send = send;
    handler->send_receive = send_receive;

    uint16_t data = 0;
    data = EAGLETRT_API_BIT_SET(data, MAX22530_CONTROL_FIELD_SW_RESET);
    data = EAGLETRT_API_BIT_SET(data, MAX22530_CONTROL_FIELD_CLEAR_POR);
    return prv_max22530_api_write(handler, MAX22530_REGISTER_CONTROL, data);
}

max22530_id max22530_api_get_id(struct Max22530Handler *const handler) {

    constexpr uint16_t id_shift = 8U;

    if (handler == NULL) {
        return -1;
    }
    const uint16_t data = prv_max22530_api_read(handler, MAX22530_REGISTER_ID);
    return (int8_t)(data >> id_shift);
}

enum Max22530PowerOnReset max22530_api_get_power_on_reset(struct Max22530Handler *const handler) {

    constexpr uint16_t por_shift = 7U;
    constexpr uint16_t por_mask = 0x80;

    if (handler == NULL) {
        return -1;
    }
    const uint16_t data = prv_max22530_api_read(handler, MAX22530_REGISTER_ID);
    return (data & por_mask) >> por_shift;
}

max22530_revision max22530_api_get_revision(struct Max22530Handler *const handler) {

    constexpr uint16_t revision_mask = 0x7f;

    if (handler == NULL) {
        return -1;
    }
    const uint16_t data = prv_max22530_api_read(handler, MAX22530_REGISTER_ID);
    return (int8_t)(data & revision_mask);
}

volt_t max22530_api_read_channel(struct Max22530Handler *const handler, const enum Max22530Channel channel, const bool filtered) {
    if (handler == NULL) {
        return 0U;
    }
    // Get the channel register address
    max22530_address address = (int)filtered ? MAX22530_REGISTER_FILTERED_ADC : MAX22530_REGISTER_ADC;
    address += (uint8_t)channel;

    // Get data
    const raw_volt_t data = prv_max22530_api_read(handler, address);
    return MAX22530_RAW_VALUE_TO_VOLT(data); //NOLINT
}

enum Max22530ReturnCode max22530_api_read_channels_all(struct Max22530Handler *const handler, const bool filtered, volt_t *const out, uint16_t *const interrupt_status) {
    if (handler == NULL || out == NULL) {
        return MAX22530_RC_NULL_POINTER;
    }
    // Get data
    raw_volt_t data[MAX22530_CHANNEL_COUNT + 1U] = { 0 };
    const enum Max22530ReturnCode code = prv_max22530_api_burst(handler, filtered, data);

    // Copy data
    for (size_t i = 0U; i < MAX22530_CHANNEL_COUNT; ++i) {
        out[i] = MAX22530_RAW_VALUE_TO_VOLT(data[i]); //NOLINT
    }
    if (interrupt_status != NULL) {
        *interrupt_status = data[MAX22530_CHANNEL_COUNT];
    }
    return code;
}
