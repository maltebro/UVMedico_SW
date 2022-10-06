/*!
 * @file fan.h
 * @brief This file contains API for FAN.
 */

#ifndef FAN_H
#define FAN_H

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "expander.h"
#include "driver/mcpwm.h"
#include "top.h"


/**
 * @brief this is an ISR callback, we take action according to the captured edge
 */
bool fan_tack_isr_handler(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_sig, const cap_event_data_t *edata, void *arg);

void fan_tack_config(void);

void fan_tack_calculation(void);

void fan_init(void);

void fan_task(void *pvParameter);

#endif // FAN_H