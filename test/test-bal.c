
#include "unity.h"
#include "bal.h"
#include "identity.h"
#include "mainboard-def.h"
#include "timebase.h"
#include "volt.h"
#include <string.h>

extern _BalHandler hbal;

void test_bal_init_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(BAL_OK, bal_init(), "bal_init() failed to return BAL_OK");
}

void test_bal_init_event_type() {
    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, hbal.event.type, "Initial event type should be IGNORED");
}

void test_bal_init_target() {
    TEST_ASSERT_EQUAL_MESSAGE(BAL_TARGET_MAX_V, hbal.params.target, "Target voltage did not initialize to safe MAX_V");
}

void test_bal_init_threshold() {
    TEST_ASSERT_EQUAL_MESSAGE(BAL_THRESHOLD_MAX_V, hbal.params.threshold, "Threshold voltage did not initialize to safe MAX_V");
}

void test_bal_is_active_false() {
    TEST_ASSERT_FALSE_MESSAGE(bal_is_active(), "Module should be inactive by default");
}

void test_bal_is_active_true() {
    hbal.active = true; 
    TEST_ASSERT_TRUE_MESSAGE(bal_is_active(), "Module should be active when hbal.active is true");
}

void test_bal_start_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(BAL_OK, bal_start(), "bal_start() failed to return BAL_OK");
}

void test_bal_stop_ok() {
    hbal.active = true;
    TEST_ASSERT_EQUAL_MESSAGE(BAL_OK, bal_stop(), "bal_stop() failed to return BAL_OK");
}

void test_bal_stop_active() {
    hbal.active = true;
    bal_stop();
    TEST_ASSERT_FALSE_MESSAGE(bal_is_active(), "Module should be inactive after stop");
}

void test_bal_get_status_canlib_payload() {
    size_t byte_size;
    primary_hv_balancing_status_converted_t *payload = bal_get_status_canlib_payload(&byte_size);
    TEST_ASSERT_EQUAL_MESSAGE(&hbal.status_can_payload, payload, "Returned payload pointer does not match internal handler");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(hbal.status_can_payload), byte_size, "Returned payload size mismatch");
}

void test_bal_get_set_status_canlib_payload() {
    size_t byte_size;
    
    hbal.active = true;
    hbal.params.target = 3.5f;
    hbal.params.threshold = 0.05f;

    bms_cellboard_set_balancing_status_converted_t *payload = bal_get_set_status_canlib_payload(&byte_size);
    
    TEST_ASSERT_EQUAL_MESSAGE(&hbal.set_status_can_payload, payload, "Returned payload pointer does not match internal handler");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(hbal.set_status_can_payload), byte_size, "Returned payload size mismatch");
    
    TEST_ASSERT_TRUE_MESSAGE(payload->start, "Active status not correctly mapped to payload->start");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.5f, payload->target, "Target voltage not correctly mapped to payload");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.05f, payload->threshold, "Threshold voltage not correctly mapped to payload");
}

void test_bal_cellboard_balancing_status_handle_content() {
    bms_cellboard_balancing_status_converted_t input;
    memset(&input, 0, sizeof(input));
    
    input.status = 1;
    input.cellboard_id = 5;
    input.discharging_cell_0 = 1;
    input.discharging_cell_11 = 1;
    input.discharging_cell_23 = 1;

    bal_cellboard_balancing_status_handle(&input);

    TEST_ASSERT_EQUAL_MESSAGE(1, hbal.status_can_payload.status, "Status mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(5, hbal.status_can_payload.cellboard_id, "Cellboard ID mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(1, hbal.status_can_payload.discharging_cell_0, "Discharging cell 0 mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(0, hbal.status_can_payload.discharging_cell_1, "Discharging cell 1 should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(1, hbal.status_can_payload.discharging_cell_11, "Discharging cell 11 mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(1, hbal.status_can_payload.discharging_cell_23, "Discharging cell 23 mismatch");
}

void test_bal_set_balancing_state_from_steering_wheel_handle_target_clamp() {
    primary_hv_set_balancing_status_steering_wheel_converted_t payload;
    payload.status = true;
    payload.threshold = BAL_THRESHOLD_MAX_V;
    
    bal_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
        0.001f,
        BAL_TARGET_MIN_V,
        hbal.params.target,
        "Target voltage should be clamped to BAL_TARGET_MIN_V when volt_get_min() input is too low");
}

void test_bal_set_balancing_state_from_steering_wheel_handle_threshold() {
    primary_hv_set_balancing_status_steering_wheel_converted_t payload;
    payload.status = true;
    payload.threshold = BAL_THRESHOLD_MIN_V - 1.0f; // Force low value
    
    bal_set_balancing_state_from_steering_wheel_handle(&payload);

    float delta = (BAL_THRESHOLD_MIN_V + BAL_THRESHOLD_MAX_V) / 2.0f;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, BAL_THRESHOLD_MIN_V + delta, hbal.params.threshold, "Threshold voltage was not correctly clamped/validated");
}

void test_bal_set_balancing_state_from_steering_wheel_handle_event() {
    hbal.active = false; // Ensure current state is not balancing
    
    primary_hv_set_balancing_status_steering_wheel_converted_t payload;
    payload.status = true;
    bal_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_START, hbal.event.type, "Event type was not updated to BALANCING_START");
}

void test_bal_set_balancing_state_from_handcart_handle_threshold() {
    primary_hv_set_balancing_status_handcart_converted_t payload;
    payload.status = true;
    payload.threshold = BAL_THRESHOLD_MIN_V - 1.0f;
    
    bal_set_balancing_state_from_handcart_handle(&payload);

    float delta = (BAL_THRESHOLD_MIN_V + BAL_THRESHOLD_MAX_V) / 2.0f;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, BAL_THRESHOLD_MIN_V + delta, hbal.params.threshold, "Threshold voltage was not correctly clamped/validated");
}

void test_bal_set_balancing_state_from_handcart_handle_event() {
    hbal.active = false;
    
    primary_hv_set_balancing_status_handcart_converted_t payload;
    payload.status = true;
    bal_set_balancing_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_START, hbal.event.type, "Event type was not updated to BALANCING_START");
}

#ifdef BALANCING_TESTS

void setUp() {
    timebase_init(500U);
    bal_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_bal_init_ok);
    RUN_TEST(test_bal_init_event_type);
    RUN_TEST(test_bal_init_target);
    RUN_TEST(test_bal_init_threshold);
    RUN_TEST(test_bal_is_active_false);
    RUN_TEST(test_bal_is_active_true);
    RUN_TEST(test_bal_start_ok);
    RUN_TEST(test_bal_stop_ok);
    RUN_TEST(test_bal_stop_active);
    RUN_TEST(test_bal_get_status_canlib_payload);
    RUN_TEST(test_bal_get_set_status_canlib_payload);
    RUN_TEST(test_bal_cellboard_balancing_status_handle_content);
    RUN_TEST(test_bal_set_balancing_state_from_steering_wheel_handle_target_clamp);
    RUN_TEST(test_bal_set_balancing_state_from_steering_wheel_handle_threshold);
    RUN_TEST(test_bal_set_balancing_state_from_steering_wheel_handle_event);
    RUN_TEST(test_bal_set_balancing_state_from_handcart_handle_threshold);
    RUN_TEST(test_bal_set_balancing_state_from_handcart_handle_event);
    return UNITY_END();
}

#endif // BALANCING_TESTS