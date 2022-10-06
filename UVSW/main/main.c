#include "top.h"

static const char *TAG = "main";

/**********************
 *   APPLICATION MAIN
 **********************/
void app_main()
{
    sys_timer_init();

    ethernet_init();
    
    nvs_init();

    vTaskDelay(esp_random() % 50); // Delay tasks for random ms(0-50) in order to avoid crashes between different devices
    
    gpio_init();

    i2c_init();
    
    if(module_info[Module_Type].HasFan == true)
        oscio_pwm_init(); // IO expander oscio control for Fan
    /*
    if (Module_Type != Display_Module && Module_Type != Traffic_Module)
        oscio_pwm_init(); // IO expander oscio control for Fan
    */
    expander_init();
    
    if(module_info[Module_Type].HasLightSource == true)
    {
        hw_watchdog_init(); //Hardware watchdog for controlling the UV lamps
        xTaskCreatePinnedToCore(light_task, "light_task", 4096, NULL, 8, &task_handles[light_task_handle], 1);
    }

    if(module_info[Module_Type].HasUVSensor == true)
        check_uv_sensor();  //Check if UV sensor exsits on this module

    if(module_info[Module_Type].HasLidar == true)
        xTaskCreatePinnedToCore(lidar_task, "lidar_task", 4096, NULL, 10, &task_handles[lidar_task_handle], 1);
    
    if(module_info[Module_Type].HasDisplay == true)
        xTaskCreatePinnedToCore(guiTask, "gui", 8192, NULL, 6, &task_handles[guiTask_handle], 0);

    if(module_info[Module_Type].HasHallSensor == true)
        xTaskCreatePinnedToCore(hall_task, "hall_task", 4096, NULL, 4, &task_handles[hall_task_handle], 0);

    if(module_info[Module_Type].HasTrafficLight == true)
        xTaskCreatePinnedToCore(traffic_task, "traffic_task", 4096, NULL, 5, &task_handles[traffic_task_handle], 0);
        
    if(module_info[Module_Type].HasTempSensor == true)
        xTaskCreatePinnedToCore(temp_task, "temp_task", 4096, NULL, 1, &task_handles[temp_task_handle], 0);
      
    if(module_info[Module_Type].HasCheckTask == true)
        xTaskCreatePinnedToCore(dev_check_task, "dev_check_task", 4096, NULL, 1, &task_handles[dev_check_task_handle], 1);

    /*
    if (Module_Type != Display_Module && Module_Type != Check_Module)
    {
        check_uv_sensor();  //Check if UV sensor exsits on this module

        hw_watchdog_init(); //Hardware watchdog for controlling the UV lamps

        xTaskCreatePinnedToCore(light_task, "light_task", 4096, NULL, 8, &task_handles[light_task_handle], 1);
        
        xTaskCreatePinnedToCore(lidar_task, "lidar_task", 4096, NULL, 10, &task_handles[lidar_task_handle], 1);
    }

    if (Module_Type == Traffic_Module)
    {
        xTaskCreatePinnedToCore(traffic_task, "traffic_task", 4096, NULL, 5, &task_handles[traffic_task_handle], 0);
    }

    if (Module_Type == Hall1_Module || Module_Type == Hall2_Module)
    {
        xTaskCreatePinnedToCore(hall_task, "hall_task", 4096, NULL, 4, &task_handles[hall_task_handle], 0);
    }

    if (Module_Type == Display_Module)
    {
        xTaskCreatePinnedToCore(guiTask, "gui", 8192, NULL, 6, NULL, 0);
    }

    if (Module_Type == Check_Module)
    {
        xTaskCreatePinnedToCore(dev_check_task, "dev_check_task", 4096, NULL, 1, NULL, 1);
    }

    xTaskCreatePinnedToCore(temp_task, "temp_task", 4096, NULL, 1, &task_handles[temp_task_handle], 0);
    */
    xTaskCreatePinnedToCore(send_thread, "send_thread", 12000, (void *)AF_INET, 9, &task_handles[send_thread_handle], 1);
    
    xTaskCreatePinnedToCore(udp_server_task, "udp_server", 12000, (void *)AF_INET, 10, &task_handles[udp_server_task_handle], 1);
    
    vTaskDelay(4000 / portTICK_PERIOD_MS);

    sw_watchdog_init();

    xTaskCreate(state_task, "state_task", 4096, NULL, 6, &task_handles[state_task_handle]);

    while (1)
    {

        // ESP_LOGI(TAG, "Running main loop");
        vTaskDelay(400 / portTICK_PERIOD_MS);

        if (ota_start_flag == true)
        {
            if(watchdog_running == true)
            {
                vTaskDelete(task_handles[send_thread_handle]);
                vTaskDelete(task_handles[state_task_handle]);
                if(module_info[Module_Type].HasTempSensor == true)
                vTaskDelete(task_handles[temp_task_handle]);
                if(module_info[Module_Type].HasCheckTask == true)
                vTaskDelete(task_handles[dev_check_task_handle]);
                if(module_info[Module_Type].HasDisplay == true)
                vTaskDelete(task_handles[guiTask_handle]);
                sw_watchdog_deinit();
                ESP_LOGE(TAG, "Delete tasks and software watchdog.");
            }
            xTaskCreate(&ota_task, "ota_task", 8192, NULL, 7, NULL);
            ota_start_flag = false;
        }
        else if (ota_end_flag == true)      //If they are the same version. Update will fail.
        {
            Shutup_Flag = false;
            esp_restart();
            /*
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            xTaskCreatePinnedToCore(send_thread, "send_thread", 12000, (void *)AF_INET, 9, &task_handles[send_thread_handle], 1);
            xTaskCreatePinnedToCore(udp_server_task, "udp_server", 12000, (void *)AF_INET, 10, &task_handles[udp_server_task_handle], 1);
            if(module_info[Module_Type].HasCheckTask == true)
            xTaskCreatePinnedToCore(dev_check_task, "dev_check_task", 4096, NULL, 1, &task_handles[dev_check_task_handle], 1);
            */
            ota_end_flag = false;
        }

        timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &Module_counter_value);
        Module_time_log = Module_counter_value / TIMER_SCALE;
        //printf("time : %lld\n",Module_counter_value);
        //printf("Module running time = %ds  UV light on time = %ds\n", Module_time_log, UV_time_log);
        //printf("Time   : %.8f s\r\n", (double) Module_time_log / TIMER_SCALE);
        // ESP_LOGI(TAG, "uv_on_time_ms = %d", uv_on_time_ms);

        //if(watchdog_running == true)CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
    }
}