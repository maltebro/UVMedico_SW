#include "expander.h"
#include "ethernet.h"
#include "lidar.h"
#include "driver/mcpwm.h"
#include "top.h"

static const char *TAG = "i2c-expander";

static uint8_t expand9_reg_i_on[ 16 ] = 
{
    EXPAND9_REG_I_ON_0,
    EXPAND9_REG_I_ON_1,
    EXPAND9_REG_I_ON_2,
    EXPAND9_REG_I_ON_3,
    EXPAND9_REG_I_ON_4,
    EXPAND9_REG_I_ON_5,
    EXPAND9_REG_I_ON_6,
    EXPAND9_REG_I_ON_7,
    EXPAND9_REG_I_ON_8,
    EXPAND9_REG_I_ON_9,
    EXPAND9_REG_I_ON_10,
    EXPAND9_REG_I_ON_11,
    EXPAND9_REG_I_ON_12,
    EXPAND9_REG_I_ON_13,
    EXPAND9_REG_I_ON_14,
    EXPAND9_REG_I_ON_15
};

esp_err_t expander_register_read(uint8_t reg_addr, uint8_t *rx_buf, size_t rx_len)
{
    int ret;
    uint8_t cnt = 0;
    ret = i2c_master_write_read_device(I2C_MASTER_NUM, EXPAND9_SET_DEV_ADDR_GND, &reg_addr, 1, \
    rx_buf, rx_len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    while (ret != ESP_OK && cnt <= 5) {
        cnt ++;
        ESP_LOGE(TAG, "IO expander reading had a problem for %d time(s), ret = %d",cnt,ret);
        ret = i2c_master_write_read_device(I2C_MASTER_NUM, EXPAND9_SET_DEV_ADDR_GND, &reg_addr, 1, \
        rx_buf, rx_len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    }
    //if(rx_len == 1)ESP_LOGI(TAG, "Read %d data from %X register : %X", rx_len, reg_addr, rx_buf[0]);
    //else ESP_LOGI(TAG, "Read %d data from %X register : %X,%X", rx_len, reg_addr, rx_buf[0], rx_buf[1]);
    return ret;
}

esp_err_t expander_register_write(uint8_t reg_addr, uint8_t *tx_buf, uint8_t tx_len)
{

    int ret;
    uint8_t write_buf[3] = {0};
    write_buf[0] = reg_addr;
    uint8_t cnt = 0;
    //write_buf[1] = tx_buf[0];
    //write_buf[2] = tx_buf[1];
    
    for ( uint8_t cnt = 1; cnt <= tx_len; cnt++ )
    {
        write_buf[ cnt ] = tx_buf[ cnt - 1 ];
    }
    
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, EXPAND9_SET_DEV_ADDR_GND, write_buf, \
    tx_len+1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    cnt = 0;
    while (ret != ESP_OK && cnt <= 5) {
        cnt ++;
        ESP_LOGE(TAG, "IO expander writing had a problem for %d time(s), ret = %d",cnt,ret);
        ret = i2c_master_write_to_device(I2C_MASTER_NUM, EXPAND9_SET_DEV_ADDR_GND, write_buf, \
        tx_len+1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    }
    //if(tx_len==1)ESP_LOGI(TAG, "Write %d data to %X register : %X", tx_len, reg_addr, write_buf[1]);
    //else ESP_LOGI(TAG, "Write %d data to %X register : %X,%X", tx_len, reg_addr, write_buf[1],write_buf[2]);
    return ret;
}

void expander_init (void)
{
    uint8_t rx_buf[2] ={0x00,0x00}; 
    //uint8_t tx_buf_1[ 2 ] = {0xE3,0x78};
    uint8_t tx_buf_1[ 2 ] = {0xE3,0x7F};
    expand9_soft_reset();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    expand9_set_all_ch_output();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    /*
    expand9_write_ch (0, CH_OUTPUT_OFF );    //LED_R
    expand9_write_ch (1, CH_OUTPUT_OFF );    //LED_G
    expand9_write_ch (2, CH_OUTPUT_OFF );   //LED_B  0 = ON
    expand9_write_ch (10, CH_OUTPUT_ON );   //UV LIGHT 1    1 = ON
    expand9_write_ch (11, CH_OUTPUT_ON );   //UV LIGHT 2    1 = ON
    expand9_write_ch (13, CH_OUTPUT_ON );   //FAN    1 = ON
    */
    ESP_ERROR_CHECK(expander_register_write(   EXPAND9_REG_DATA_B, tx_buf_1, 2 ));
    //if(Module_Type != Display_Module && Module_Type != Traffic_Module){
    if(module_info[Module_Type].HasFan == true){    
        expand9_led_driver_config(12, EXPAND9_FREQ_DIV_1, EXPAND9_LED_MODE_LINEAR );    //FAN_PWM
        expand9_set_intensity(12, FAN_DUTY_CYCLE );
    }
    /*
    ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_PULL_UP_B, rx_buf, 2 ));
    ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_PULL_DOWN_B, rx_buf, 2 ));
    ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_DIR_B, rx_buf, 2 ));
    ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_DATA_B, rx_buf, 2 ));
    */

    ESP_LOGI(TAG, "Warming up IO Expander...");
    expander_register_write(EXPAND9_REG_DATA_B, tx_buf_1, 2);

    ESP_LOGI(TAG, "IO Expander channel configured successfully");
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void expand9_soft_reset() 
{
    uint8_t tx_buf[1]={0};
    
    tx_buf[0] = 0x12;
    expander_register_write(EXPAND9_REG_RESET, tx_buf, 1);
    
    tx_buf[0] = 0x34;
    expander_register_write(EXPAND9_REG_RESET, tx_buf, 1);

    ESP_LOGI(TAG, "Soft reset successfully");
}


void expand9_set_ch_direction (uint8_t ch_pos, expand9_direction_value_t direction ) 
{
    uint8_t tx_buf[ 2 ];
    uint8_t rx_buf[ 2 ];
    uint16_t sel_data;
    
    
    expander_register_read(EXPAND9_REG_DIR_B, rx_buf, 2);
    
    sel_data = rx_buf[ 0 ];
    //ESP_LOGI(TAG, "sel_data1 = %X",sel_data);
    sel_data <<= 8;
    sel_data |= rx_buf[ 1 ];
    //ESP_LOGI(TAG, "sel_data2 = %X",sel_data);
    if ( direction == DIRECTION_INPUT )
    {
        sel_data |= ( 0x0001 << ch_pos );
    }
    else
    {
        sel_data &= ~( 0x0001 << ch_pos );    
    }
    //ESP_LOGI(TAG, "sel_data3 = %X",sel_data);
    tx_buf[ 0 ] = ( uint8_t ) ( sel_data >> 8 );
    tx_buf[ 1 ] = ( uint8_t )   sel_data;
    //ESP_LOGI(TAG, "tx_buf : %X,%X", tx_buf[0], tx_buf[1]);
    expander_register_write(EXPAND9_REG_DIR_B, tx_buf, 2);
    
}

void expand9_set_ch_output_state (uint8_t ch_pos, expand9_ch_state_value_t ch_state ) 
{
    uint8_t tx_buf[ 2 ];
    uint8_t rx_buf[ 2 ];
    uint16_t sel_data;
    
    
    expand9_set_ch_direction(ch_pos, DIRECTION_OUTPUT );
    
    expander_register_read(EXPAND9_REG_DATA_B, rx_buf, 2 );
    
    sel_data = rx_buf[ 0 ];
    //ESP_LOGI(TAG, "sel_data4 = %X",sel_data);
    sel_data <<= 8;
    sel_data |= rx_buf[ 1 ];
    //ESP_LOGI(TAG, "sel_data5 = %X",sel_data);
    
    if ( ch_state == CH_OUTPUT_ON ) 
    {
        sel_data |= ( 0x0001 << ch_pos );
    }
    else
    {
        sel_data &= ~( 0x0001 << ch_pos );    
    }
    //ESP_LOGI(TAG, "sel_data6 = %X",sel_data);
    tx_buf[ 0 ] = ( uint8_t ) ( sel_data >> 8 );
    tx_buf[ 1 ] = ( uint8_t )   sel_data;
    //ESP_LOGI(TAG, "tx_buf[0] = %X,tx_buf[1]=%X",tx_buf[ 0 ],tx_buf[ 1 ]);
    expander_register_write(EXPAND9_REG_DIR_B, tx_buf, 2);
    
}

uint8_t expand9_get_ch_output_state (uint8_t ch_pos) 
{
    uint8_t rx_buf[ 2 ];
    uint16_t sel_data;
    uint8_t flag = 0;
    
    //expand9_set_ch_direction(ch_pos, DIRECTION_INPUT );
    
    expander_register_read(EXPAND9_REG_DATA_B, rx_buf, 2 );

    //expand9_set_ch_direction(ch_pos, DIRECTION_OUTPUT );
    
    sel_data = rx_buf[ 0 ];
    sel_data <<= 8;
    sel_data |= rx_buf[ 1 ];
    
    if ( sel_data & ( 0x0001 << ch_pos ) ) 
    {  
        flag = 1;
    }
    else
    {
        flag = 0;
    }
    return flag;
}

void expand9_write_ch (uint8_t ch_pos, expand9_ch_state_value_t ch_state )
{
    uint8_t tx_buf[ 2 ] = {0,0};
    uint8_t rx_buf[ 2 ]= {0,0};
    uint16_t temp_dir;
    
    expander_register_read(EXPAND9_REG_DIR_B, rx_buf, 2 );
    
    temp_dir = rx_buf[ 0 ];
    temp_dir <<= 8;
    temp_dir |= rx_buf[ 1 ];

    if ( ( 0xFFFF ^ temp_dir ) & ( 0x0001 << ch_pos ) )	
    {
        uint16_t temp_data;
        
        expander_register_read(   EXPAND9_REG_DATA_B, rx_buf, 2 );
        
        temp_data = rx_buf[ 0 ];
        temp_data <<= 8;
        temp_data |= rx_buf[ 1 ];
        
        if ( ch_state )
        {
            temp_data |=  ( 0x0001 << ch_pos );
        }
        else
        {
            temp_data &= ~( 0x0001 << ch_pos );
        }
        
        tx_buf[ 0 ] = ( uint8_t ) ( temp_data >> 8 );
        tx_buf[ 1 ] = ( uint8_t )   temp_data;
        expander_register_write(   EXPAND9_REG_DATA_B, tx_buf, 2 );
    }
    else
    {
        uint16_t temp_pull_up;
        uint16_t temp_pull_down;
        
        expander_register_read(   EXPAND9_REG_PULL_UP_B, rx_buf, 2 );
        
        temp_pull_up = rx_buf[ 0 ];
        temp_pull_up <<= 8;
        temp_pull_up |= rx_buf[ 1 ];
        
        expander_register_read(   EXPAND9_REG_PULL_DOWN_B, rx_buf, 2 );
        
        temp_pull_down = rx_buf[ 0 ];
        temp_pull_down <<= 8;
        temp_pull_down |= rx_buf[ 1 ];

        if ( ch_state == CH_OUTPUT_ON )
        {
            temp_pull_up |= ( 0x0001 << ch_pos );
            tx_buf[ 0 ] = ( uint8_t ) ( temp_pull_up >> 8 );
            tx_buf[ 1 ] = ( uint8_t )   temp_pull_up;
            expander_register_write(   EXPAND9_REG_PULL_UP_B, tx_buf, 2 );
            
            temp_pull_down &= ~( 0x0001 << ch_pos );
            tx_buf[ 0 ] = ( uint8_t ) ( temp_pull_down >> 8 );
            tx_buf[ 1 ] = ( uint8_t )   temp_pull_down;
            expander_register_write(   EXPAND9_REG_PULL_DOWN_B, tx_buf, 2 );
            
        }
        else
        {
            temp_pull_up &= ~( 0x0001 << ch_pos );
            tx_buf[ 0 ] = ( uint8_t ) ( temp_pull_up >> 8 );
            tx_buf[ 1 ] = ( uint8_t )   temp_pull_up;
            expander_register_write(   EXPAND9_REG_PULL_UP_B, tx_buf, 2 );
            
            temp_pull_down |= ( 0x0001 << ch_pos );
            tx_buf[ 0 ] = ( uint8_t ) ( temp_pull_down >> 8 );
            tx_buf[ 1 ] = ( uint8_t )   temp_pull_down;
            expander_register_write(   EXPAND9_REG_PULL_DOWN_B, tx_buf, 2 );
        }
    }
    
}

void expand9_read_ch(uint8_t ch_pos, expand9_ch_state_value_t *ch_state )
{
    uint8_t rx_buf[ 2 ];
    uint16_t temp;

    
    expander_register_read(EXPAND9_REG_DIR_B, rx_buf, 2 );
    
    temp = rx_buf[ 0 ];
    temp <<= 8;
    temp |= rx_buf[ 1 ];
    
    if ( temp & ( 0x0001 << ch_pos ) )
    {
        expander_register_read(EXPAND9_REG_DATA_B, rx_buf, 2 );
        temp = rx_buf[ 0 ];
        temp <<= 8;
        temp |= rx_buf[ 1 ];
        if ( temp & ( 1 << ch_pos ) )
        {
            *ch_state = CH_OUTPUT_ON;
        }
        else
        {
            *ch_state = CH_OUTPUT_OFF;    
        }
    }
    
}

void expand9_led_driver_config (uint8_t ch_pos, uint8_t freq_div, uint8_t lin_log )
{
    uint8_t tx_buf[ 2 ];
    uint8_t rx_buf[ 2 ];
    uint16_t temp;
    

    ESP_ERROR_CHECK(expander_register_read(EXPAND9_REG_INPUT_DISABLE_B, rx_buf, 2 ));
    
    temp = rx_buf[ 0 ];
    temp <<= 8;
    temp |= rx_buf[ 1 ];
    
    temp |= ( 0x0001 << ch_pos );
    tx_buf[ 0 ] = ( uint8_t ) ( temp >> 8 );
    tx_buf[ 1 ] = ( uint8_t )   temp;
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_INPUT_DISABLE_B, tx_buf, 2 ));

    ESP_ERROR_CHECK(expander_register_read(EXPAND9_REG_PULL_UP_B, rx_buf, 2 ));
    
    temp = rx_buf[ 0 ];
    temp <<= 8;
    temp |= rx_buf[ 1 ];
    
    temp &= ~( 0x0001 << ch_pos );
    tx_buf[ 0 ] = ( uint8_t ) ( temp >> 8 );
    tx_buf[ 1 ] = ( uint8_t )   temp;
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_PULL_UP_B, tx_buf, 2 ));

    ESP_ERROR_CHECK(expander_register_read(EXPAND9_REG_DIR_B, rx_buf, 2 ));
    
    temp = rx_buf[ 0 ];
    temp <<= 8;
    temp |= rx_buf[ 1 ];
    
    temp &= ~( 0x0001 << ch_pos );
    tx_buf[ 0 ] = ( uint8_t ) ( temp >> 8 );
    tx_buf[ 1 ] = ( uint8_t )   temp;
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_DIR_B, tx_buf, 2 ));    
    
    ESP_ERROR_CHECK(expander_register_read(EXPAND9_REG_CLOCK, rx_buf, 1 ));
    
    tx_buf[ 0 ] = rx_buf [ 0 ];
    tx_buf[ 0 ] |= ( 1 << 6 );
    tx_buf[ 0 ] &= ~( 1 <<5 );
    rx_buf[ 0 ] |= freq_div;
    tx_buf[ 0 ] = 0x20;     //using external clock input (OSCIN)
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_CLOCK, tx_buf, 1 ));  
    

    ESP_ERROR_CHECK(expander_register_read(EXPAND9_REG_MISC, rx_buf, 1 ));
    if ( lin_log )
    {
        rx_buf[ 0 ] |= 0x80;
        rx_buf[ 0 ] |= 0x08;
    }
    else
    {
        rx_buf[ 0 ] &= ~0x80;
        rx_buf[ 0 ] &= ~0x08;
    }
    tx_buf[ 0 ] = 0x10;     //freq_div = 1
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_MISC, tx_buf, 1 )); 

    ESP_ERROR_CHECK(expander_register_read(EXPAND9_REG_LED_DRIVER_ENABLE_B, rx_buf, 2 ));
    
    temp = rx_buf[ 0 ];
    temp <<= 8;
    temp |= rx_buf[ 1 ];
    
    temp |= ( 0x0001 << ch_pos );
    tx_buf[ 0 ] = ( uint8_t ) ( temp >> 8 );
    tx_buf[ 1 ] = ( uint8_t )   temp;
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_LED_DRIVER_ENABLE_B, tx_buf, 2 ));

    ESP_ERROR_CHECK(expander_register_read(EXPAND9_REG_DATA_B, rx_buf, 2 ));
    
    temp = rx_buf[ 0 ];
    temp <<= 8;
    temp |= rx_buf[ 1 ];
    
    temp &= ~( 0x0001 << ch_pos );
    tx_buf[ 0 ] = ( uint8_t ) ( temp >> 8 );
    tx_buf[ 1 ] = ( uint8_t )   temp;
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_DATA_B, tx_buf, 2 ));

}

void expand9_set_intensity (uint8_t ch_pos, uint8_t intensity )
{    
    ESP_ERROR_CHECK(expander_register_write(expand9_reg_i_on[ ch_pos ], &intensity, 1 ));
}

void expand9_set_all_ch_output() 
{
    uint8_t tx_buf[ 2 ] = { 0x00, 0x00 };
    
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_DIR_B, tx_buf, 2 ));
}

void expand9_set_all_ch_input() 
{
    uint8_t tx_buf[ 2 ] = { 0xFF, 0xFF };
    
    ESP_ERROR_CHECK(expander_register_write(EXPAND9_REG_DIR_B, tx_buf, 2 ));
}

void expand9_lcd_send_cmd ()
{
    uint8_t tx_buf[ 1 ] = {0x78}; 
    //uint8_t tx_buf[ 1 ] = {0x7F}; 
    vTaskDelay(1 / portTICK_RATE_MS);
    expander_register_write(   EXPAND9_REG_DATA_A, tx_buf, 1 );
    //expand9_write_ch(7,CH_OUTPUT_OFF);
}

void expand9_lcd_send_data ()
{
    uint8_t tx_buf[ 1 ] = {0xF8}; 
    //uint8_t tx_buf[ 1 ] = {0x7F}; 
    vTaskDelay(1 / portTICK_RATE_MS);
    expander_register_write(   EXPAND9_REG_DATA_A, tx_buf, 1 );
    //expand9_write_ch(7,CH_OUTPUT_ON);
}

void pwm_gpio_initialize(void)
{
    ESP_LOGI(TAG, "initializing pwm gpio...");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, OSCIO_GPIO);
    //mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, WATCHDOG);
}

void oscio_pwm_init(void)
{
    /* pwm gpio initialization */
    pwm_gpio_initialize();
    /* initial mcpwm configuration */
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 5100000;     //frequency = 1kHz,
    pwm_config.cmpr_a = 50.0;                              //initial duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 50.0;                              //initial duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;         //up counting mode
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    ESP_LOGI(TAG, "PWM init successfully.");
}

void light_task(void *pvParameter)
{
    ESP_LOGI(TAG, "light task started");
    // expand9_write_ch(10,CH_OUTPUT_ON);
    // expand9_write_ch(11,CH_OUTPUT_ON);
    uint8_t uv_on_flag = 0;
    uint8_t color_now = 4;
    uint8_t uv_check_cnt = 0;
    // uint8_t tx_buf_1[ 2 ] = {0xEF,0x7B};        //UV light on    blue RGB
    // uint8_t tx_buf_2[ 2 ] = {0xE3,0x78};        //UV light off   white RGB
    uint8_t tx_buf_1[2] = {0xEF, 0x7F}; // UV light on    RGB off
    uint8_t tx_buf_2[2] = {0xE3, 0x7F}; // UV light off   RGB off
    uint8_t tx_buf_3[2] = {0xE3, 0x78}; // UV light off   white RGB
    uint8_t tx_buf_4[2] = {0xE3, 0x7B}; // UV light off   blue RGB
    uint8_t tx_buf_5[2] = {0xE3, 0x7D}; // UV light off   green RGB
    uint8_t tx_buf_6[2] = {0xE3, 0x7E}; // UV light off   red RGB

    ESP_LOGI(TAG, "light_task is warmming up...");
    expander_register_write(EXPAND9_REG_DATA_B, tx_buf_2, 2);
    
    while (1)
    {

        UVLightSource1_status = gpio_get_level(LAMP_STATUS_1);
        UVLightSource2_status = gpio_get_level(LAMP_STATUS_2);
        if (RGB_light_color == Disable)
        {
            // Control UV light ON TIME(ms)
            if (uv_on_time_ms >= UV_LOOP_PERIOD)
            {
                uv_on_time_ms = uv_on_time_ms - UV_LOOP_PERIOD;
                //if (uv_on_flag == 0 || UVLightSource1_status == 1)
                if (uv_on_flag == 0)
                {
                    /*
                    expand9_write_ch (10,CH_OUTPUT_ON);
                    expand9_write_ch (11,CH_OUTPUT_ON);
                    expand9_write_ch (0, CH_OUTPUT_ON );    //LED_R  0 = ON
                    expand9_write_ch (1, CH_OUTPUT_ON );    //LED_G  0 = ON
                    expand9_write_ch (2, CH_OUTPUT_OFF );   //LED_B  0 = ON     Blue light is on here
                    */
                    
                    //if( uv_on_flag == 0)
                      //  vTaskDelay(rand()%100);
                    if(I2C_occupying == false)
                    {
                        I2C_occupying = true;
                        expander_register_write(EXPAND9_REG_DATA_B, tx_buf_1, 2);
                        timer_set_counter_value(TIMER_GROUP_1, TIMER_1, 0);
                        timer_start(TIMER_GROUP_1, TIMER_1);
                        ESP_LOGI(TAG, "Turn on uv light");
                        uv_on_flag = 1;
                        I2C_occupying = false;
                    }
                    else ESP_LOGE(TAG, "UV light turn on command skipped");
                }
            }
            else
            {
                if (uv_on_flag == 1 || UVLightSource1_status == 0)
                {
                    /*
                    expand9_write_ch (0, CH_OUTPUT_OFF );   //All lights are on (White)
                    expand9_write_ch (1, CH_OUTPUT_OFF );
                    expand9_write_ch (2, CH_OUTPUT_OFF );
                    expand9_write_ch (10,CH_OUTPUT_OFF);
                    expand9_write_ch (11,CH_OUTPUT_OFF);
                    */
                    if(I2C_occupying == false)
                    {
                        I2C_occupying = true;
                        expander_register_write(EXPAND9_REG_DATA_B, tx_buf_2, 2);
                        timer_pause(TIMER_GROUP_1, TIMER_1);
                        timer_set_counter_value(TIMER_GROUP_1, TIMER_1, 0);
                        ESP_LOGI(TAG, "Turn off uv light");
                        uv_on_flag = 0;
                        I2C_occupying = false;
                    }
                    else ESP_LOGE(TAG, "UV light turn on command skipped");
                }
            }
        }
        else if (RGB_light_color != Disable)
        {
            // ESP_LOGW(TAG, "RGB_light_color != Disable. Something is wrong");
            //  Control UV light ON TIME(ms)
            if (uv_on_time_ms >= UV_LOOP_PERIOD)
            {
                uv_on_time_ms = uv_on_time_ms - UV_LOOP_PERIOD;
                if (uv_on_flag == 0)
                {
                    expand9_write_ch(10, CH_OUTPUT_ON);
                    expand9_write_ch(11, CH_OUTPUT_ON);
                    uv_on_flag = 1;
                }
            }
            else
            {
                if (uv_on_flag == 1)
                {
                    expand9_write_ch(10, CH_OUTPUT_OFF);
                    expand9_write_ch(11, CH_OUTPUT_OFF);
                    uv_on_flag = 0;
                }
            }
            // light color control
            if (RGB_light_color == White && color_now != White)
            {
                /*
                expand9_write_ch(0, CH_OUTPUT_OFF); // All lights are on (White)
                expand9_write_ch(1, CH_OUTPUT_OFF);
                expand9_write_ch(2, CH_OUTPUT_OFF);
                */
                expander_register_write(EXPAND9_REG_DATA_B, tx_buf_3, 2);
                color_now = White;
            }
            else if (RGB_light_color == Blue && color_now != Blue)
            {
                /*
                expand9_write_ch(0, CH_OUTPUT_ON);  // LED_R  0 = ON
                expand9_write_ch(1, CH_OUTPUT_ON);  // LED_G  0 = ON
                expand9_write_ch(2, CH_OUTPUT_OFF); // LED_B  0 = ON     Blue light is on here
                */
                expander_register_write(EXPAND9_REG_DATA_B, tx_buf_4, 2);
                color_now = Blue;
            }
            else if (RGB_light_color == Green && color_now != Green)
            {
                /*
                expand9_write_ch(0, CH_OUTPUT_ON);  // LED_R  0 = ON
                expand9_write_ch(1, CH_OUTPUT_OFF); // LED_G  0 = ON
                expand9_write_ch(2, CH_OUTPUT_ON);  // LED_B  0 = ON     Green light is on here
                */
                expander_register_write(EXPAND9_REG_DATA_B, tx_buf_5, 2);
                color_now = Green;
            }
            else if (RGB_light_color == Red && color_now != Red)
            {
                /*
                expand9_write_ch(0, CH_OUTPUT_OFF); // LED_R  0 = ON
                expand9_write_ch(1, CH_OUTPUT_ON);  // LED_G  0 = ON
                expand9_write_ch(2, CH_OUTPUT_ON);  // LED_B  0 = ON     Red light is on here
                */
                expander_register_write(EXPAND9_REG_DATA_B, tx_buf_6, 2);
                color_now = Red;
            }
            
        }

        if (uv_on_flag == UVLightSource1_status)
        { // NOT EQUAL
            uv_check_cnt++;
            if (uv_check_cnt == 70)
            {
                uv_check_cnt = 0;
                Device_missing[0] = 1;
                Device_missing[Module_Type] = 1;
                
            }
        }
        else
        { // EQUAL
            uv_check_cnt = 0;
            Device_missing[0] = 0;
            Device_missing[Module_Type] = 0;
        }

        // Get UV light control set and OnOffStatus
        UVLightSource1_set = expand9_get_ch_output_state(ON1);
        UVLightSource2_set = expand9_get_ch_output_state(ON2);
        // ESP_LOGI(TAG, "LAMP_STATUS_1 = %d. LAMP_STATUS_2 = %d", UVLightSource1_status, UVLightSource1_status);
        // Print above data
        // if(PRINTDATA==1){
        // ESP_LOGI(TAG, "ON1 = %d. ON2 = %d", UVLightSource1_set, UVLightSource2_set);
        //ESP_LOGW(TAG, "LAMP_STATUS_1 = %d. LAMP_STATUS_2 = %d", UVLightSource1_status, UVLightSource2_status);
        //}
        Light_task_cnt++;
        //ESP_LOGI(TAG, "Light_task_cnt = %d", Light_task_cnt);
        timer_get_counter_value(TIMER_GROUP_1, TIMER_1, &UV_counter_value);
        UV_time_log = UV_counter_value / TIMER_SCALE;
        
        if(watchdog_running == true)CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
        vTaskDelay(UV_LOOP_PERIOD / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}