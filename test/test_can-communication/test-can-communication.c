/*!
 * \file test-can-communication.c
 * \date 2026-10-07
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tests for the CAN-bus communication module
 *
 * PAL is tested externally, only the logic of this module is tested here
 */
#include "can-communication.h"
#include "can-communication-api.h"
#include "eagletrt-api.h"
#include "unity.h"
#include <stdint.h>
#include <string.h>

#include <fff.h>

DEFINE_FFF_GLOBALS;

extern struct CanCommunicationHandler handler;

FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, send_primary, const struct CanCommunicationFrame *);
FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, send_bms, const struct CanCommunicationFrame *);
FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, on_receive_primary, struct CanCommunicationFrame *);
FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, on_receive_bms, struct CanCommunicationFrame *);
FAKE_VOID_FUNC(cs_enter);
FAKE_VOID_FUNC(cs_exit);

static struct CanCommunicationFrame last_sent[CAN_COMMUNICATION_NETWORK_COUNT];
static struct CanCommunicationFrame last_received[CAN_COMMUNICATION_NETWORK_COUNT];

static enum CanCommunicationReturnCode prv_send_primary_capture(const struct CanCommunicationFrame *frame) {
    last_sent[CAN_COMMUNICATION_NETWORK_PRIMARY] = *frame;
    return CAN_COMMUNICATION_RC_OK;
}

static enum CanCommunicationReturnCode prv_send_bms_capture(const struct CanCommunicationFrame *frame) {
    last_sent[CAN_COMMUNICATION_NETWORK_BMS] = *frame;
    return CAN_COMMUNICATION_RC_OK;
}

static enum CanCommunicationReturnCode prv_on_receive_primary_capture(struct CanCommunicationFrame *frame) {
    last_received[CAN_COMMUNICATION_NETWORK_PRIMARY] = *frame;
    return CAN_COMMUNICATION_RC_OK;
}

static enum CanCommunicationReturnCode prv_on_receive_bms_capture(struct CanCommunicationFrame *frame) {
    last_received[CAN_COMMUNICATION_NETWORK_BMS] = *frame;
    return CAN_COMMUNICATION_RC_OK;
}

static struct CanCommunicationNetworkConfig prv_valid_configs[CAN_COMMUNICATION_NETWORK_COUNT];

static struct CanCommunicationFrame prv_make_frame(uint32_t id, uint8_t length) {
    struct CanCommunicationFrame frame = { .id = id, .length = length };
    for (uint8_t i = 0U; i < CAN_COMMUNICATION_FRAME_DATA_SIZE; ++i) {
        frame.data[i] = (uint8_t)(id + i);
    }
    return frame;
}

/*!
 * \defgroup            init Tests for the initialization function
 * @{
 */

void test_can_communication_init_with_null_configs(void) {
    const enum CanCommunicationReturnCode result = can_communication_api_init(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_NULL_POINTER, result, "Expected NULL_POINTER with NULL configs");
}

void test_can_communication_init_with_null_send(void) {
    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        struct CanCommunicationNetworkConfig configs[CAN_COMMUNICATION_NETWORK_COUNT];
        memcpy(configs, prv_valid_configs, sizeof(configs));
        configs[network].send = NULL;

        const enum CanCommunicationReturnCode result = can_communication_api_init(configs);

        TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_NULL_POINTER, result, "Expected NULL_POINTER with NULL send callback");
    }
}

void test_can_communication_init_with_null_on_receive(void) {
    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        struct CanCommunicationNetworkConfig configs[CAN_COMMUNICATION_NETWORK_COUNT];
        memcpy(configs, prv_valid_configs, sizeof(configs));
        configs[network].on_receive = NULL;

        const enum CanCommunicationReturnCode result = can_communication_api_init(configs);

        TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_NULL_POINTER, result, "Expected NULL_POINTER with NULL on_receive callback");
    }
}

void test_can_communication_init_with_null_critical_section_is_ok(void) {
    struct CanCommunicationNetworkConfig configs[CAN_COMMUNICATION_NETWORK_COUNT];
    memcpy(configs, prv_valid_configs, sizeof(configs));
    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        configs[network].cs_enter = NULL;
        configs[network].cs_exit = NULL;
    }

    const enum CanCommunicationReturnCode result = can_communication_api_init(configs);

    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_OK, result, "Expected OK with NULL critical section callbacks");
}

void test_can_communication_init_with_valid_configs(void) {
    const enum CanCommunicationReturnCode result = can_communication_api_init(prv_valid_configs);

    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_OK, result, "Expected OK with valid configs");
}

void test_can_communication_init_stores_callbacks(void) {
    EAGLETRT_API_UNUSED(can_communication_api_init(prv_valid_configs));

    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        TEST_ASSERT_TRUE(handler.networks[network].send == prv_valid_configs[network].send);
        TEST_ASSERT_TRUE(handler.networks[network].on_receive == prv_valid_configs[network].on_receive);
    }
}

/*! @} */

/*!
 * \defgroup            add_to_queue Tests for the add to tx and rx queue functions
 * @{
 */

void test_can_communication_add_to_tx_with_null_frame(void) {
    const enum CanCommunicationReturnCode result = can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_BMS, NULL);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_NULL_POINTER, result);
}

void test_can_communication_add_to_rx_with_null_frame(void) {
    const enum CanCommunicationReturnCode result = can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_BMS, NULL);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_NULL_POINTER, result);
}

void test_can_communication_add_to_tx_with_invalid_network(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x100U, 8U);

    const enum CanCommunicationReturnCode result = can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_COUNT, &frame);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_NETWORK, result);
}

void test_can_communication_add_to_rx_with_invalid_network(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x100U, 8U);

    const enum CanCommunicationReturnCode result = can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_COUNT, &frame);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_NETWORK, result);
}

void test_can_communication_add_to_tx_with_invalid_length(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x100U, CAN_COMMUNICATION_FRAME_DATA_SIZE + 1U);

    const enum CanCommunicationReturnCode result = can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_BMS, &frame);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_LENGTH, result);
}

void test_can_communication_add_to_rx_with_invalid_length(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x100U, CAN_COMMUNICATION_FRAME_DATA_SIZE + 1U);

    const enum CanCommunicationReturnCode result = can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_BMS, &frame);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_LENGTH, result);
}

void test_can_communication_add_validation_order(void) {
    const struct CanCommunicationFrame bad_frame = prv_make_frame(0x100U, CAN_COMMUNICATION_FRAME_DATA_SIZE + 1U);

    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_NULL_POINTER, can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_COUNT, NULL), "NULL frame must be checked before the network");
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_INVALID_NETWORK, can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_COUNT, &bad_frame), "Network must be checked before the length");
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_NULL_POINTER, can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_COUNT, NULL), "NULL frame must be checked before the network");
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMMUNICATION_RC_INVALID_NETWORK, can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_COUNT, &bad_frame), "Network must be checked before the length");
}

void test_can_communication_add_with_valid_frame(void) {
    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        for (uint8_t length = 0U; length <= CAN_COMMUNICATION_FRAME_DATA_SIZE; ++length) {
            const struct CanCommunicationFrame frame = prv_make_frame(0x100U + length, length);

            TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_api_add_to_tx(network, &frame));
            TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_api_add_to_rx(network, &frame));
        }
    }
}

/*! @} */

/*!
 * \defgroup            process_tx Tests for the process tx function
 * @{
 */

void test_can_communication_process_tx_with_invalid_network(void) {
    const enum CanCommunicationReturnCode result = can_communication_api_process_tx(CAN_COMMUNICATION_NETWORK_COUNT);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_NETWORK, result);
}

void test_can_communication_process_tx_with_empty_queue(void) {
    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_api_process_tx(network));
    }

    TEST_ASSERT_EQUAL(0U, send_primary_fake.call_count);
    TEST_ASSERT_EQUAL(0U, send_bms_fake.call_count);
}

void test_can_communication_process_tx_sends_frame_to_own_network(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x123U, 5U);
    EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_BMS, &frame));

    const enum CanCommunicationReturnCode result = can_communication_api_process_tx(CAN_COMMUNICATION_NETWORK_BMS);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, result);
    TEST_ASSERT_EQUAL_MESSAGE(1U, send_bms_fake.call_count, "Expected the BMS send callback to be called once");
    TEST_ASSERT_EQUAL_MESSAGE(0U, send_primary_fake.call_count, "Expected the primary send callback not to be called");
    TEST_ASSERT_EQUAL_UINT32(frame.id, last_sent[CAN_COMMUNICATION_NETWORK_BMS].id);
    TEST_ASSERT_EQUAL_UINT8(frame.length, last_sent[CAN_COMMUNICATION_NETWORK_BMS].length);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(frame.data, last_sent[CAN_COMMUNICATION_NETWORK_BMS].data, CAN_COMMUNICATION_FRAME_DATA_SIZE);
}

void test_can_communication_process_tx_sends_primary_frame_to_primary_network(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x321U, 8U);
    EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));

    const enum CanCommunicationReturnCode result = can_communication_api_process_tx(CAN_COMMUNICATION_NETWORK_PRIMARY);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, result);
    TEST_ASSERT_EQUAL(1U, send_primary_fake.call_count);
    TEST_ASSERT_EQUAL(0U, send_bms_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(frame.id, last_sent[CAN_COMMUNICATION_NETWORK_PRIMARY].id);
}

void test_can_communication_process_tx_with_failing_send(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x123U, 5U);
    send_bms_fake.custom_fake = NULL;
    send_bms_fake.return_val = CAN_COMMUNICATION_RC_ERROR;
    EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_COMMUNICATION_NETWORK_BMS, &frame));

    const enum CanCommunicationReturnCode result = can_communication_api_process_tx(CAN_COMMUNICATION_NETWORK_BMS);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_TRANSMISSION_ERROR, result);
    TEST_ASSERT_EQUAL(1U, send_bms_fake.call_count);
}

/*! @} */

/*!
 * \defgroup            process_rx Tests for the process rx function
 * @{
 */

void test_can_communication_process_rx_with_invalid_network(void) {
    const enum CanCommunicationReturnCode result = can_communication_api_process_rx(CAN_COMMUNICATION_NETWORK_COUNT);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_NETWORK, result);
}

void test_can_communication_process_rx_with_empty_queue(void) {
    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_api_process_rx(network));
    }

    TEST_ASSERT_EQUAL(0U, on_receive_primary_fake.call_count);
    TEST_ASSERT_EQUAL(0U, on_receive_bms_fake.call_count);
}

void test_can_communication_process_rx_dispatches_frame_to_own_network(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x456U, 6U);
    EAGLETRT_API_UNUSED(can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));

    const enum CanCommunicationReturnCode result = can_communication_api_process_rx(CAN_COMMUNICATION_NETWORK_PRIMARY);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, result);
    TEST_ASSERT_EQUAL_MESSAGE(1U, on_receive_primary_fake.call_count, "Expected the primary receive callback to be called once");
    TEST_ASSERT_EQUAL_MESSAGE(0U, on_receive_bms_fake.call_count, "Expected the BMS receive callback not to be called");
    TEST_ASSERT_EQUAL_UINT32(frame.id, last_received[CAN_COMMUNICATION_NETWORK_PRIMARY].id);
    TEST_ASSERT_EQUAL_UINT8(frame.length, last_received[CAN_COMMUNICATION_NETWORK_PRIMARY].length);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(frame.data, last_received[CAN_COMMUNICATION_NETWORK_PRIMARY].data, CAN_COMMUNICATION_FRAME_DATA_SIZE);
}

void test_can_communication_process_rx_dispatches_bms_frame_to_bms_network(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x654U, 2U);
    EAGLETRT_API_UNUSED(can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_BMS, &frame));

    const enum CanCommunicationReturnCode result = can_communication_api_process_rx(CAN_COMMUNICATION_NETWORK_BMS);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, result);
    TEST_ASSERT_EQUAL(1U, on_receive_bms_fake.call_count);
    TEST_ASSERT_EQUAL(0U, on_receive_primary_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(frame.id, last_received[CAN_COMMUNICATION_NETWORK_BMS].id);
}

void test_can_communication_process_rx_with_failing_handler(void) {
    const struct CanCommunicationFrame frame = prv_make_frame(0x456U, 6U);
    on_receive_primary_fake.custom_fake = NULL;
    on_receive_primary_fake.return_val = CAN_COMMUNICATION_RC_ERROR;
    EAGLETRT_API_UNUSED(can_communication_api_add_to_rx(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));

    const enum CanCommunicationReturnCode result = can_communication_api_process_rx(CAN_COMMUNICATION_NETWORK_PRIMARY);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR, result);
    TEST_ASSERT_EQUAL(1U, on_receive_primary_fake.call_count);
}

/*! @} */

void setUp(void) {
    RESET_FAKE(send_primary);
    RESET_FAKE(send_bms);
    RESET_FAKE(on_receive_primary);
    RESET_FAKE(on_receive_bms);
    RESET_FAKE(cs_enter);
    RESET_FAKE(cs_exit);
    memset(last_sent, 0, sizeof(last_sent));
    memset(last_received, 0, sizeof(last_received));

    send_primary_fake.custom_fake = prv_send_primary_capture;
    send_bms_fake.custom_fake = prv_send_bms_capture;
    on_receive_primary_fake.custom_fake = prv_on_receive_primary_capture;
    on_receive_bms_fake.custom_fake = prv_on_receive_bms_capture;

    prv_valid_configs[CAN_COMMUNICATION_NETWORK_PRIMARY] = (struct CanCommunicationNetworkConfig){
        .send = send_primary,
        .on_receive = on_receive_primary,
        .cs_enter = cs_enter,
        .cs_exit = cs_exit
    };
    prv_valid_configs[CAN_COMMUNICATION_NETWORK_BMS] = (struct CanCommunicationNetworkConfig){
        .send = send_bms,
        .on_receive = on_receive_bms,
        .cs_enter = cs_enter,
        .cs_exit = cs_exit
    };

    EAGLETRT_API_UNUSED(can_communication_api_init(prv_valid_configs));
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    /*! \addgroup init @{ */
    RUN_TEST(test_can_communication_init_with_null_configs);
    RUN_TEST(test_can_communication_init_with_null_send);
    RUN_TEST(test_can_communication_init_with_null_on_receive);
    RUN_TEST(test_can_communication_init_with_null_critical_section_is_ok);
    RUN_TEST(test_can_communication_init_with_valid_configs);
    RUN_TEST(test_can_communication_init_stores_callbacks);
    /*! @} */

    /*! \addgroup add_to_queue @{ */
    RUN_TEST(test_can_communication_add_to_tx_with_null_frame);
    RUN_TEST(test_can_communication_add_to_rx_with_null_frame);
    RUN_TEST(test_can_communication_add_to_tx_with_invalid_network);
    RUN_TEST(test_can_communication_add_to_rx_with_invalid_network);
    RUN_TEST(test_can_communication_add_to_tx_with_invalid_length);
    RUN_TEST(test_can_communication_add_to_rx_with_invalid_length);
    RUN_TEST(test_can_communication_add_validation_order);
    RUN_TEST(test_can_communication_add_with_valid_frame);
    /*! @} */

    /*! \addgroup process_tx @{ */
    RUN_TEST(test_can_communication_process_tx_with_invalid_network);
    RUN_TEST(test_can_communication_process_tx_with_empty_queue);
    RUN_TEST(test_can_communication_process_tx_sends_frame_to_own_network);
    RUN_TEST(test_can_communication_process_tx_sends_primary_frame_to_primary_network);
    RUN_TEST(test_can_communication_process_tx_with_failing_send);
    /*! @} */

    /*! \addgroup process_rx @{ */
    RUN_TEST(test_can_communication_process_rx_with_invalid_network);
    RUN_TEST(test_can_communication_process_rx_with_empty_queue);
    RUN_TEST(test_can_communication_process_rx_dispatches_frame_to_own_network);
    RUN_TEST(test_can_communication_process_rx_dispatches_bms_frame_to_bms_network);
    RUN_TEST(test_can_communication_process_rx_with_failing_handler);
    /*! @} */

    return UNITY_END();
}
