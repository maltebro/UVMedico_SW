#include <stdio.h>
#include "esp_log.h"
#include "fan.h"
#include "expander.h"
#include "soc/rtc.h"

static const char *TAG = "fan";
#define FAN_TACK        33
static xQueueHandle cap_queue;
static uint32_t cap_val_begin_of_sample = 0;
static uint32_t cap_val_end_of_sample = 0;


/**
 * @brief this is an ISR callback, we take action according to the captured edge
 */
bool fan_tack_isr_handler(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_sig, const cap_event_data_t *edata,
                                  void *arg) {
    //calculate the interval in the ISR,
    //so that the interval will be always correct even when cap_queue is not handled in time and overflow.
    BaseType_t high_task_wakeup = pdFALSE;
    if (edata->cap_edge == MCPWM_POS_EDGE) {
        // store the timestamp when pos edge is detected
        cap_val_begin_of_sample = edata->cap_value;
        cap_val_end_of_sample = cap_val_begin_of_sample;
    } else {
        cap_val_end_of_sample = edata->cap_value;
        // following formula refers to: https://www.elecrow.com/download/HC_SR04%20Datasheet.pdf
        uint32_t pulse_count = cap_val_end_of_sample - cap_val_begin_of_sample;
        // send measurement back though queue
        xQueueSendFromISR(cap_queue, &pulse_count, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

void fan_tack_config(void)
{
    // the queue where we read data
    cap_queue = xQueueCreate(1, sizeof(uint32_t));
    if (cap_queue == NULL) {
        ESP_LOGE(TAG, "failed to alloc cap_queue");
        return;
    }
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, FAN_TACK));
    ESP_ERROR_CHECK(gpio_pulldown_en(FAN_TACK));
    mcpwm_capture_config_t conf = {
        .cap_edge = MCPWM_BOTH_EDGE,
        .cap_prescale = 1,
        .capture_cb = fan_tack_isr_handler,
        .user_data = NULL
    };
    ESP_ERROR_CHECK(mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &conf));
    ESP_LOGI(TAG, "Capture pin configured");
}

void fan_tack_calculation(void)
{
    uint32_t pulse_count;
    // block and wait for new measurement
    xQueueReceive(cap_queue, &pulse_count, portMAX_DELAY  );
    uint32_t pulse_width_us = pulse_count * (4000000.0 / rtc_clk_apb_freq_get());
    float TS = (float)pulse_width_us/1000000;
    RPM = 60/(TS);
    
}

void fan_init(void)
{
    fan_tack_config();                              //Fan feedback
    mcpwm_start(MCPWM_UNIT_0,MCPWM_TIMER_0);        //Fan feedback
}

void fan_task(void *pvParameter)
{
    ESP_LOGI(TAG, "FAN task started");
    // ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_CLOCK, 0x20, 1 ));  //Use external clock

    while (1)
    {
        fan_tack_calculation();
        if (PRINTDATA == 1)
            ESP_LOGI(TAG, "FAN SPEED:  %d r/min", RPM);
        vTaskDelay(SENSOR_PERIOD / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}