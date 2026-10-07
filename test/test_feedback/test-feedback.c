/*!
 * \file test-feedback.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback management function
 */
#include "feedback.h"
#include "eagletrt-api.h"
#include "unity.h"
#include "feedback-api.h"
#include "mainboard-def.h"
#include "unity_internals.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fff.h>

DEFINE_FFF_GLOBALS;

extern struct FeedbackHandler feedback_handler;

extern enum FeedbackId prv_feedback_get_id_from_digital_bit(enum FeedbackDigitalBit bit);
extern enum FeedbackId prv_feedback_get_id_from_analog_index(enum FeedbackAnalogIndex index);
extern enum FeedbackStatus prv_feedback_api_get_analog_status(enum FeedbackAnalogIndex index, volt_t thr_high);

FAKE_VALUE_FUNC(uint32_t, feedback_read_digital_all);
FAKE_VOID_FUNC(feedback_start_analog_conversion);
FAKE_VALUE_FUNC(bool, feedback_read_hc_connected);

/*!
 * \defgroup            id_from_digital_bit Test for feedback ID from digital bit index function
 * @{
 */

void test_feedback_get_id_from_digital_bit_returns_correct_id(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_digital_bit(FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_AIRN_OPEN_COM, result, "Expected AIRN_OPEN_COM id for AIRN_OPEN_COM digital bit");
}

void test_feedback_get_id_from_digital_bit_returns_invalid_for_unknown_bit(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_digital_bit((enum FeedbackDigitalBit)FEEDBACK_DIGITAL_BIT_COUNT);

    TEST_ASSERT_EQUAL(FEEDBACK_ID_INVALID, result);
}

void test_feedback_digital_bit_and_id_mappings_are_inverse_of_each_other(void) {
    for (enum FeedbackDigitalBit bit = 0U; bit < FEEDBACK_DIGITAL_BIT_COUNT; ++bit) {
        const enum FeedbackId id = prv_feedback_get_id_from_digital_bit(bit);

        TEST_ASSERT_LESS_THAN_UINT8(FEEDBACK_ID_COUNT, id);
        TEST_ASSERT_TRUE(feedback_api_is_digital(id));
        TEST_ASSERT_EQUAL_UINT8(bit, feedback_api_get_digital_bit_from_id(id));
    }
}

/*! @} */

/*!
 * \defgroup            id_from_analog_index Tests for feedback ID from analog index function
 * @{
 */

void test_feedback_get_id_from_analog_index_returns_correct_id(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_analog_index(FEEDBACK_ANALOG_INDEX_IMD_OK);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_IMD_OK, result, "Expected IMD_OK id for IMD_OK analog index");
}

void test_feedback_get_id_from_analog_index_returns_invalid_for_unknown_index(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_analog_index((enum FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT);

    TEST_ASSERT_EQUAL(FEEDBACK_ID_INVALID, result);
}

void test_feedback_analog_index_and_id_mappings_are_inverse_of_each_other(void) {
    for (enum FeedbackAnalogIndex index = 0U; index < FEEDBACK_ANALOG_INDEX_COUNT; ++index) {
        const enum FeedbackId id = prv_feedback_get_id_from_analog_index(index);

        TEST_ASSERT_LESS_THAN_UINT8(FEEDBACK_ID_COUNT, id);
        TEST_ASSERT_FALSE(feedback_api_is_digital(id));
        TEST_ASSERT_EQUAL_UINT8(index, feedback_api_get_analog_index_from_id(id));
    }
}

/*! @} */

/*!
 * \defgroup            get_analog_status Test get analog status
 * @{
 */
void test_feedback_api_get_analog_status_probing_3v3_returns_high_when_in_range(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "Expected HIGH status for PROBING_3V3 value within valid range");
}

void test_feedback_api_get_analog_status_probing_3v3_returns_error_when_above_high_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = FEEDBACK_THRESHOLD_HIGH_V + 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status for PROBING_3V3 value above high threshold");
}

void test_feedback_api_get_analog_status_probing_3v3_returns_error_when_below_low_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = FEEDBACK_THRESHOLD_LOW_V - 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status for PROBING_3V3 value below low threshold");
}

void test_feedback_api_get_analog_status_probing_3v3_ignores_given_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = 1.6F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3, 0.1F);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "PROBING_3V3 must not depend on the high threshold argument");
}

void test_feedback_api_get_analog_status_returns_low_when_below_low_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_LOW_V - 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, result, "Expected LOW status when analog value is below low threshold");
}

void test_feedback_api_get_analog_status_returns_high_when_above_high_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "Expected HIGH status when analog value is above high threshold");
}

void test_feedback_api_get_analog_status_returns_error_when_between_thresholds(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status when analog value is in the forbidden mid-range");
}

void test_feedback_api_get_analog_status_value_equal_to_high_threshold_is_high(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "Value equal to the high threshold should be HIGH");
}

void test_feedback_api_get_analog_status_value_equal_to_low_threshold_is_low(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_LOW_V;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN, FEEDBACK_THRESHOLD_HIGH_V);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, result, "Value equal to the low threshold should be LOW");
}

void test_feedback_api_get_analog_status_uses_given_high_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = 1.2F;

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN, FEEDBACK_THRESHOLD_HIGH_HC_V), "1.2V should be HIGH with the HC threshold");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN, FEEDBACK_THRESHOLD_HIGH_V), "1.2V should be ERROR with the default threshold");
}

/*! @} */

/*!
 * \defgroup            init Test initialization
 * @{
 */

void test_feedback_api_init_returns_null_pointer_when_read_callback_is_null(void) {
    const enum FeedbackReturnCode result = feedback_api_init(NULL, feedback_start_analog_conversion, feedback_read_hc_connected);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_NULL_POINTER, result, "Expected NULL_POINTER when read callback is NULL");
}

void test_feedback_api_init_returns_null_pointer_when_start_conversion_callback_is_null(void) {
    const enum FeedbackReturnCode result = feedback_api_init(feedback_read_digital_all, NULL, feedback_read_hc_connected);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_NULL_POINTER, result, "Expected NULL_POINTER when start_conversion callback is NULL");
}

void test_feedback_api_init_returns_null_pointer_when_hc_callback_is_null(void) {
    const enum FeedbackReturnCode result = feedback_api_init(feedback_read_digital_all, feedback_start_analog_conversion, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_NULL_POINTER, result, "Expected NULL_POINTER when hc connected callback is NULL");
}

void test_feedback_api_init_saves_callbacks_and_returns_ok(void) {
    const enum FeedbackReturnCode result = feedback_api_init(feedback_read_digital_all, feedback_start_analog_conversion, feedback_read_hc_connected);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_OK, result, "Expected OK return code after successful init");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(feedback_read_digital_all, feedback_handler.read_digital, "read_digital callback not stored correctly");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(feedback_start_analog_conversion, feedback_handler.start_conversion, "start_conversion callback not stored correctly");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(feedback_read_hc_connected, feedback_handler.read_hc_connected, "read_hc_connected callback not stored correctly");
}

void test_feedback_api_init_clears_previous_state(void) {
    feedback_handler.digital = 0xFFFFU;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_IMD_OK] = 3.0F;
    feedback_handler.status[FEEDBACK_ID_IMD_OK] = FEEDBACK_STATUS_HIGH;

    const enum FeedbackReturnCode result = feedback_api_init(feedback_read_digital_all, feedback_start_analog_conversion, feedback_read_hc_connected);

    TEST_ASSERT_EQUAL(FEEDBACK_RC_OK, result);
    TEST_ASSERT_EQUAL_UINT32(0U, feedback_handler.digital);
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0F, feedback_handler.analog[FEEDBACK_ANALOG_INDEX_IMD_OK]);
    TEST_ASSERT_EQUAL(FEEDBACK_STATUS_LOW, feedback_handler.status[FEEDBACK_ID_IMD_OK]);
}

/*! @} */

/*!
 * \defgroup            callbacks Test digital read and analog conversion wrappers
 * @{
 */

void test_feedback_api_update_digital_feedback_all_stores_callback_value(void) {
    feedback_read_digital_all_fake.return_val = 0x1234U;

    const enum FeedbackReturnCode rc = feedback_api_update_digital_feedback_all();

    TEST_ASSERT_EQUAL(FEEDBACK_RC_OK, rc);
    TEST_ASSERT_EQUAL_UINT32(1U, feedback_read_digital_all_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(0x1234U, feedback_handler.digital);
}

void test_feedback_api_start_analog_conversion_all_calls_callback(void) {
    const enum FeedbackReturnCode rc = feedback_api_start_analog_conversion_all();

    TEST_ASSERT_EQUAL(FEEDBACK_RC_OK, rc);
    TEST_ASSERT_EQUAL_UINT32(1U, feedback_start_analog_conversion_fake.call_count);
}

/*! @} */

/*!
 * \defgroup            update_analog Test update analog feedback
 * @{
 */

void test_feedback_api_update_analog_feedback_stores_value(void) {
    const enum FeedbackReturnCode result = feedback_api_update_analog_feedback(FEEDBACK_ANALOG_INDEX_IMD_OK, 2.5F);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_OK, result, "Expected OK return code from analog update");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 2.5F, feedback_handler.analog[FEEDBACK_ANALOG_INDEX_IMD_OK], "Analog value for IMD_OK not stored correctly");
}

void test_feedback_api_update_analog_feedback_returns_invalid_index_when_out_of_range(void) {
    const enum FeedbackReturnCode result = feedback_api_update_analog_feedback((enum FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT, 1.0F);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_INVALID_INDEX, result, "Expected INVALID_INDEX for out-of-range analog index");
}

/*! @} */

/*!
 * \defgroup            getters Test get digital, get analog
 * @{
 */

void test_feedback_api_get_digital_returns_bit_value(void) {
    feedback_handler.digital = EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_LATCH_RESET);

    TEST_ASSERT_TRUE(feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_LATCH_RESET));
    TEST_ASSERT_FALSE(feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE));
}

void test_feedback_api_get_digital_returns_false_when_out_of_range(void) {
    feedback_handler.digital = 0xFFFFFFFFU;

    TEST_ASSERT_FALSE(feedback_api_get_digital((enum FeedbackDigitalBit)FEEDBACK_DIGITAL_BIT_COUNT));
}

void test_feedback_api_get_analog_returns_stored_value(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_V5_MCU] = 1.5F;

    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 1.5F, feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_V5_MCU));
}

void test_feedback_api_get_analog_returns_zero_when_out_of_range(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0F, feedback_api_get_analog((enum FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT));
}

/*! @} */

/*!
 * \defgroup            update_status Test update status feedback
 * @{
 */

void test_feedback_api_update_status_sets_high_for_set_digital_bits(void) {
    feedback_handler.digital = EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE);

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE], "feedback status should be HIGH");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

void test_feedback_api_update_status_sets_low_for_cleared_digital_bits(void) {
    feedback_handler.digital = 0U;

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_LOW, feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE], "feedback status should be LOW");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

void test_feedback_api_update_status_maps_every_digital_bit_to_its_feedback(void) {
    feedback_handler.digital = 0xFFFFFFFFU;

    EAGLETRT_API_UNUSED(feedback_api_update_status());

    for (enum FeedbackDigitalBit bit = 0U; bit < FEEDBACK_DIGITAL_BIT_COUNT; ++bit) {
        TEST_ASSERT_EQUAL(FEEDBACK_STATUS_HIGH, feedback_handler.status[prv_feedback_get_id_from_digital_bit(bit)]);
    }
}

void test_feedback_api_update_status_sets_high_for_analog_above_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.2F;

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_handler.status[FEEDBACK_ID_TSAL_GREEN], "feedback status should be HIGH");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

void test_feedback_api_update_status_sets_low_for_analog_below_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = FEEDBACK_THRESHOLD_LOW_V - 0.2F;

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_LOW, feedback_handler.status[FEEDBACK_ID_SD_OUT], "feedback status should be LOW");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

void test_feedback_api_update_status_sets_error_for_mid_range_analog(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_IN] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5F;

    EAGLETRT_API_UNUSED(feedback_api_update_status());

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_ERROR, feedback_handler.status[FEEDBACK_ID_SD_IN], "feedback status should be ERROR");
}

void test_feedback_api_update_status_sets_high_for_probing_3v3_in_range(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = 1.6F;

    EAGLETRT_API_UNUSED(feedback_api_update_status());

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_handler.status[FEEDBACK_ID_PROBING_3V3], "feedback status should be HIGH");
}

void test_feedback_api_update_status_reads_hc_callback_once_per_update(void) {
    EAGLETRT_API_UNUSED(feedback_api_update_status());

    TEST_ASSERT_EQUAL_UINT32(1U, feedback_read_hc_connected_fake.call_count);
}

void test_feedback_api_update_status_uses_hc_threshold_when_hc_connected(void) {
    feedback_read_hc_connected_fake.return_val = true;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = 1.2F;

    EAGLETRT_API_UNUSED(feedback_api_update_status());

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_handler.status[FEEDBACK_ID_TSAL_GREEN], "feedback status should be HIGH with the HC threshold");
}

void test_feedback_api_update_status_uses_default_threshold_when_hc_not_connected(void) {
    feedback_read_hc_connected_fake.return_val = false;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = 1.2F;

    EAGLETRT_API_UNUSED(feedback_api_update_status());

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_ERROR, feedback_handler.status[FEEDBACK_ID_TSAL_GREEN], "feedback status should be ERROR with the default threshold");
}

/*! @} */

/*!
 * \defgroup            get_status Test get status feedback
 * @{
 */

void test_feedback_api_get_status_with_invalid_id(void) {
    const enum FeedbackStatus status = feedback_api_get_status(FEEDBACK_ID_INVALID);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_ERROR, status, "feedback status should be ERROR");
}

void test_feedback_api_get_status_returns_stored_status(void) {
    feedback_handler.status[FEEDBACK_ID_SD_END] = FEEDBACK_STATUS_HIGH;

    TEST_ASSERT_EQUAL_UINT8(FEEDBACK_STATUS_HIGH, feedback_api_get_status(FEEDBACK_ID_SD_END));
}

/*! @} */

/*!
 * \defgroup            check_values Test feedback check values
 * @{
 */

void test_feedback_api_check_values_when_all_states_matches(void) {
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;
    feedback_handler.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;

    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(
        EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_AIRN_OPEN_COM),
        FEEDBACK_ID_SD_IMD_FB);
    constexpr bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    enum FeedbackId out = FEEDBACK_ID_COUNT;

    const bool result = feedback_api_check_values(mask, value, &out);

    TEST_ASSERT_TRUE_MESSAGE(result, "at least one value in the mask do not match");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_INVALID, out, "return feedback id is not invalid");
}

void test_feedback_api_check_values_when_any_state_dont_match(void) {
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;
    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_AIRN_OPEN_COM);
    constexpr bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_AIRN_OPEN_COM);
    enum FeedbackId out = FEEDBACK_ID_INVALID;

    const bool result = feedback_api_check_values(mask, value, &out);

    TEST_ASSERT_FALSE_MESSAGE(result, "all values in mask matches");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_AIRN_OPEN_COM, out, "unexpected feedback id");
}

void test_feedback_api_check_values_skips_feedbacks_not_in_mask(void) {
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    feedback_handler.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;
    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    constexpr bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    enum FeedbackId out = FEEDBACK_ID_INVALID;

    const bool result = feedback_api_check_values(mask, value, &out);

    TEST_ASSERT_TRUE_MESSAGE(result, "all values in mask matches");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_INVALID, out, "return feedback id is not invalid");
}

void test_feedback_api_check_values_when_out_pointer_is_null(void) {
    feedback_handler.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_LOW;
    const bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    const bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);

    const bool result = feedback_api_check_values(mask, value, NULL);

    TEST_ASSERT_FALSE_MESSAGE(result, "all values in mask matches");
}

void test_feedback_api_check_values_when_out_pointer_is_null_and_all_match(void) {
    feedback_handler.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;
    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);

    const bool result = feedback_api_check_values(mask, mask, NULL);

    TEST_ASSERT_TRUE_MESSAGE(result, "all values in mask match");
}

void test_feedback_api_check_values_with_empty_mask_returns_true(void) {
    enum FeedbackId out = FEEDBACK_ID_COUNT;

    const bool result = feedback_api_check_values(0U, 0U, &out);

    TEST_ASSERT_TRUE_MESSAGE(result, "an empty mask should always match");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_INVALID, out, "return feedback id is not invalid");
}

void test_feedback_api_check_values_fails_on_error_status_regardless_of_expected_value(void) {
    feedback_handler.status[FEEDBACK_ID_TSAL_GREEN] = FEEDBACK_STATUS_ERROR;
    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_TSAL_GREEN);
    enum FeedbackId out = FEEDBACK_ID_INVALID;

    TEST_ASSERT_FALSE_MESSAGE(feedback_api_check_values(mask, mask, &out), "ERROR status must not match an expected HIGH");
    TEST_ASSERT_EQUAL_UINT8(FEEDBACK_ID_TSAL_GREEN, out);

    out = FEEDBACK_ID_INVALID;
    TEST_ASSERT_FALSE_MESSAGE(feedback_api_check_values(mask, 0U, &out), "ERROR status must not match an expected LOW");
    TEST_ASSERT_EQUAL_UINT8(FEEDBACK_ID_TSAL_GREEN, out);
}

void test_feedback_api_check_values_reports_first_mismatching_feedback(void) {
    feedback_handler.status[FEEDBACK_ID_AIRP_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    feedback_handler.status[FEEDBACK_ID_SD_END] = FEEDBACK_STATUS_HIGH;
    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(
        EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_AIRP_OPEN_COM),
        FEEDBACK_ID_SD_END);
    enum FeedbackId out = FEEDBACK_ID_INVALID;

    const bool result = feedback_api_check_values(mask, 0U, &out);

    TEST_ASSERT_FALSE_MESSAGE(result, "both feedbacks are HIGH but LOW is expected");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_AIRP_OPEN_COM, out, "the lowest mismatching id should be reported");
}

/*! @} */

/*!
 * \defgroup             is_digital Test feedback is digital
 * @{
 */

void test_feedback_api_is_digital_with_invalid_id(void) {
    const bool is_digital = feedback_api_is_digital(FEEDBACK_ID_INVALID);

    TEST_ASSERT_FALSE(is_digital);
}

void test_feedback_api_is_digital_true_for_digital_and_false_for_analog(void) {
    TEST_ASSERT_TRUE(feedback_api_is_digital(FEEDBACK_ID_AIRN_OPEN_COM));
    TEST_ASSERT_TRUE(feedback_api_is_digital(FEEDBACK_ID_EXT_FAULT_LATCHED));
    TEST_ASSERT_FALSE(feedback_api_is_digital(FEEDBACK_ID_AIRN_OPEN_MEC));
    TEST_ASSERT_FALSE(feedback_api_is_digital(FEEDBACK_ID_V5_MCU));
}

void test_feedback_every_id_is_either_digital_or_analog_with_a_valid_mapping(void) {
    for (enum FeedbackId id = 0U; id < FEEDBACK_ID_COUNT; ++id) {
        if (feedback_api_is_digital(id)) {
            TEST_ASSERT_NOT_EQUAL_UINT8(FEEDBACK_DIGITAL_BIT_INVALID, feedback_api_get_digital_bit_from_id(id));
        } else {
            TEST_ASSERT_NOT_EQUAL_UINT8(FEEDBACK_ANALOG_INDEX_INVALID, feedback_api_get_analog_index_from_id(id));
        }
    }
}

/*! @} */

/*!
 * \defgroup             get_digital Test feedback get digital
 * @{
 */

void test_feedback_api_get_digital_bit_from_id_with_invalid_id(void) {
    const enum FeedbackDigitalBit result = feedback_api_get_digital_bit_from_id(FEEDBACK_ID_INVALID);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_DIGITAL_BIT_INVALID, result, "the feedback digital bit is not invalid");
}

void test_feedback_api_get_digital_bit_from_id_with_analog_id_returns_invalid(void) {
    const enum FeedbackDigitalBit result = feedback_api_get_digital_bit_from_id(FEEDBACK_ID_IMD_OK);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_DIGITAL_BIT_INVALID, result, "an analog feedback has no digital bit");
}

/*! @} */

/*!
 * \defgroup             get_analog Test feedback get analog
 * @{
 */

void test_feedback_api_get_analog_index_from_id_with_invalid_id(void) {
    const enum FeedbackAnalogIndex result = feedback_api_get_analog_index_from_id(FEEDBACK_ID_INVALID);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ANALOG_INDEX_INVALID, result, "the feedback analog index is not invalid");
}

void test_feedback_api_get_analog_index_from_id_with_digital_id_returns_invalid(void) {
    const enum FeedbackAnalogIndex result = feedback_api_get_analog_index_from_id(FEEDBACK_ID_AIRN_OPEN_COM);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ANALOG_INDEX_INVALID, result, "a digital feedback has no analog index");
}

/*! @} */

/*!
 * \defgroup             get_feedback_payload Test feedback get feedback payload
 * @{
 */

void test_feedback_api_get_feedback_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;

    const union CanPrimaryMessages *payload = feedback_api_get_feedaback_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.libcan_message_feedback, payload, "returned pointer do not match with the internal feedback message");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(can_primary_byte_size_tsacmainboardfeedback, (uint32_t)byte_size, "Feedback payload byte size mismatch");
}

void test_feedback_api_get_feedback_payload_with_null_pointer(void) {
    const union CanPrimaryMessages *payload = feedback_api_get_feedaback_payload(NULL);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.libcan_message_feedback, payload, "returned pointer do not match with the internal feedback message");
}

void test_feedback_api_get_feedback_payload_fills_digital_and_analog_fields(void) {
    feedback_handler.digital = EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = 1.23F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = 2.0F;

    const union CanPrimaryMessages *payload = feedback_api_get_feedaback_payload(NULL);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1U, payload->tsacmainboardfeedback.airnopencom, "airnopencom should be set");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, payload->tsacmainboardfeedback.airpopencom, "airpopencom should be cleared");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, FEEDBACK_VOLTAGE_TO_SD_VOLT(1.23F), payload->tsacmainboardfeedback.airnopenmec, "airnopenmec value mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, FEEDBACK_VOLTAGE_TO_SD_VOLT(2.0F), payload->tsacmainboardfeedback.tsalgreen, "tsalgreen value mismatch");
}

/*! @} */

/*!
 * \defgroup             get_shutdown_payload Test feedback get shutdown payload
 * @{
 */

void test_feedback_api_get_shutdown_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;

    const union CanPrimaryMessages *payload = feedback_api_get_shutdown_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.libcan_message_shutdown, payload, "returned pointer do not match with the internal shutdown message");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(can_primary_byte_size_tsacmainboardshutdown, (uint32_t)byte_size, "Shutdown payload byte size mismatch");
}

void test_feedback_api_get_shutdown_payload_fills_voltages(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = 1.0F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_BMS_FB] = 2.0F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_IMD_FB] = 3.0F;

    const union CanPrimaryMessages *payload = feedback_api_get_shutdown_payload(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, FEEDBACK_VOLTAGE_TO_SD_VOLT(1.0F), payload->tsacmainboardshutdown.interlocktsacvoltage, "interlock voltage mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, FEEDBACK_VOLTAGE_TO_SD_VOLT(2.0F), payload->tsacmainboardshutdown.amsvoltage, "ams voltage mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, FEEDBACK_VOLTAGE_TO_SD_VOLT(3.0F), payload->tsacmainboardshutdown.imdvoltage, "imd voltage mismatch");
}

void test_feedback_api_get_shutdown_payload_precharge_voltage_follows_digital_bit(void) {
    feedback_handler.digital = EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC);
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, FEEDBACK_SD_VREF, feedback_api_get_shutdown_payload(NULL)->tsacmainboardshutdown.prechargevoltage);

    feedback_handler.digital = 0U;
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0F, feedback_api_get_shutdown_payload(NULL)->tsacmainboardshutdown.prechargevoltage);
}

/*! @} */

/*!
 * \defgroup             get_feedback_shutdown_payload Test feedback get feedback shutdown payload
 * @{
 */

void test_feedback_api_get_feedback_shutdown_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;

    const union CanPrimaryMessages *payload = feedback_api_get_feedback_shutdown_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.libcan_message_feedback_shutdown, payload, "returned pointer do not match with the internal feedback shutdown message");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(can_primary_byte_size_tsacmainboardfeedbackshutdown, (uint32_t)byte_size, "Feedback shutdown payload byte size mismatch");
}

void test_feedback_api_get_feedback_shutdown_payload_fills_voltages(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_IN] = 2.0F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_END] = 3.0F;

    const union CanPrimaryMessages *payload = feedback_api_get_feedback_shutdown_payload(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, FEEDBACK_VOLTAGE_TO_SD_VOLT(2.0F), payload->tsacmainboardfeedbackshutdown.sdin, "sdin value mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, FEEDBACK_VOLTAGE_TO_SD_VOLT(3.0F), payload->tsacmainboardfeedbackshutdown.sdend, "sdend value mismatch");
}

/*! @} */

void setUp(void) {
    RESET_FAKE(feedback_read_digital_all);
    RESET_FAKE(feedback_start_analog_conversion);
    RESET_FAKE(feedback_read_hc_connected);
    EAGLETRT_API_UNUSED(feedback_api_init(feedback_read_digital_all, feedback_start_analog_conversion, feedback_read_hc_connected));
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    /*!
     * \addgroup            id_from_digital_bit Run feedback ID from digital bit index tests
     * @{
     */
    RUN_TEST(test_feedback_get_id_from_digital_bit_returns_correct_id);
    RUN_TEST(test_feedback_get_id_from_digital_bit_returns_invalid_for_unknown_bit);
    RUN_TEST(test_feedback_digital_bit_and_id_mappings_are_inverse_of_each_other);
    /*! @} */

    /*!
     * \addgroup            id_from_analog_index Run feedback ID from analog index tests
     * @{
     */
    RUN_TEST(test_feedback_get_id_from_analog_index_returns_correct_id);
    RUN_TEST(test_feedback_get_id_from_analog_index_returns_invalid_for_unknown_index);
    RUN_TEST(test_feedback_analog_index_and_id_mappings_are_inverse_of_each_other);
    /*! @} */

    /*!
     * \addgroup            get_analog_status Run get analog status tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_analog_status_probing_3v3_returns_high_when_in_range);
    RUN_TEST(test_feedback_api_get_analog_status_probing_3v3_returns_error_when_above_high_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_probing_3v3_returns_error_when_below_low_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_probing_3v3_ignores_given_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_returns_low_when_below_low_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_returns_high_when_above_high_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_returns_error_when_between_thresholds);
    RUN_TEST(test_feedback_api_get_analog_status_value_equal_to_high_threshold_is_high);
    RUN_TEST(test_feedback_api_get_analog_status_value_equal_to_low_threshold_is_low);
    RUN_TEST(test_feedback_api_get_analog_status_uses_given_high_threshold);
    /*! @} */

    /*!
     * \addgroup            init Run initialization tests
     * @{
     */
    RUN_TEST(test_feedback_api_init_returns_null_pointer_when_read_callback_is_null);
    RUN_TEST(test_feedback_api_init_returns_null_pointer_when_start_conversion_callback_is_null);
    RUN_TEST(test_feedback_api_init_returns_null_pointer_when_hc_callback_is_null);
    RUN_TEST(test_feedback_api_init_saves_callbacks_and_returns_ok);
    RUN_TEST(test_feedback_api_init_clears_previous_state);
    /*! @} */

    /*!
     * \addgroup            callbacks Run digital read and analog conversion wrapper tests
     * @{
     */
    RUN_TEST(test_feedback_api_update_digital_feedback_all_stores_callback_value);
    RUN_TEST(test_feedback_api_start_analog_conversion_all_calls_callback);
    /*! @} */

    /*!
     * \addgroup            update_analog Run update analog feedback tests
     * @{
     */
    RUN_TEST(test_feedback_api_update_analog_feedback_stores_value);
    RUN_TEST(test_feedback_api_update_analog_feedback_returns_invalid_index_when_out_of_range);
    /*! @} */

    /*!
     * \addgroup            getters Run get digital and get analog tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_digital_returns_bit_value);
    RUN_TEST(test_feedback_api_get_digital_returns_false_when_out_of_range);
    RUN_TEST(test_feedback_api_get_analog_returns_stored_value);
    RUN_TEST(test_feedback_api_get_analog_returns_zero_when_out_of_range);
    /*! @} */

    /*!
     * \addgroup            update_status Run update status feedback tests
     * @{
     */
    RUN_TEST(test_feedback_api_update_status_sets_high_for_set_digital_bits);
    RUN_TEST(test_feedback_api_update_status_sets_low_for_cleared_digital_bits);
    RUN_TEST(test_feedback_api_update_status_maps_every_digital_bit_to_its_feedback);
    RUN_TEST(test_feedback_api_update_status_sets_high_for_analog_above_threshold);
    RUN_TEST(test_feedback_api_update_status_sets_low_for_analog_below_threshold);
    RUN_TEST(test_feedback_api_update_status_sets_error_for_mid_range_analog);
    RUN_TEST(test_feedback_api_update_status_sets_high_for_probing_3v3_in_range);
    RUN_TEST(test_feedback_api_update_status_reads_hc_callback_once_per_update);
    RUN_TEST(test_feedback_api_update_status_uses_hc_threshold_when_hc_connected);
    RUN_TEST(test_feedback_api_update_status_uses_default_threshold_when_hc_not_connected);
    /*! @} */

    /*!
     * \addgroup            get_status Run get status feedback tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_status_with_invalid_id);
    RUN_TEST(test_feedback_api_get_status_returns_stored_status);
    /*! @} */

    /*!
     * \addgroup            check_values Run feedback check values tests
     * @{
     */
    RUN_TEST(test_feedback_api_check_values_when_all_states_matches);
    RUN_TEST(test_feedback_api_check_values_when_any_state_dont_match);
    RUN_TEST(test_feedback_api_check_values_skips_feedbacks_not_in_mask);
    RUN_TEST(test_feedback_api_check_values_when_out_pointer_is_null);
    RUN_TEST(test_feedback_api_check_values_when_out_pointer_is_null_and_all_match);
    RUN_TEST(test_feedback_api_check_values_with_empty_mask_returns_true);
    RUN_TEST(test_feedback_api_check_values_fails_on_error_status_regardless_of_expected_value);
    RUN_TEST(test_feedback_api_check_values_reports_first_mismatching_feedback);
    /*! @} */

    /*!
     * \addgroup             is_digital Run feedback is digital tests
     * @{
     */
    RUN_TEST(test_feedback_api_is_digital_with_invalid_id);
    RUN_TEST(test_feedback_api_is_digital_true_for_digital_and_false_for_analog);
    RUN_TEST(test_feedback_every_id_is_either_digital_or_analog_with_a_valid_mapping);
    /*! @} */

    /*!
     * \addgroup             get_digital Run feedback get digital tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_digital_bit_from_id_with_invalid_id);
    RUN_TEST(test_feedback_api_get_digital_bit_from_id_with_analog_id_returns_invalid);
    /*! @} */

    /*!
     * \addgroup             get_analog Run feedback get analog tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_analog_index_from_id_with_invalid_id);
    RUN_TEST(test_feedback_api_get_analog_index_from_id_with_digital_id_returns_invalid);
    /*! @} */

    /*!
     * \addgroup             get_feedback_payload Run feedback get feedback payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_feedback_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_api_get_feedback_payload_with_null_pointer);
    RUN_TEST(test_feedback_api_get_feedback_payload_fills_digital_and_analog_fields);
    /*! @} */

    /*!
     * \addgroup             get_shutdown_payload Run feedback get shutdown payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_shutdown_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_api_get_shutdown_payload_fills_voltages);
    RUN_TEST(test_feedback_api_get_shutdown_payload_precharge_voltage_follows_digital_bit);
    /*! @} */

    /*!
     * \addgroup             get_feedback_shutdown_payload Run feedback get feedback shutdown payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_feedback_shutdown_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_api_get_feedback_shutdown_payload_fills_voltages);
    /*! @} */

    return UNITY_END();
}
