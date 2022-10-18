/*!
 * @file top.h
 * @brief This file contains definition of most of variables.
 */

#ifndef TOP_H
#define TOP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_event.h"
#include "driver/i2c.h"
#include "expander.h"
#include "lidar.h"
#include "fan.h"
#include "watchdog.h"
#include "driver/mcpwm.h"
#include "soc/rtc.h"
#include "time.h"
#include "esp_rom_sys.h"
#include "ui.h"
#include "driver/rmt.h"
#include "led_strip.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "errno.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "traffic.h"
#include "check.h"
#include "temp.h"
#include "hall.h"
#include "sys_init.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define EXAMPLE_CHASE_SPEED_MS (10)
#define LAMP_STATUS_1 35
#define LAMP_STATUS_2 34
#define BUZZER 33
#define WATCHDOG 32
#define LED_R 0
#define LED_G 1
#define LED_B 2
#define ON1 10
#define ON2 11
#define BUFFSIZE 1024
#define IAMHERE_PORT 55601
#define CONTROL_PORT 55603
#define RECEIVER_IP_ADDR "255.255.255.255"
#define DISPLAY_IP_ADDR "10.64.65.55"
#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 64  // Multisampling
#define UV_LOOP_PERIOD 40
#define TWDT_TIMEOUT_S  1
#define GPIO_ex_3       5
#define PRINTDATA 0       // enable data printing 1    disable data printing 0
#define SENSOR_PERIOD 100
#define NORMAL_UDP_PERIOD   100
#define SPECIAL_UDP_PERIOD  10

enum Task_handle_Types
{
    send_thread_handle = 0,
    udp_server_task_handle = 1,
    temp_task_handle = 2,
    dev_check_task_handle = 3,
    guiTask_handle = 4,
    hall_task_handle = 5,
    traffic_task_handle = 6,
    lidar_task_handle = 7,
    light_task_handle = 8,
    state_task_handle = 9,
    watchdog_task_handle = 10
};

enum UVM_Module_Types
{
    Display_Module = 1,
    Normal_Module = 2,
    Lidar_Start_Module = 3,
    Lidar_Stop_Module = 4,
    Hall1_Module = 5,
    Hall2_Module = 6,
    Traffic_Module = 7,
    Check_Module = 8
};

enum UVM_Message_Types
{
    UVM_Message_Type_EtherFirmwareUpdate = 1,
    UVM_Message_Type_IAMHERE = 2,
    UVM_Message_Type_CONTROL_UV = 3,
    UVM_Message_Type_Set_Type = 4,
    UVM_Message_Type_Set_RGB = 5,
    UVM_Message_Type_Reboot = 6,
    UVM_Message_Type_Traffic = 7,
    UVM_Message_Type_Shutup = 8
};

enum RGB_Light_Types
{
    Red = 0,
    Green = 1,
    Blue = 2,
    White = 3,
    Yellow = 4,
    Disable = 255
};

typedef struct EtherHeader_Type
{
    uint8_t Type;               //Message type
    //uint8_t SubType;
    //uint8_t HeaderLength;
    char SenderIp[4];
    char ReciverIp[4];         //0xff.0xff.0xff.0xff broadcast
    //uint32_t MessageCRC;        //Zero ignor crc
    
} EtherHeader;

typedef struct EtherFirmwareUpdate_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_EtherFirmwareUpdate;
    char UpdateText[6]; //This should always be "UPDATE"
    uint8_t versionH;
    uint8_t versionL;
    char Path[64];
} EtherFirmwareUpdate;

typedef struct EtherIAMHERE_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_IAMHERE;
    //Current Software Version
    uint8_t versionH; //1
    uint8_t versionL;// 0
    uint8_t module_type;
    uint16_t LidarLength;
    uint8_t Temperature;
    uint16_t FanRPM;
    uint8_t UVIntensity;
    uint8_t UVLightSource1;// 0 on 1 off
    uint8_t UVLightSource2;// 0 on 1 off
    uint16_t HallVoltage;
    uint8_t Device_Info[8];
    uint32_t Module_on_time;
    uint32_t UV_on_time;
} EtherIAMHERE;

typedef struct EtherControlUV_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_CONTROL_UV;
    uint16_t OnTimeMiliSec; //0 off now 
} EtherControlUV;

typedef struct EtherSetType_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_Set_Type;
    uint8_t module_type;
} EtherSetType;

typedef struct EtherSetRGB_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_Set_RGB;
    uint8_t light_color; //255 disable 
} EtherSetRGB;

typedef struct EtherReboot_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_Reboot;
    uint8_t reboot; //1 yes 0 no 
} EtherReboot;

typedef struct EtherTraffic_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_Traffic;
    uint8_t LCD_page; 
} EtherTraffic;

typedef struct EtherShutup_Type
{
    EtherHeader Header; //Header.Type = UVM_Message_Type_Traffic;
    uint8_t Shutup; 
} EtherShutup;

typedef struct Module_Info
{
    uint8_t Type;               //Module type
    char Name[20]; //HALL
    uint16_t send_port;
    uint16_t receive_port;
    uint8_t UDP_sending_period;
    uint8_t HasDisplay;
    uint8_t HasTempSensor;
    uint8_t HasLightSource;
    uint8_t HasLidar;
    uint8_t HasUVSensor;
    uint8_t HasHallSensor;
    uint8_t HasTrafficLight;
    uint8_t HasFan;
    uint8_t HasCheckTask;
    uint8_t HasBuzzer;
} ModuleInfo;

ModuleInfo module_info[10];

// Traffic light parameter
led_strip_t *strip;

// Temprature sensor parameters
esp_adc_cal_characteristics_t *adc_chars;
esp_adc_cal_characteristics_t *hall_chars;
const adc_channel_t channel;   // SENSOR_VN  ADC1_CH3
const adc_channel_t channel_5; // SENSOR_VN  ADC1_CH3
const adc_bits_width_t width;
const adc_atten_t atten;
const adc_unit_t unit;

// Hardware watchdog interrupt
xQueueHandle s_timer_queue;

// Device infos
uint32_t Dev_ip_input;
uint32_t Dev_uv_error;
uint32_t Dev_ip_library[256][4];
uint8_t Active_dev_num;
uint8_t Active_normal_num;
uint8_t Active_special_num;
uint8_t Missing_num;
uint32_t dev_udp_cnt[256];
bool Display_exsit;
bool Lidar_Start_exsit;
bool Lidar_Stop_exsit;
bool Hall1_exsit;
bool Hall2_exsit;
bool Traffic_exsit;
uint8_t Device_status_now[8];
uint8_t Device_missing[8];
bool Device_missing_flag;
bool I2C_occupying;
bool UVsensor_exist;

// IP/MAC INFO
uint8_t Dev_mac[6];
char Dev_ip_num[4];
char Dev_ip_str[32];
char Dest_ip_num[4];
char Dest_ip_str[32];

// UDP related flags
bool ota_start_flag;
bool firmware_updating_flag;
bool ota_running_flag;
bool ota_end_flag;
char ota_write_data[BUFFSIZE + 1];
char rx_buffer[128];
uint8_t udp_rec_type;
bool UV_On_UDP_Flag;
bool LCD_Change_Flag;
bool Shutup_Flag;
bool watchdog_running;
bool hall_sensor_flag;
bool start_sensor_flag;
bool stop_sensor_flag;

// UDP socket related
char addr_str[16];
struct sockaddr_in dest_addr;
int sock;
int socket_fd;

// Device parameters init
uint8_t Module_Type;
uint8_t Restart_cnt;
int32_t UDP_Send_Type;
uint16_t hall_sensor_voltage;
uint16_t distance;
uint16_t Hall1_voltage;
uint16_t Hall2_voltage;
uint16_t Start_distance;
uint16_t Stop_distance;
uint8_t RGB_light_color;
uint8_t temp;
uint16_t RPM;
uint8_t uv_intensity;
uint16_t uv_on_time_ms;
uint8_t wd_status;
uint8_t UVLightSource1_set;
uint8_t UVLightSource2_set;
uint8_t UVLightSource1_status;
uint8_t UVLightSource2_status;
uint16_t OnTimeMiliSec; // 0 off now
int LCD_Page;
int uvMiliLeft;

// Functioning check
int16_t Light_task_cnt;
int16_t Lidar_task_cnt;
int16_t hall_task_cnt;
int16_t temp_task_cnt;
int16_t send_thread_task_cnt;
int16_t udp_server_task_cnt;
int16_t Light_task_cnt2;
int16_t Lidar_task_cnt2;
int16_t hall_task_cnt2;
int16_t temp_task_cnt2;
int16_t send_thread_task_cnt2;
int16_t udp_server_task_cnt2;
uint8_t UVStatus_check;
char *pcWriteBuffer;

uint64_t UV_counter_value;
uint64_t IAMHERE_counter_value;
uint32_t UV_time_log;
uint32_t IAMHERE_time_log_ms;
uint32_t IAMHERE_longest_gap;
uint64_t Module_counter_value;
uint32_t Module_time_log;
bool Lidar_read_error_flag;

// UDP package structure init
EtherFirmwareUpdate Firmware_Update_send;
EtherFirmwareUpdate Firmware_Update_rec;
EtherIAMHERE EtherIAMHERE_send;
//EtherIAMHERE EtherIAMHERE_rec;
EtherControlUV EtherControlUV_send;
EtherControlUV EtherControlUV_rec;
EtherSetType EtherSetType_send;
EtherSetType EtherSetType_rec;
EtherSetRGB EtherSetRGB_send;
EtherSetRGB EtherSetRGB_rec;
EtherReboot EtherReboot_send;
EtherReboot EtherReboot_rec;
EtherTraffic EtherTraffic_send;
EtherTraffic EtherTraffic_rec;
EtherShutup EtherShutup_send;
EtherShutup EtherShutup_rec;

TaskHandle_t task_handles[12];



#endif // TOP_H