#include "traffic.h"

static const char *TAG = "traffic";

void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i)
    {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}

void traffic_init()
{
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(BUZZER, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(3, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip)
    {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
}

void traffic_task(void *pvParameter)
{

    ESP_LOGI(TAG, "Traffic task started");
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint16_t hue = 0;
    uint16_t pos = 0;
    uint16_t color = Disable;
    uint16_t color_now = Disable;
    LCD_Page = Display_Doors;
    traffic_init();
    while (1)
    {
        if (LCD_Page == Display_Hold)
            color = Red;
        else if (LCD_Page == Display_Error)
            color = Yellow;
        else
            color = Green;
        // printf("LCD_Page: %d, color: %d, color_now: %d\n", LCD_Page, color, color_now);
        if (color_now != color)
        {
            switch (color)
            {
            case Red:
                pos = 2; // top red
                hue = 0;
                strip->clear(strip, 50);
                vTaskDelay(pdMS_TO_TICKS(100));
                led_strip_hsv2rgb(hue, 100, 100, &red, &green, &blue);
                // Write RGB values to strip driver
                ESP_ERROR_CHECK(strip->set_pixel(strip, pos, red, green, blue));
                // Flush RGB values to LEDs
                ESP_ERROR_CHECK(strip->refresh(strip, 100));
                color_now = Red;
                break;
            case Yellow:
                pos = 1; // top red
                hue = 60;
                strip->clear(strip, 50);
                vTaskDelay(pdMS_TO_TICKS(100));
                led_strip_hsv2rgb(hue, 100, 100, &red, &green, &blue);
                // Write RGB values to strip driver
                ESP_ERROR_CHECK(strip->set_pixel(strip, pos, red, green, blue));
                // Flush RGB values to LEDs
                ESP_ERROR_CHECK(strip->refresh(strip, 100));
                color_now = Yellow;
                break;
            case Green:
                pos = 0; // top red
                hue = 120;
                strip->clear(strip, 50);
                vTaskDelay(pdMS_TO_TICKS(100));
                led_strip_hsv2rgb(hue, 100, 100, &red, &green, &blue);
                // Write RGB values to strip driver
                ESP_ERROR_CHECK(strip->set_pixel(strip, pos, red, green, blue));
                // Flush RGB values to LEDs
                ESP_ERROR_CHECK(strip->refresh(strip, 100));
                color_now = Green;
                break;
            default:
                break;
            }
        }
        if(watchdog_running == true)CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
        vTaskDelay(SENSOR_PERIOD / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}