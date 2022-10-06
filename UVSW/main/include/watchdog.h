/*!
 * @file watchdog.h
 * @brief This file contains API for TIMER.
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "driver/timer.h"
#include "ethernet.h"

#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
//#define TIMER_SCALE           ((TIMER_BASE_CLK/400) / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_SCALE           ((TIMER_BASE_CLK) / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_SCALE_MS           (((TIMER_BASE_CLK) / TIMER_DIVIDER)/1000 )  // convert counter value to seconds

/*
 * Macro to check the outputs of TWDT functions and trigger an abort if an
 * incorrect code is returned.
 */
#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("TWDT ERROR\n");                                \
                abort();                                               \
            }                                                          \
})

typedef struct {
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
} example_timer_info_t;

/**
 * @brief A sample structure to pass events from the timer ISR to task
 *
 */
typedef struct {
    example_timer_info_t info;
    uint64_t timer_counter_value;
} example_timer_event_t;


void hw_watchdog_init(void);

void sw_watchdog_init(void);

void sw_watchdog_deinit(void);

void watchdog_task(void *pvParameter); //NOT USED!!!! BACKUP CODE ONLY

bool IRAM_ATTR timer_group_isr_callback(void *args);

/**
 * @brief Initialize selected timer of timer group
 *
 * @param group Timer Group number, index from 0
 * @param timer timer ID, index from 0
 * @param auto_reload whether auto-reload on alarm event
 * @param timer_interval_sec interval of alarm
 */
void example_tg_timer_init(int group, int timer, bool auto_reload, int timer_interval_sec);

void print_timer_counter(uint64_t counter_value);

void watchdog_init(void);
#endif // WATCHDOG_H