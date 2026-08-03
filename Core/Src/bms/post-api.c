/*!
 * \file post.c
 * \date 2024-04-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Power-On Self Test and initialization functions implementation needed
 *        to make sure that the system is working as expected
 */

#include "post-api.h"

#include "eagletrt-api.h"
#include "error-api.h"
#include "identity-api.h"
#include "post.h"
#include "programmer-api.h"
#include "timebase.h"
#include "volt-api.h"
#include "current-api.h"
#include "internal-voltage-api.h"
#include "bal-api.h"
#include "pcu-api.h"
#include "can-comm-api.h"
#include "led-api.h"
#include "imd-api.h"
#include "feedback-api.h"
#include "display-api.h"

#ifdef CONF_POST_MODULE_ENABLE

/*!
 * \brief Initialize all the cellboard modules
 *
 * \attention The order in which the init functions are called matters
 *
 * \param data A pointer to the initialization data
 *
 * \retval POST_RC_UNINITIALIZED if any module failed initialization
 * \retval POST_RC_OK otherwise
 */
enum PostReturnCode prv_post_modules_init(const struct PostInitData *const data) {
    /*
     * The error and identity initialization functions have to be executed
     * before every other function to ensure the proper functionality
     */
    if (error_api_init() != ERROR_RC_OK) {
        return POST_RC_UNINITIALIZED;
    }
    identity_api_init();

    /*!
     * Some of the function return values can be ignored because they are either
     * always OK or some assertion can be made (like for the NULL checks)
     */
    enum PostReturnCode result = POST_RC_OK;
    if (timebase_init(1U) != TIMEBASE_RC_OK) {
        result = POST_RC_UNINITIALIZED;
    }
    EAGLETRT_API_UNUSED(pcu_api_init(data->pcu_set, data->pcu_toggle));
    EAGLETRT_API_UNUSED(volt_api_init());
    EAGLETRT_API_UNUSED(current_api_init());
    EAGLETRT_API_UNUSED(can_comm_init(data->can_send));
    EAGLETRT_API_UNUSED(programmer_api_init(data->system_reset));
    EAGLETRT_API_UNUSED(led_api_init(data->led_set, data->led_toggle));
    EAGLETRT_API_UNUSED(imd_api_init(data->imd_start));
    EAGLETRT_API_UNUSED(feedback_api_init(data->feedback_read_all, data->feedback_start_conversion));
    EAGLETRT_API_UNUSED(display_api_init(data->display_set, data->display_toggle));
    EAGLETRT_API_UNUSED(internal_voltage_api_init(data->spi_send, data->spi_send_receive));
    EAGLETRT_API_UNUSED(bal_api_init());
    return result;
}

enum PostReturnCode prv_post_module_setup(void) {
    pcu_api_reset_all();
    timebase_set_enable(true);
    can_comm_enable_all();

    // Wait for the current sensor to start its normal operation cycle
    milliseconds_t time = timebase_get_time();
    while (timebase_get_time() - time <= CURRENT_SENSOR_STARTUP_TIME_MS) {
    }
    if (current_api_start_sensor_communication_watchdog() != WATCHDOG_RC_OK) {
        return POST_RC_SETUP_ERROR;
    }

    return POST_RC_OK;
}

enum PostReturnCode post_run(const struct PostInitData data) {
    if (data.system_reset == NULL ||
        data.can_send == NULL ||
        data.led_set == NULL ||
        data.led_toggle == NULL ||
        data.imd_start == NULL ||
        data.pcu_set == NULL ||
        data.pcu_toggle == NULL ||
        data.feedback_read_all == NULL ||
        data.feedback_start_conversion == NULL ||
        data.display_set == NULL ||
        data.display_toggle == NULL ||
        data.spi_send == NULL ||
        data.spi_send_receive == NULL) {
        return POST_RC_NULL_POINTER;
    }

    // Module initialization
    enum PostReturnCode post_code = prv_post_modules_init(&data);
    if (post_code != POST_RC_OK) {
        return post_code;
    }

    // Module confiuration
    post_code = prv_post_module_setup();

    // TODO: Test that every peripheral is working

    return post_code;
}

#ifdef CONF_POST_STRINGS_ENABLE

EAGLETRT_STATIC char *post_module_name = "post";

EAGLETRT_STATIC char *post_return_code_name[] = {
    [POST_RC_OK] = "ok",
    [POST_RC_UNINITIALIZED] = "uninitialized",
    [POST_RC_SETUP_ERROR] = "setup error",
    [POST_RC_NULL_POINTER] = "null pointer"
};

EAGLETRT_STATIC char *post_return_code_description[] = {
    [POST_RC_OK] = "executed successfully",
    [POST_RC_UNINITIALIZED] = "a module has not been initialized correctly",
    [POST_RC_SETUP_ERROR] = "a module has not been configured correctly",
    [POST_RC_NULL_POINTER] = "attempt to dereference a null pointer"
};

#endif // CONF_POST_STRINGS_ENABLE

#endif // CONF_POST_MODULE_ENABLE
