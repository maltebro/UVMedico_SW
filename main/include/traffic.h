/*!
 * @file traffic.h
 * @brief This file contains API for traffic light.
 */

#ifndef TRAFFIC_H
#define TRAFFIC_H

#include "top.h"

void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

void traffic_init();

void traffic_task(void *pvParameter);

#endif // TRAFFIC_H