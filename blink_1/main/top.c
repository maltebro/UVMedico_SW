#include "top.h"

// Traffic light parameter
led_strip_t *strip;

// Temprature sensor parameters
esp_adc_cal_characteristics_t *adc_chars;
esp_adc_cal_characteristics_t *hall_chars;
const adc_channel_t channel = ADC_CHANNEL_3;   // SENSOR_VN  ADC1_CH3
const adc_channel_t channel_5 = ADC_CHANNEL_5; // SENSOR_VN  ADC1_CH3
const adc_bits_width_t width = ADC_WIDTH_BIT_12;
const adc_atten_t atten = ADC_ATTEN_DB_2_5;
const adc_unit_t unit = ADC_UNIT_1;

// Hardware watchdog interrupt
xQueueHandle s_timer_queue;

// Device infos
uint32_t Dev_ip_input = 0;
uint32_t Dev_uv_error = 0;
uint32_t Dev_ip_library[256][4] = {0};  //1:ip 2:module type 3:receive cnt 4:this mudule is missing or not  
uint8_t Active_dev_num = 0;
uint8_t Active_normal_num = 0;
uint8_t Active_special_num = 0;
uint8_t Missing_num = 0;
uint32_t dev_udp_cnt[256] = {0};
bool Display_exsit = false;
bool Lidar_Start_exsit = false;
bool Lidar_Stop_exsit = false;
bool Hall1_exsit = false;
bool Hall2_exsit = false;
bool Traffic_exsit = false;
uint8_t Device_status_now[8] = {0};
uint8_t Device_missing[8] = {0};
bool Device_missing_flag = false;
bool I2C_occupying = false;
bool UVsensor_exist = false;

// IP/MAC INFO
uint8_t Dev_mac[6];
char Dev_ip_num[4] = {255,255,255,255};
char Dev_ip_str[32];
//char Dest_ip_num[4] = {10,230,96,99};
char Dest_ip_num[4] = {255,255,255,255};
char Dest_ip_str[32];

// UDP related flags
bool ota_start_flag = false;
bool firmware_updating_flag = false;
bool ota_running_flag = false;
bool ota_end_flag = false;
char ota_write_data[BUFFSIZE + 1] = {0};
char rx_buffer[128];
uint8_t udp_rec_type = 0;
bool UV_On_UDP_Flag = false;
bool LCD_Change_Flag = false;
bool Shutup_Flag = 0;
bool watchdog_running = false;
bool hall_sensor_flag = false;
bool start_sensor_flag = false;
bool stop_sensor_flag = false;

// UDP socket related
char addr_str[16];
struct sockaddr_in dest_addr;
int sock = 0;
int socket_fd;

// Device parameters init
uint8_t Module_Type = 0;
uint8_t Restart_cnt = 0;
int32_t UDP_Send_Type = 0;
uint16_t hall_sensor_voltage = 9999;
uint16_t distance = 9999;
uint16_t Hall1_voltage = 9999;
uint16_t Hall2_voltage = 9999;
uint16_t Start_distance = 9999;
uint16_t Stop_distance = 9999;
uint8_t RGB_light_color = Disable;
uint8_t temp = 0;
uint16_t RPM = 0;
uint8_t uv_intensity = 0;
uint16_t uv_on_time_ms = 0;
uint8_t wd_status = 0;
uint8_t UVLightSource1_set = 0;
uint8_t UVLightSource2_set = 0;
uint8_t UVLightSource1_status = 0;
uint8_t UVLightSource2_status = 0;
uint16_t OnTimeMiliSec; // 0 off now
int LCD_Page = Display_Doors;
int uvMiliLeft = 0;

// Functioning check
int16_t Light_task_cnt = 0;
int16_t Lidar_task_cnt = 0;
int16_t hall_task_cnt = 0;
int16_t temp_task_cnt = 0;
int16_t send_thread_task_cnt = 0;
int16_t udp_server_task_cnt = 0;
int16_t Light_task_cnt2 = 0;
int16_t Lidar_task_cnt2 = 0;
int16_t hall_task_cnt2 = 0;
int16_t temp_task_cnt2 = 0;
int16_t send_thread_task_cnt2 = 0;
int16_t udp_server_task_cnt2 = 0;
uint8_t UVStatus_check = 0;
char *pcWriteBuffer = 0x00;

uint64_t UV_counter_value = 0;
uint64_t IAMHERE_counter_value = 0;
uint32_t UV_time_log = 0;
uint32_t IAMHERE_time_log_ms = 0;
uint32_t IAMHERE_longest_gap = 0;
uint64_t Module_counter_value = 0;
uint32_t Module_time_log = 0;
bool Lidar_read_error_flag = false;

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


ModuleInfo module_info[10] = {
    {
        0
    },{
        Display_Module,     //Module type
        "Display Module",   //Module string name
        CONTROL_PORT,  //Send_port
        IAMHERE_PORT,  //Receive_port
        SPECIAL_UDP_PERIOD,         //UDP_sending_period   IAMHERE period = Sending period / 10
        true,       //HasDisplay
        true,       //HasTempSensor
        false,      //HasLightSource
        false,      //HasLidar
        false,      //HasUVSensor
        false,      //HasHallSensor
        false,      //HasTrafficLight
        false,      //HasFan
        true,      //HasCheckTask
        true,       //HasBuzzer
    },{
        Normal_Module,     //Module type
        "Normal Module",   //Module string name
        IAMHERE_PORT,   //Send_port
        CONTROL_PORT,   //Receive_port
        NORMAL_UDP_PERIOD,         //UDP_sending_period
        false,       //HasDisplay
        true,      //HasTempSensor
        true,      //HasLightSource
        true,      //HasLidar
        true,      //HasUVSensor
        false,      //HasHallSensor
        false,      //HasTrafficLight
        true,      //HasFan
        false,      //HasCheckTask
        false,      //HasBuzzer
    },{
        Lidar_Start_Module,     //Module type
        "Lidar Start Module",   //Module string name
        IAMHERE_PORT,  //Send_port
        CONTROL_PORT,   //Receive_port
        SPECIAL_UDP_PERIOD,         //UDP_sending_period
        false,       //HasDisplay
        true,      //HasTempSensor
        true,      //HasLightSource
        true,      //HasLidar
        true,      //HasUVSensor
        false,      //HasHallSensor
        false,      //HasTrafficLight
        true,      //HasFan
        false,      //HasCheckTask
        false,      //HasBuzzer
    },{
        Lidar_Stop_Module,     //Module type
        "Lidar Stop Module",   //Module string name
        IAMHERE_PORT,  //Send_port
        CONTROL_PORT,   //Receive_port
        SPECIAL_UDP_PERIOD,         //UDP_sending_period
        false,       //HasDisplay
        true,      //HasTempSensor
        true,      //HasLightSource
        true,      //HasLidar
        true,      //HasUVSensor
        false,      //HasHallSensor
        false,      //HasTrafficLight
        true,      //HasFan
        false,      //HasCheckTask
        false,      //HasBuzzer
    },{
        Hall1_Module,     //Module type
        "Hall1 Module",   //Module string name
        IAMHERE_PORT,  //Send_port
        CONTROL_PORT,   //Receive_port
        SPECIAL_UDP_PERIOD,         //UDP_sending_period
        false,       //HasDisplay
        true,      //HasTempSensor
        true,      //HasLightSource
        true,      //HasLidar
        true,      //HasUVSensor
        true,      //HasHallSensor
        false,      //HasTrafficLight
        false,      //HasFan
        false,      //HasCheckTask
        false,      //HasBuzzer
    },{
        Hall2_Module,     //Module type
        "Hall2 Module",   //Module string name
        IAMHERE_PORT,  //Send_port
        CONTROL_PORT,   //Receive_port
        SPECIAL_UDP_PERIOD,         //UDP_sending_period
        false,       //HasDisplay
        true,      //HasTempSensor
        true,      //HasLightSource
        true,      //HasLidar
        true,      //HasUVSensor
        true,      //HasHallSensor
        false,      //HasTrafficLight
        false,      //HasFan
        false,      //HasCheckTask
        false,      //HasBuzzer
    },{
        Traffic_Module,     //Module type
        "Traffic Module",   //Module string name
        IAMHERE_PORT,  //Send_port
        CONTROL_PORT,   //Receive_port
        SPECIAL_UDP_PERIOD,         //UDP_sending_period
        false,       //HasDisplay
        true,      //HasTempSensor
        true,      //HasLightSource
        true,      //HasLidar
        true,      //HasUVSensor
        false,      //HasHallSensor
        true,      //HasTrafficLight
        true,      //HasFan
        false,      //HasCheckTask
        false,      //HasBuzzer
    },{
        Check_Module,       //Module type
        "Check Module",     //Module string name
        IAMHERE_PORT,   //Send_port
        IAMHERE_PORT, //Receive_port
        SPECIAL_UDP_PERIOD,         //UDP_sending_period
        false,       //HasDisplay
        true,      //HasTempSensor
        false,      //HasLightSource
        false,      //HasLidar
        false,      //HasUVSensor
        false,      //HasHallSensor
        false,      //HasTrafficLight
        false,      //HasFan
        true,      //HasCheckTask
        false,      //HasBuzzer
    }
    
};
//ModuleType *currentModuleType;