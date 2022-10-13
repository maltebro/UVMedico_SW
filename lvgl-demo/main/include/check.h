/*!
 * @file check.h
 * @brief This file contains API for hall sensor.
 */

#ifndef CHECK_H
#define CHECK_H

#include "top.h"

#define Total_Missing_Module   0

void dev_check_task(void *pvParameter);

void state_task(void *pvParameters);

#endif // CHECK_H