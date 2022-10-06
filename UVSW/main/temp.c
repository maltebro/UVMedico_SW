#include "temp.h"

static const char *TAG = "temp";

void temp_task(void *pvParameter)
{   
    ESP_LOGI(TAG, "temp task started.");
    if (unit == ADC_UNIT_1)
    {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    }
    else
    {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    // Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

    while (1)
    {
        uint32_t adc_reading = 0;
        // Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
            if (unit == ADC_UNIT_1)
            {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            }
            else
            {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        // Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);

        temp = 0.00000000761364735539 * voltage * voltage * voltage - 0.0000341550534547655 * voltage * voltage + 0.0791622442559253 * voltage - 28.710203155237;
        // if(PRINTDATA==1)
        // ESP_LOGI(TAG, "Temprature: %d °C", temp);
        temp_task_cnt ++;
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if(watchdog_running == true)CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
    }

    vTaskDelete(NULL);
}

