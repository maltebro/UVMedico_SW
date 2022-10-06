#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "watchdog.h"
#include "driver/gpio.h"
#include "top.h"



void hw_watchdog_init(void)
{
    ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_LOW_SPEED_MODE,
            .timer_num        = LEDC_TIMER_0,
            .duty_resolution  = LEDC_TIMER_13_BIT,
            .freq_hz          = 300,
            .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = 32,
        .duty           = 4095,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void sw_watchdog_init(void)
{
    //Initialize or reinitialize TWDT
    CHECK_ERROR_CODE(esp_task_wdt_init(TWDT_TIMEOUT_S, true), ESP_OK);
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(0));
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(1));

    CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[send_thread_handle]), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[send_thread_handle]), ESP_OK);

    if(module_info[Module_Type].HasTempSensor == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[temp_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[temp_task_handle]), ESP_OK);
    }
    if(module_info[Module_Type].HasLightSource == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[light_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[light_task_handle]), ESP_OK);
    }
    if(module_info[Module_Type].HasLidar == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[lidar_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[lidar_task_handle]), ESP_OK);
    }
    if(module_info[Module_Type].HasTrafficLight == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[traffic_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[traffic_task_handle]), ESP_OK);
    }
    if(module_info[Module_Type].HasHallSensor == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[hall_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[hall_task_handle]), ESP_OK);
    }
    /*
    CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[send_thread_handle]), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[send_thread_handle]), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[temp_task_handle]), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[temp_task_handle]), ESP_OK);

    if (Module_Type != Display_Module && Module_Type != Check_Module)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[light_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[light_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[lidar_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[lidar_task_handle]), ESP_OK);
    }

    if (Module_Type == Traffic_Module)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[traffic_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[traffic_task_handle]), ESP_OK);
    }

    if (Module_Type == Hall1_Module || Module_Type == Hall2_Module)
    {
        CHECK_ERROR_CODE(esp_task_wdt_add(task_handles[hall_task_handle]), ESP_OK);
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[hall_task_handle]), ESP_OK);
    }
    */
    watchdog_running = true;
}

void sw_watchdog_deinit(void)
{
    watchdog_running = false;
    CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[send_thread_handle]), ESP_OK);     //Unsubscribe task from TWDT
    CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[send_thread_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    if(module_info[Module_Type].HasTempSensor == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[temp_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[temp_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }
    if(module_info[Module_Type].HasLightSource == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[light_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[light_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }
    if(module_info[Module_Type].HasLidar == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[lidar_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[lidar_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }
    if(module_info[Module_Type].HasTrafficLight == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[traffic_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[traffic_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }
    if(module_info[Module_Type].HasHallSensor == true)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[hall_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[hall_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }

    /*
    CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[send_thread_handle]), ESP_OK);     //Unsubscribe task from TWDT
    CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[send_thread_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[temp_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
    CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[temp_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    
    if (Module_Type != Display_Module && Module_Type != Check_Module)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[light_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[light_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[lidar_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[lidar_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }

    if (Module_Type == Traffic_Module)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[traffic_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[traffic_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }

    if (Module_Type == Hall1_Module || Module_Type == Hall2_Module)
    {
        CHECK_ERROR_CODE(esp_task_wdt_delete(task_handles[hall_task_handle]), ESP_OK);     //Unsubscribe task from TWDT
        CHECK_ERROR_CODE(esp_task_wdt_status(task_handles[hall_task_handle]), ESP_ERR_NOT_FOUND);  //Confirm task is unsubscribed
    }
    */
    CHECK_ERROR_CODE(esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(0)), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(xTaskGetIdleTaskHandleForCPU(0)), ESP_ERR_NOT_FOUND);
    CHECK_ERROR_CODE(esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(1)), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(xTaskGetIdleTaskHandleForCPU(1)), ESP_ERR_NOT_FOUND);

    CHECK_ERROR_CODE(esp_task_wdt_deinit(), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(NULL), ESP_ERR_INVALID_STATE);     //Confirm TWDT has been deinitialized
}

void watchdog_task(void *pvParameter)       //NOT USED!!!! BACKUP CODE ONLY
{
    /*
    uint32_t us1=3000;
    uint32_t us2=3000;
    uint32_t duty_cycle = (us1+us2)/us1;
    ESP_LOGI(TAG, "Watchdog task started. Frequency = %dHz. Duty cycle = %d%% ", 1000000/(us1+us2), 100/duty_cycle);

    while(1)
    {
        gpio_set_level(WATCHDOG, 1);
        esp_rom_delay_us(us1);
        gpio_set_level(WATCHDOG, 0);
        esp_rom_delay_us(us2);
    }
    */
    while (1)
    {
        example_timer_event_t evt;
        xQueueReceive(s_timer_queue, &evt, portMAX_DELAY);
        if (wd_status == 1)
            wd_status = 0;
        else if (wd_status == 0)
            wd_status = 1;
        gpio_set_level(32, wd_status);
        // uint64_t task_counter_value;
        // timer_get_counter_value(evt.info.timer_group, evt.info.timer_idx, &task_counter_value);
    }

    vTaskDelete(NULL);
}

bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    example_timer_info_t *info = (example_timer_info_t *) args;

    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(info->timer_group, info->timer_idx);

    /* Prepare basic event data that will be then sent back to task */
    example_timer_event_t evt = {
        .info.timer_group = info->timer_group,
        .info.timer_idx = info->timer_idx,
        .info.auto_reload = info->auto_reload,
        .info.alarm_interval = info->alarm_interval,
        .timer_counter_value = timer_counter_value
    };

    if (!info->auto_reload) {
        timer_counter_value += info->alarm_interval * TIMER_SCALE;
        timer_group_set_alarm_value_in_isr(info->timer_group, info->timer_idx, timer_counter_value);
    }

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken);
    
    
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}


void example_tg_timer_init(int group, int timer, bool auto_reload, int timer_interval_sec)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    }; // default clock source is APB
    timer_init(group, timer, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(group, timer, 0);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(group, timer, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(group, timer);

    example_timer_info_t *timer_info = calloc(1, sizeof(example_timer_info_t));
    timer_info->timer_group = group;
    timer_info->timer_idx = timer;
    timer_info->auto_reload = auto_reload;
    timer_info->alarm_interval = timer_interval_sec;
    timer_isr_callback_add(group, timer, timer_group_isr_callback, timer_info, 0);

    timer_start(group, timer);
}

void print_timer_counter(uint64_t counter_value)
{
    printf("Counter: 0x%08x%08x\r\n", (uint32_t) (counter_value >> 32),
           (uint32_t) (counter_value));
    printf("Time   : %.8f s\r\n", (double) counter_value / TIMER_SCALE);
}

void watchdog_init(void)
{
    s_timer_queue = xQueueCreate(10, sizeof(example_timer_event_t)); // watchdog interruption (another way to realize hardware watchdog)
    example_tg_timer_init(TIMER_GROUP_0, TIMER_1, false, 1);         // watchdog interruption
}