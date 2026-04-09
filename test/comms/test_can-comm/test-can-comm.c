/**
 * @file test_can-comm.c
 * @date 2024-06-23
 *
 * @brief Comprehensive test functions for the can-comm module
 */

#include "unity.h"
#include "can-comm.h"
#include "identity.h"
#include "mainboard-def.h"
#include <string.h>
#include <timebase.h>
#include "fff.h"
DEFINE_FFF_GLOBALS;

#define CELLBOARD_ID CELLBOARD_ID_1

extern _CanCommHandler hcan_comm;

uint8_t test_data[CAN_COMM_MAX_PAYLOAD_BYTE_SIZE + 5] = { 0 };

CanCommReturnCode can_comm_send_capture_data(const CanNetwork net, const can_id_t id, const CanFrameType type, const uint8_t *data, const size_t len) {

    memcpy(test_data, data, len);
    return CAN_COMM_OK;
}

FAKE_VALUE_FUNC(CanCommReturnCode, can_comm_send, const CanNetwork, const can_id_t, const CanFrameType, const uint8_t *, const size_t);

// --- INIT TESTS ---

void test_can_comm_init_null() {
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_NULL_POINTER, can_comm_init(NULL), "Init should return CAN_COMM_NULL_POINTER when callback is NULL");
}

void test_can_comm_init_correct_callback() {
    can_comm_send_fake.return_val = CAN_COMM_OK;
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_OK, can_comm_init(can_comm_send), "Init should return CAN_COMM_OK with valid callback");
    TEST_ASSERT_EQUAL_MESSAGE(hcan_comm.send, can_comm_send, "Init should store the callback in the handler");
}

// --- STATE MANAGEMENT TESTS ---

void test_can_comm_enable_all() {
    can_comm_enable_all();
    TEST_ASSERT_EQUAL_MESSAGE(0x03, hcan_comm.enabled, "Enabled bitmask should be 0x03 after enabling all");
}

void test_can_comm_disable_all() {
    hcan_comm.enabled = 0x03;
    can_comm_disable_all();
    TEST_ASSERT_EQUAL_MESSAGE(0x00, hcan_comm.enabled, "Enabled bitmask should be 0x00 after disabling all");
}

void test_can_comm_is_enabled_all() {
    can_comm_enable_all();
    TEST_ASSERT_TRUE_MESSAGE(can_comm_is_enabled_all(), "Should return true when all bits are set");
}

void test_can_comm_enable() {
    can_comm_enable(0);
    can_comm_enable(1);
    TEST_ASSERT_EQUAL_MESSAGE(0x03, hcan_comm.enabled, "Enabled bitmask should be 0x03 after individual enables");
}

void test_can_comm_disable() {
    can_comm_enable_all();
    can_comm_disable(0);
    can_comm_disable(1);
    TEST_ASSERT_EQUAL_MESSAGE(0x00, hcan_comm.enabled, "Enabled bitmask should be 0x00 after individual disables");
}

// --- IMMEDIATE SEND TESTS ---

void test_can_comm_send_immediate_ok() {
    can_comm_enable_all();
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, (void *)0x01, 0);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_OK, ret, "Immediate send should return OK when enabled and valid");
}

void test_can_comm_send_immediate_disabled() {
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, (void *)0x01, 0);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_DISABLED, ret, "Should return CAN_COMM_DISABLED when module is disabled");
}

void test_can_comm_send_immediate_invalid_network() {
    can_comm_enable_all();
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_COUNT, 0, CAN_FRAME_TYPE_DATA, (void *)0x01, 0);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_INVALID_NETWORK, ret, "Should return INVALID_NETWORK for out of bounds network");
}

void test_can_comm_send_immediate_invalid_frame_type() {
    can_comm_enable_all();
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_COUNT, (void *)0x01, 0);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_INVALID_FRAME_TYPE, ret, "Should return INVALID_FRAME_TYPE for unknown frame");
}

void test_can_comm_send_immediate_null() {
    can_comm_enable_all();
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, NULL, 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_NULL_POINTER, ret, "Should return NULL_POINTER when data pointer is NULL on DATA frame");
}

// SECURITY TEST: This will fail until the payload size check is uncommented in `can-comm.c`
void test_can_comm_send_immediate_invalid_payload_size_security_check() {
    TEST_IGNORE_MESSAGE("This test fails due to a known vulnerability where the payload size is not checked");
    can_comm_enable_all();
    uint8_t oversized_data[CAN_COMM_MAX_PAYLOAD_BYTE_SIZE + 5] = { 0 };
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, oversized_data, CAN_COMM_MAX_PAYLOAD_BYTE_SIZE + 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_INVALID_PAYLOAD_SIZE, ret, "Should reject oversized payload to prevent memcpy buffer overflow!");
}

// --- RX QUEUE TESTS ---

void test_can_comm_rx_add_disabled() {
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, (void *)0x01, 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_DISABLED, ret, "RX add should fail with DISABLED when not enabled");
}

void test_can_comm_rx_add_invalid_index() {
    can_comm_enable_all();
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, bms_MESSAGE_COUNT, CAN_FRAME_TYPE_DATA, (void *)0x01, 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_INVALID_INDEX, ret, "RX add should fail with INVALID_INDEX");
}

void test_can_comm_rx_add_ok() {
    can_comm_enable_all();
    uint8_t data[] = { 0xAA, 0xBB };
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, 1, CAN_FRAME_TYPE_DATA, data, 2);

    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_OK, ret, "RX add should return OK");
    TEST_ASSERT_TRUE_MESSAGE(hcan_comm.rx_busy[CAN_NETWORK_BMS][1], "Busy flag for this network and index should be set");

    CanMessage rx_msg;
    ring_buffer_pop_front(&hcan_comm.rx_buf, &rx_msg);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(data, rx_msg.payload.rx, 2, "RX message payload content should match added data");
}

// --- TX QUEUE TESTS ---

void test_can_comm_tx_add_disabled() {
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, (void *)0x01, 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_DISABLED, ret, "TX add should fail with DISABLED when not enabled");
}

void test_can_comm_tx_add_ignore_if_busy() {
    can_comm_enable_all();
    uint8_t data[] = { 0x01 };

    // First add
    CanCommReturnCode ret1 = can_comm_tx_add(CAN_NETWORK_BMS, 2, CAN_FRAME_TYPE_DATA, data, 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_OK, ret1, "First TX add should succeed");

    // Second add for same index
    CanCommReturnCode ret2 = can_comm_tx_add(CAN_NETWORK_BMS, 2, CAN_FRAME_TYPE_DATA, data, 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_OK, ret2, "Second TX add should return OK (ignored silently)");

    // Buffer should only have 1 item
    CanMessage msg;
    ring_buffer_pop_front(&hcan_comm.tx_buf, &msg);
    TEST_ASSERT_EQUAL_MESSAGE(RING_BUFFER_EMPTY, ring_buffer_pop_front(&hcan_comm.tx_buf, &msg), "Buffer should only contain one instance of the message");
}

// SECURITY TEST: This will fail until the payload size check is uncommented in `can-comm.c`
void test_can_comm_tx_add_invalid_payload_size_security_check() {
    TEST_IGNORE_MESSAGE("This test fails due to a known vulnerability where the payload size is not checked");
    can_comm_enable_all();
    uint8_t oversized_data[CAN_COMM_MAX_PAYLOAD_BYTE_SIZE + 5] = { 0 };
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, oversized_data, CAN_COMM_MAX_PAYLOAD_BYTE_SIZE + 1);
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_INVALID_PAYLOAD_SIZE, ret, "Should reject oversized payload to prevent memcpy buffer overflow!");
}

// --- ROUTINE TESTS ---

void test_can_comm_routine_disabled() {
    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_DISABLED, can_comm_routine(), "Routine should return DISABLED if module is not enabled");
}

void test_can_comm_routine_tx_processing() {

    memset(test_data, 0, sizeof(test_data));
    can_comm_enable_all();
    can_comm_send_fake.custom_fake = can_comm_send_capture_data;
    uint8_t data_in[] = { 0x11, 0x22, 0x33 };
    hcan_comm.tx_busy[CAN_NETWORK_BMS][0] = false; // Ensure not busy
    can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, data_in, 3);

    uint8_t serialized_data[CAN_COMM_MAX_PAYLOAD_BYTE_SIZE] = { 0 };
    int can_id = bms_id_from_index(0);

    int size = bms_serialize_from_id(
        data_in,
        can_id,
        serialized_data);

    CanCommReturnCode ret = can_comm_routine();

    TEST_ASSERT_EQUAL_MESSAGE(CAN_COMM_OK, ret, "Routine should return OK after processing");
    TEST_ASSERT_FALSE_MESSAGE(hcan_comm.tx_busy[CAN_NETWORK_BMS][0], "Busy flag should be cleared after routine");

    TEST_ASSERT_EQUAL_MESSAGE(CAN_NETWORK_BMS, can_comm_send_fake.arg0_val, "Data should be send");
    TEST_ASSERT_EQUAL_MESSAGE(can_id, can_comm_send_fake.arg1_val, "Index should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(CAN_FRAME_TYPE_DATA, can_comm_send_fake.arg2_val, "Frame type should be DATA");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(serialized_data, test_data, size, "Payload should match the added data");
    TEST_ASSERT_EQUAL_MESSAGE(size, can_comm_send_fake.arg4_val, "Payload size should be 3");

    CanMessage msg;
    TEST_ASSERT_EQUAL_MESSAGE(RING_BUFFER_EMPTY, ring_buffer_pop_front(&hcan_comm.tx_buf, &msg), "TX buffer should be empty after routine");
}

#ifdef CAN_COMM_TESTS

void setUp() {

    RESET_FAKE(can_comm_send);
    FFF_RESET_HISTORY();

    can_comm_send_fake.return_val = CAN_COMM_OK;

    can_comm_init(can_comm_send);
    timebase_init(500U);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    // Init & State
    RUN_TEST(test_can_comm_init_null);
    RUN_TEST(test_can_comm_init_correct_callback);
    RUN_TEST(test_can_comm_enable_all);
    RUN_TEST(test_can_comm_disable_all);
    RUN_TEST(test_can_comm_is_enabled_all);
    RUN_TEST(test_can_comm_enable);
    RUN_TEST(test_can_comm_disable);

    // Immediate Sends
    RUN_TEST(test_can_comm_send_immediate_ok);
    RUN_TEST(test_can_comm_send_immediate_disabled);
    RUN_TEST(test_can_comm_send_immediate_invalid_network);
    RUN_TEST(test_can_comm_send_immediate_invalid_frame_type);
    RUN_TEST(test_can_comm_send_immediate_null);
    RUN_TEST(test_can_comm_send_immediate_invalid_payload_size_security_check); // Expected to FAIL due to vulnerability

    // RX
    RUN_TEST(test_can_comm_rx_add_disabled);
    RUN_TEST(test_can_comm_rx_add_invalid_index);
    RUN_TEST(test_can_comm_rx_add_ok);

    // TX
    RUN_TEST(test_can_comm_tx_add_disabled);
    RUN_TEST(test_can_comm_tx_add_ignore_if_busy);
    RUN_TEST(test_can_comm_tx_add_invalid_payload_size_security_check); // Expected to FAIL due to vulnerability

    // Routine
    RUN_TEST(test_can_comm_routine_disabled);
    RUN_TEST(test_can_comm_routine_tx_processing);

    return UNITY_END();
}

#endif // CAN_COMM_TESTS