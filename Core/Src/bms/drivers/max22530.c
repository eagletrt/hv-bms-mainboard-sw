/**
 * @file max22530.h
 * @date 2024-07-01
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief MAX22530AWE+ external ADC driver
 */

#include "max22530.h"

#include <string.h>

/**
 * @brief Write to the ADC registers
 *
 * @param handler A pointer to the handler structure
 * @param address The address of the register to write to
 * @param data The data to write
 *
 * @return Max22530ReturnCode
 *     - MAX22530_OK
 */
Max22530ReturnCode _max22530_write(
    Max22530Handler * const handler,
    const max22530_address_t address,
    const uint16_t data)
{
    uint8_t cmd[MAX22530_COMMAND_BYTE_SIZE] = {
        (address << 2U) | (MAX22530_COMMAND_WRITE << 1U),
        (data & 0xff00) >> 8U,
        data & 0x00ff
    }; 
    handler->send(SPI_NETWORK_ADC, cmd, MAX22530_COMMAND_BYTE_SIZE);
    return MAX22530_OK;
}

/**
 * @brief Read from an ADC registers
 *
 * @param handler A pointer to the handler structure
 * @param address The address of the register to read from
 *
 * @return uint16_t The data of the register
 */
uint16_t _max22530_read(Max22530Handler * const handler, const max22530_address_t address) {
    uint8_t cmd[MAX22530_COMMAND_BYTE_SIZE];
    cmd[0U] = (address << 2U) |
        (MAX22530_COMMAND_READ << 1U) |
        (MAX22530_BURST_OFF);

    // Fill out every other byte with 1s so that they are ignored
    for (size_t i = 1U; i < MAX22530_COMMAND_BYTE_SIZE; ++i)
        cmd[i] = MAX22530_BYTE_UNUSED;
    handler->send_receive(SPI_NETWORK_ADC, cmd, &cmd[1U], 1U, MAX22530_COMMAND_BYTE_SIZE - 1U);
    return ((uint16_t)cmd[1U] << 8U) | cmd[2U];
}

/**
 * @brief Read all the channels data from the ADC
 *
 * @param handler A pointer to the handler structure
 * @param filtered If true reads the filtered values
 * @param out[out] A pointer to the array where the data is stored
 *
 * @return Max22530ReturnCode
 *     - MAX22530_OK
 */
Max22530ReturnCode _max22530_burst(
    Max22530Handler * const handler,
    const bool filtered,
    uint16_t * const out) {
    const max22530_address_t address = filtered ?
        MAX22530_REGISTER_FILTERED_ADC :
        MAX22530_REGISTER_ADC;

    uint8_t cmd[MAX22530_BURST_BYTE_SIZE];
    cmd[0U] = (address << 2U) | (MAX22530_COMMAND_READ << 1U) | (MAX22530_BURST_ON);

    // Fill out every other byte with 1s so that they are ignored
    for (size_t i = 1U; i < MAX22530_BURST_BYTE_SIZE; ++i)
        cmd[i] = MAX22530_BYTE_UNUSED;

    handler->send_receive(SPI_NETWORK_ADC, cmd, &cmd[1U], 1U, MAX22530_BURST_BYTE_SIZE - 1U);

    for (size_t i = 0U; i < MAX22530_CHANNEL_COUNT + 1U; ++i)
        out[i] = ((cmd[i * 2U + 1U] & 0x0f) << 8U) | cmd[i * 2U + 2U];
    return MAX22530_OK;
}

Max22530ReturnCode max22530_init(
    Max22530Handler * const handler,
    const spi_send_callback_t send,
    const spi_send_receive_callback_t send_receive)
{
    if (handler == NULL ||
        send == NULL ||
        send_receive == NULL)
        return MAX22530_NULL_POINTER;
    handler->send = send;
    handler->send_receive = send_receive;

    uint16_t data = 0;
    data = MAINBOARD_BIT_SET(data, MAX22530_CONTROL_FIELD_SW_RESET);
    data = MAINBOARD_BIT_SET(data, MAX22530_CONTROL_FIELD_CLEAR_POR);
    return _max22530_write(handler, MAX22530_REGISTER_CONTROL, data);
}

max22530_id_t max22530_get_id(Max22530Handler * const handler) {
    if (handler == NULL)
        return -1;
    const int16_t data = _max22530_read(handler, MAX22530_REGISTER_ID);
    return data >> 8U;
}

Max22530PowerOnReset max22530_get_power_on_reset(Max22530Handler * const handler) {
    if (handler == NULL)
        return -1;
    const int16_t data = _max22530_read(handler, MAX22530_REGISTER_ID);
    return (data & 0x80) >> 7U;
}

max22530_revision_t max22530_get_revision(Max22530Handler * const handler) {
    if (handler == NULL)
        return -1;
    const int16_t data = _max22530_read(handler, MAX22530_REGISTER_ID);
    return data & 0x7F;
}

volt_t max22530_read_channel(
    Max22530Handler * const handler,
    const Max22530Channel channel,
    const bool filtered)
{
    if (handler == NULL)
        return 0U;
    // Get the channel register address
    max22530_address_t address = filtered ?
        MAX22530_REGISTER_FILTERED_ADC :
        MAX22530_REGISTER_ADC;
    address += channel;

    // Get data
    const raw_volt_t data = _max22530_read(handler, address);
    return MAX22530_RAW_VALUE_TO_VOLT(data);
}

Max22530ReturnCode max22530_read_channels_all(
    Max22530Handler * const handler,
    const bool filtered,
    volt_t * const out,
    uint16_t * const interrupt_status)
{
    if (handler == NULL || out == NULL)
        return MAX22530_NULL_POINTER;
    // Get data
    raw_volt_t data[MAX22530_CHANNEL_COUNT + 1U] = { 0 };
    const Max22530ReturnCode code = _max22530_burst(handler, filtered, data);

    // Copy data
    for (size_t i = 0U; i < MAX22530_CHANNEL_COUNT; ++i)
        out[i] = MAX22530_RAW_VALUE_TO_VOLT(data[i]);
    if (interrupt_status != NULL)
        *interrupt_status = data[MAX22530_CHANNEL_COUNT];
    return code;
}
