/*!
 * \file volt.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Voltage measurment and control
 */

#ifndef VOLT_H
#define VOLT_H

#include <stddef.h>

#include "can-primary.h"
#include "mainboard-def.h"

/*! \brief Minimum and maximum allowed cell voltage in V */
#define VOLT_MIN_V (2.8F)
#define VOLT_MAX_V (4.2F)

#define VOLT_NOMINAL_V (3.6F)

/*!
 * \brief Return code for the voltage module functions
 */
enum VoltReturnCode {
    VOLT_RC_OK,           /*!< The function executed successfully  */
    VOLT_RC_NULL_POINTER, /*!< A NULL pointer is given as parameter or used inside the function */
    VOLT_RC_OUT_OF_BOUNDS /*!< An index (or pointer) value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief Type definition for a the matrix of cells voltages in V
 *
 * \details The matrix contains a row for each cellboard and every column contains
 * the i-th voltage of each segment
 */
typedef volt_t cells_voltage[CELLBOARD_COUNT][CELLBOARD_SEGMENT_SERIES_COUNT];

/*!
 * \brief Voltages handler structure
 *
 * \warning This structure should never be used outside of this file
 */
struct VoltHandler {
    cells_voltage voltages; /*!< The array of cells voltages in V */

    union CanPrimaryMessages libcan_message_cellboard1; /*!< The CAN payload for Cellboard 1 cell voltages */
    union CanPrimaryMessages libcan_message_cellboard2; /*!< The CAN payload for Cellboard 2 cell voltages */
    union CanPrimaryMessages libcan_message_cellboard3; /*!< The CAN payload for Cellboard 3 cell voltages */
    union CanPrimaryMessages libcan_message_cellboard4; /*!< The CAN payload for Cellboard 4 cell voltages */
    union CanPrimaryMessages libcan_message_cellboard5; /*!< The CAN payload for Cellboard 5 cell voltages */
    union CanPrimaryMessages libcan_message_cellboard6; /*!< The CAN payload for Cellboard 6 cell voltages */
};

#endif // VOLT_H
