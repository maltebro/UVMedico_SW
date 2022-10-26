#include "hall.h"

static const char *TAG = "hall";

void hall_task(void *pvParameter)
{

    adc1_config_width(width);
    adc1_config_channel_atten(channel_5, atten);

    // Characterize ADC
    hall_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, hall_chars);
    // Continuously sample ADC1
    while (1)
    {
        uint32_t adc_reading = 0;
        // Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
            adc_reading += adc1_get_raw((adc1_channel_t)channel_5);
        }
        adc_reading /= NO_OF_SAMPLES;
        // Convert adc_reading to voltage in mV
        hall_sensor_voltage = esp_adc_cal_raw_to_voltage(adc_reading, hall_chars);
        // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);

        // temp = 0.00000000761364735539*voltage*voltage*voltage - 0.0000341550534547655*voltage*voltage + 0.0791622442559253*voltage - 28.710203155237;
        if(PRINTDATA==1)
        ESP_LOGI(TAG, "voltage: %d mV", hall_sensor_voltage);
        if(watchdog_running == true)CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
        hall_task_cnt ++;
        vTaskDelay(SENSOR_PERIOD / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
