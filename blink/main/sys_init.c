#include "sys_init.h"

static const char *TAG = "sys_init";

void gpio_init(void)
{
    gpio_reset_pin(LAMP_STATUS_1);
    gpio_set_direction(LAMP_STATUS_1, GPIO_MODE_INPUT);

    gpio_reset_pin(LAMP_STATUS_2);
    gpio_set_direction(LAMP_STATUS_2, GPIO_MODE_INPUT);

    gpio_reset_pin(WATCHDOG);
    gpio_set_direction(WATCHDOG, GPIO_MODE_INPUT_OUTPUT);

    //if (Module_Type == Display_Module)
    if(module_info[Module_Type].HasBuzzer == true)
    {
        gpio_reset_pin(BUZZER);
        gpio_set_direction(BUZZER, GPIO_MODE_INPUT_OUTPUT);
    }
    ESP_LOGI(TAG, "GPIO init successfully.");
}

void sys_timer_init(void)
{
    timer_config_t timer_config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
    };
    timer_init(TIMER_GROUP_0, TIMER_1, &timer_config);      //Lidar task time log 
    timer_init(TIMER_GROUP_1, TIMER_0, &timer_config);      //Module running time log timer
    timer_init(TIMER_GROUP_1, TIMER_1, &timer_config);      //UV light on time log timer
    timer_start(TIMER_GROUP_1, TIMER_0);
    ESP_LOGI(TAG, "Timer init successfully.");
}

void ethernet_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init()); // Ethernet connect
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(eth_connect());
}

void check_uv_sensor(void)
{
    uint8_t read_buffer[2] = {0};
    size_t read_size = sizeof(read_buffer);
    int ret;
    uint8_t cnt = 0;
    if(UVsensor_exist == false)
    {
        ESP_LOGI(TAG, "Check UV sensor status.");
        cnt = 0;
        ret = i2c_master_read_from_device(1, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
        while (ret != ESP_OK && cnt <= 2) {
            cnt ++;
            ESP_LOGW(TAG, "Trying to find UV sensor for %d time(s).",cnt);
            ret = i2c_master_read_from_device(1, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
            if(ret == ESP_OK) break;
        }
        if(ret == ESP_OK) 
        {
            UVsensor_exist = true;
            ESP_LOGW(TAG, "UV sensor exsits.");
        }
        else if(ret != ESP_OK)ESP_LOGW(TAG, "UV sensor doesn't exsit.");
    }
}

void i2c_init(void)
{
    int i2c_master_port_0 = 0;
    int i2c_master_port_1 = 1;

    i2c_config_t conf_0 = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_config_t conf_1 = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 14,
        .scl_io_num = 13,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    /*
    if(Module_Type != Display_Module && 
    Module_Type != Lidar_Start_Module &&
    Module_Type != Lidar_Stop_Module){
        
        printf("Slowing down i2c \n");
        conf.master.clk_speed = 75000;
    }
    */
    i2c_param_config(i2c_master_port_0, &conf_0);
    i2c_param_config(i2c_master_port_1, &conf_1);
    
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port_0, conf_0.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port_1, conf_1.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    ESP_LOGI(TAG, "IIC successfully.");
}

void nvs_init(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Read Module Type
        printf("Reading module type from NVS ... ");

        err = nvs_get_u8(my_handle, "Module_Type", &Module_Type);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("Module_Type = %d\n", Module_Type);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        if (Module_Type == 0)
        {
            // Write
            printf("Updating restart counter in NVS ... ");
            Module_Type = Normal_Module;
            err = nvs_set_u8(my_handle, "Module_Type", Module_Type);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            // Commit written value.
            printf("Committing updates in NVS ... ");
            err = nvs_commit(my_handle);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            print_module_functions();
        }

        // Close
        nvs_close(my_handle);
    }
}

void print_module_functions()
{
    printf("module_info[Module_Type].Name = %s\n",module_info[Module_Type].Name);
    printf("module_info[Module_Type].HasFan = %d\n",module_info[Module_Type].HasFan);
    printf("module_info[Module_Type].HasDisplay = %d\n",module_info[Module_Type].HasDisplay);
}
