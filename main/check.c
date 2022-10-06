#include "check.h"

static const char *TAG = "check";

void dev_check_task(void *pvParameter)
{

    vTaskDelay(5000 / portTICK_PERIOD_MS); // Waiting for getting all devices
    uint8_t Normal_missing_num = 0;             // Missing normal module number
    uint8_t Total_dev_num = 122;
    uint8_t Total_normal_num = 116;
    for (int i = 0; i < Active_dev_num; i++)    // Get active normal module number
    { 
        dev_udp_cnt[i] = Dev_ip_library[i][2];
        // if(Dev_ip_library[i][1] == Normal_Module)Active_normal_num ++;
    }

    
    printf("Active_normal_num = %d, Active_dev_num = %d\n", Active_normal_num, Active_dev_num);

    while (1)
    {
        vTaskDelay(300 / portTICK_PERIOD_MS);

        for (int i = 1; i < 8; i++)                 //Assume all the special modules are missing
            Device_missing[i] = 1;
        /* If certain types of module are not applicable at the moment, set below. (DEBUG ONLY)
        Device_missing[Hall1_Module] = 0;
        Device_missing[Hall2_Module] = 0;*/
        Device_missing[Traffic_Module] = 0;
        if (module_info[Module_Type].HasDisplay == true) Device_missing[Display_Module] = 0;

        Device_missing[Total_Missing_Module] = 0;   //Assume all the modules are NOT missing
        Device_missing[Normal_Module] = 0;          //Assume all the normal modules are NOT missing
        Normal_missing_num = 0;

        for (int i = 0; i < Active_dev_num; i++)
        {
            // Check if all the special modules are all functioning
            if (Dev_ip_library[i][1] == Display_Module)
                Device_missing[Display_Module] = 0;
            else if (Dev_ip_library[i][1] == Lidar_Start_Module)
                Device_missing[Lidar_Start_Module] = 0;
            else if (Dev_ip_library[i][1] == Lidar_Stop_Module)
                Device_missing[Lidar_Stop_Module] = 0;
            else if (Dev_ip_library[i][1] == Hall1_Module)
                Device_missing[Hall1_Module] = 0;
            else if (Dev_ip_library[i][1] == Hall2_Module)
                Device_missing[Hall2_Module] = 0;
            else if (Dev_ip_library[i][1] == Traffic_Module)
                Device_missing[Traffic_Module] = 0;

            // If didn't receive new pack from a certain ip. Report this Device missing
            if (dev_udp_cnt[i] == Dev_ip_library[i][2] || Dev_ip_library[i][3] != 0) 
            {   
                if (Dev_ip_library[i][1] == Normal_Module)
                {
                    Normal_missing_num++;
                    Device_missing[Normal_Module] = Normal_missing_num;
                }
                else if (Dev_ip_library[i][1] == Lidar_Start_Module)
                    Device_missing[Lidar_Start_Module] = 1;
                else if (Dev_ip_library[i][1] == Lidar_Stop_Module)
                    Device_missing[Lidar_Stop_Module] = 1;
                else if (Dev_ip_library[i][1] == Hall1_Module)
                    Device_missing[Hall1_Module] = 1;
                else if (Dev_ip_library[i][1] == Hall2_Module)
                    Device_missing[Hall2_Module] = 1;/*
                else if (Dev_ip_library[i][1] == Traffic_Module)
                    Device_missing[Traffic_Module] = 1;
                else if (Dev_ip_library[i][1] == Display_Module)
                    Device_missing[Display_Module] = 1;*/
            }
            else    // This module is functioning, log the received pack number
            { 
                dev_udp_cnt[i] = Dev_ip_library[i][2];
                // if(Dev_ip_library[i][1] == Normal_Module)Active_normal_num ++;
                // else Active_special_num ++;
            }
        }

        // Get missing module number and print it out
        //Active_normal_num = 116 - Normal_missing_num;
        for (int i = 1; i <= 7; i++)
        {
            if (i == Normal_Module)
            {
                Device_missing[Total_Missing_Module] += Normal_missing_num;
                Device_missing[Normal_Module] = Normal_missing_num;
                //Device_missing[0] += Total_normal_num - Active_normal_num + Normal_missing_num;
                //Device_missing[2] = Total_normal_num - Active_normal_num + Normal_missing_num;
                if (Device_missing[Normal_Module] != 0)
                    printf("Missing module 2 %d piece(s)\n", Device_missing[Normal_Module]);
            }
            if (Device_missing[i] >= 1)
            {
                if (i != Normal_Module)
                {
                    printf("Missing module %d\n", i);
                    Device_missing[Total_Missing_Module]++;
                }
            }
        }
        if (module_info[Module_Type].HasDisplay == true)
        {
            if (Device_missing[Total_Missing_Module] >= 1)
            {
                Device_missing_flag = true;
            }
            else
                Device_missing_flag = false;
        }

        /*ESP_LOGI(TAG, "Device num in library = %d, active normal device num = %d, Device_missing = %d",
                 Active_dev_num, Active_normal_num - Normal_missing_num, Device_missing[0]);*/
    }

    vTaskDelete(NULL);
}

void state_task(void *pvParameters)
{
    ESP_LOGI(TAG, "State task started.");
    static char InfoBuffer[512] = {0};
    while (1)
    {


        vTaskGetRunTimeStats((char *)&InfoBuffer);
        printf("\r\n任务名       运行计数         使用率\r\n");
        printf("\r\n%s\r\n", InfoBuffer);
/*
        vTaskList((char *)&InfoBuffer);
         printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
         printf("\r\n%s\r\n", InfoBuffer);
     */
        printf("\r\nTask\t\tCount\tGap\t%lld\n",Module_counter_value);
        printf("send_thread\t%d\t%d\n",send_thread_task_cnt,send_thread_task_cnt-send_thread_task_cnt2);
        send_thread_task_cnt2 = send_thread_task_cnt;
        send_thread_task_cnt = 0;
        printf("udp_server\t%d\t%d\n",udp_server_task_cnt,udp_server_task_cnt-udp_server_task_cnt2 );
        udp_server_task_cnt2 = udp_server_task_cnt;
        udp_server_task_cnt = 0;
        printf("temp_task\t%d\t%d\n",temp_task_cnt,temp_task_cnt-temp_task_cnt2 );
        temp_task_cnt2 = temp_task_cnt;
        temp_task_cnt = 0;
        printf("Light\t\t%d\t%d\n",Light_task_cnt,Light_task_cnt-Light_task_cnt2 );
        Light_task_cnt2 = Light_task_cnt;
        Light_task_cnt = 0;
        printf("Lidar\t\t%d\t%d\n",Lidar_task_cnt,Lidar_task_cnt-Lidar_task_cnt2 );
        Lidar_task_cnt2 = Lidar_task_cnt;
        Lidar_task_cnt = 0;
        printf("Hall\t\t%d\t%d\n",hall_task_cnt,hall_task_cnt-hall_task_cnt2 );
        hall_task_cnt2 = hall_task_cnt;
        hall_task_cnt = 0;
        /*
        ESP_LOGI(TAG, "MALLOC_CAP_DMA : %d",heap_caps_get_free_size (MALLOC_CAP_DMA));
        ESP_LOGI(TAG, "MALLOC_CAP_8BIT : %d",heap_caps_get_free_size (MALLOC_CAP_8BIT));
        //获取芯片可用内存
        printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
        //获取从未使用过的最小内存
        printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
        */
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}