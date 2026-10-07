/*!
 * \file post-api.h
 * \date 2024-04-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Power-On Self Test and initialization functions needed to make sure
 *        that the system is working as expected
 */

#ifndef POST_API_H
#define POST_API_H

#include "post.h"
#include "mainboard-conf.h"

#ifdef CONF_POST_MODULE_ENABLE

/*!
 * \brief Run the power-on self test
 *
 * \details This function check if all the systems and peripherals work
 * as execpected, otherwise it returns an error code
 *
 * \param data The data needed by the POST module for initialization
 *
 * \retval POST_RC_NULL_POINTER if the given function pointers are NULL
 * \retval POST_RC_UNINITIALIZED if any of the modules cannot be initialized correctly
 * \retval POST_RC_SETUP_ERROR a error occured during the modules setup
 * \retval POST_RC_OK otherwise
 */
enum PostReturnCode post_run(struct PostInitData data);

#else // CONF_POST_MODULE_ENABLE

#define post_run(data) (POST_RC_OK)

#endif // CONF_POST_MODULE_ENABLE

#endif // POST_API_H
