/*!
 * @file sys_init.h
 * @brief This file contains API for ststem initialization functions.
 */

#ifndef SYS_INIT_H
#define SYS_INIT_H

#include "top.h"

void gpio_init(void);

void sys_timer_init(void);

void ethernet_init(void);

void check_uv_sensor(void);

void i2c_init(void);

void nvs_init(void);

void print_module_functions();

#endif // SYS_INIT_H