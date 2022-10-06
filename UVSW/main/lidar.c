#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "lidar.h"
#include "expander.h"
#include "driver/i2c.h"
#include "hal/i2c_hal.h"
#include "watchdog.h"

typedef struct {
    i2c_hw_cmd_t hw_cmd;
    union {
        uint8_t* data;      // When total_bytes > 1
        uint8_t data_byte;  //when total_byte == 1
    };
    size_t bytes_used;
    size_t total_bytes;
} i2c_cmd_t;

typedef struct i2c_cmd_link {
    i2c_cmd_t cmd;              /*!< command in current cmd link */
    struct i2c_cmd_link *next;  /*!< next cmd link */
} i2c_cmd_link_t;

typedef struct {
    i2c_cmd_link_t *head;     /*!< head of the command link */
    i2c_cmd_link_t *cur;      /*!< last node of the command link */
    i2c_cmd_link_t *free;     /*!< the first node to free of the command link */

    void     *free_buffer;    /*!< pointer to the next free data in user's buffer */
    uint32_t  free_size;      /*!< remaining size of the user's buffer */
} i2c_cmd_desc_t;

#define I2C_TRANS_BUF_MINIMUM_SIZE     (sizeof(i2c_cmd_desc_t) + \
                                        sizeof(i2c_cmd_link_t) * 8) /* It is required to have allocate one i2c_cmd_desc_t per command:
                                                                     * start + write (device address) + write buffer +
                                                                     * start + write (device address) + read buffer + read buffer for NACK +
                                                                     * stop */

static const char *TAG = "lidar";
uint16_t fast_osc_frequency;
uint16_t osc_calibrate_val;


esp_err_t lidar_register_read(uint16_t reg_addr, uint8_t *rx_buf, uint8_t rx_len)
{
    int ret;
    uint8_t cnt = 0;
    uint8_t write_buf[2] = {0};
    write_buf[ 0 ] = reg_addr  >> 8;
    write_buf[ 1 ] = reg_addr & 0x00FF;

    ret = lidar_write_read_device(1, LR4_DEFAULT_SLAVE_ADDRESS, write_buf, 2, \
    rx_buf, rx_len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Lidar reading had a problem.%d",ret);
        Lidar_read_error_flag = true;
    }
    /*
    while (ret != ESP_OK && cnt <= 5) {
        cnt ++;
        ESP_LOGE(TAG, "Lidar reading had a problem for %d time(s), ret = %d",cnt,ret);
        ret = i2c_master_write_read_device(I2C_MASTER_NUM, LR4_DEFAULT_SLAVE_ADDRESS, write_buf, 2, \
        rx_buf, rx_len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS); 
    }
    */
    //if(rx_len == 1)ESP_LOGI(TAG, "Read %d data from %X register : %X", rx_len, reg_addr, rx_buf[0]);
    //else ESP_LOGI(TAG, "Read %d data from %X register : %X,%X", rx_len, reg_addr, rx_buf[0], rx_buf[1]);
    return ret;
}

esp_err_t lidar_register_write(uint16_t reg_addr, uint8_t *tx_buf, uint8_t tx_len)
{
    uint8_t cnt = 0;
    int ret;
    uint8_t write_buf[6] = {0};
    write_buf[ 0 ] = reg_addr  >> 8;
    write_buf[ 1 ] = reg_addr & 0x00FF;
    
    for ( uint8_t cnt = 2; cnt <= tx_len + 1; cnt++ )
    {
        write_buf[ cnt ] = tx_buf[ cnt - 2 ];
    }
    
    ret = i2c_master_write_to_device(1, LR4_DEFAULT_SLAVE_ADDRESS, write_buf, \
    tx_len + 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    while (ret != ESP_OK && cnt <= 5) {
        cnt ++;
        ESP_LOGE(TAG, "Lidar writing had a problem for %d time(s), ret = %d",cnt,ret);
        ret = i2c_master_write_to_device(1, LR4_DEFAULT_SLAVE_ADDRESS, write_buf, \
        tx_len + 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    }
    //if(tx_len==1)ESP_LOGI(TAG, "Write %d data to %X register : %X", tx_len, reg_addr, write_buf[2]);
    //else if(tx_len==2) ESP_LOGI(TAG, "Write %d data to %X register : %X,%X", tx_len, reg_addr, write_buf[2],write_buf[3]);
    //else if(tx_len==4) ESP_LOGI(TAG, "Write %d data to %X register : %X,%X,%X,%X", tx_len, reg_addr, write_buf[2],write_buf[3],write_buf[4],write_buf[5]);
    return ret;
}

esp_err_t lidar_write_read_device(i2c_port_t i2c_num, uint8_t device_address,
                                       const uint8_t* write_buffer, size_t write_size,
                                       uint8_t* read_buffer, size_t read_size,
                                       TickType_t ticks_to_wait)
{
    esp_err_t err = ESP_OK;
    uint8_t buffer[I2C_TRANS_BUF_MINIMUM_SIZE] = { 0 };

    i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    assert (handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, device_address << 1 | I2C_MASTER_WRITE, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write(handle, write_buffer, write_size, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, device_address << 1 | I2C_MASTER_READ, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_read(handle, read_buffer, read_size, I2C_MASTER_LAST_NACK);
    if (err != ESP_OK) {
        goto end;
    }

    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(i2c_num, handle, ticks_to_wait);
    /*
    if (err != ESP_OK) {
        i2c_master_stop(handle);
    }*/
end:
    i2c_cmd_link_delete_static(handle);
    return err;
}

void write_data_u8 (uint16_t addr, uint8_t _data )
{
    lidar_register_write(addr, &_data, 1);
}

void write_data_u16 (uint16_t addr, uint16_t _data )
{
    uint8_t tx_buf[ 2 ]= {0};

    tx_buf[ 0 ] = _data >> 8;
    tx_buf[ 1 ] = _data & 0x00FF;

    lidar_register_write(addr, tx_buf, 2);
}

void write_data_u32 (uint16_t addr, uint32_t _data )
{
    uint8_t tx_buf[ 4 ]= {0};

    tx_buf[ 0 ] = _data >> 24;
    tx_buf[ 1 ] = _data >> 16;
    tx_buf[ 2 ] = _data >> 8;
    tx_buf[ 3 ] = _data & 0x000000FF;

    lidar_register_write(addr, tx_buf, 4);
}

uint8_t read_data_u8 (uint16_t addr )
{
    uint8_t rx_data = {0};

    lidar_register_read(addr, &rx_data, 1);

    return rx_data;
}

uint16_t read_data_u16 (uint16_t addr )
{
    uint8_t rx_buf[ 2 ]= {0};
    uint16_t read_data = 0;

    lidar_register_read(addr, rx_buf, 2);

    read_data = rx_buf[ 0 ];
    read_data <<= 8;
    read_data |= rx_buf[ 1 ];

    return read_data;
}

void lidar_init ( void )
{
    uint8_t tx_buf[2] = {0xE3, 0x7F}; // UV light off   RGB off   ex_3 on
    ESP_LOGI(TAG, "---- Lidar Init ----");
    
    ESP_LOGI(TAG, "lidar_init is warmming up...");
    expander_register_write(EXPAND9_REG_DATA_B, tx_buf, 2);
    read_data_u8( LR4_FIRMWARE_SYSTEM_STATUS );
    
    
    lightranger4_power_on();
    ESP_LOGI(TAG, "--- Wait until the configuration of the chip is completed ---");

    lightranger4_default_cfg();
    lightranger4_set_distance_mode(LR4_DISTANCE_MODE_LONG );
    lightranger4_set_measurement_timing_budget(1000 );
    lightranger4_start_measurement(20);

    ESP_LOGI(TAG, "--- Sensor start measurement ---");
    vTaskDelay(200 / portTICK_PERIOD_MS);
}

void lightranger4_software_reset()
{
    write_data_u8( LR4_REG_SOFT_RESET, 0x00 );
    vTaskDelay(1 / portTICK_PERIOD_MS);
    write_data_u8( LR4_REG_SOFT_RESET, 0x01 );
    ESP_LOGI(TAG, "Lidar soft reset successfully");
}

void lightranger4_set_xshut_pin (expand9_ch_state_value_t ch_state )
{
    uint8_t tx_buf_1[2] ={0x00,0x00}; 
    uint8_t tx_buf_2[2] ={0x00,0x20}; 
    uint8_t rx_buf[2] ={0x00,0x00}; 
    expand9_write_ch (GPIO_ex_3, CH_OUTPUT_ON );
    if(ch_state == CH_OUTPUT_ON)
    {
        ESP_ERROR_CHECK(expander_register_write(   EXPAND9_REG_PULL_UP_B, tx_buf_2, 2 ));
        ESP_ERROR_CHECK(expander_register_write(   EXPAND9_REG_PULL_DOWN_B, tx_buf_1, 2 ));

        ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_PULL_UP_B, rx_buf, 2 ));
        ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_PULL_DOWN_B, rx_buf, 2 ));
    }
    else 
    {
        ESP_ERROR_CHECK(expander_register_write(   EXPAND9_REG_PULL_UP_B, tx_buf_1, 2 ));
        ESP_ERROR_CHECK(expander_register_write(   EXPAND9_REG_PULL_DOWN_B, tx_buf_2, 2 ));

        ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_PULL_UP_B, rx_buf, 2 ));
        ESP_ERROR_CHECK(expander_register_read(   EXPAND9_REG_PULL_DOWN_B, rx_buf, 2 ));
    }
    
}

void lightranger4_power_on ()
{
    uint8_t tx_buf_1[2] = {0xE3, 0x7F}; // UV light off   RGB off   ex_3 on
    uint8_t tx_buf_2[2] = {0xE3, 0x5F}; // UV light off   RGB off   ex_3 off
    //lightranger4_set_xshut_pin(CH_OUTPUT_OFF);
    expander_register_write(EXPAND9_REG_DATA_B, tx_buf_2, 2);
    //expand9_write_ch (GPIO_ex_3, CH_OUTPUT_OFF );
    vTaskDelay(100 / portTICK_PERIOD_MS);
    //lightranger4_set_xshut_pin(CH_OUTPUT_ON);
    expander_register_write(EXPAND9_REG_DATA_B, tx_buf_1, 2);
    //expand9_write_ch (GPIO_ex_3, CH_OUTPUT_ON );
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

LIGHTRANGER4_RETVAL lightranger4_default_cfg ()
{
    //uint16_t model_id;
    uint8_t cnt = 0;
    uint16_t result;
    //uint8_t reg_val;
    uint16_t _data;
    uint8_t status;

    // Click default configuration

    //lightranger4_set_xshut_pin(CH_OUTPUT_ON);  // shutdown

    vTaskDelay(200 / portTICK_PERIOD_MS);
    /*
    model_id = read_data_u8( LR4_IDENTIFICATION_MODEL_ID );
        if ( model_id != 0xEA )
    {
        return LR4_RESP_DEVICE_ERROR_ID_IS_NOT_VALID;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    */
    lightranger4_software_reset();

    vTaskDelay(300 / portTICK_PERIOD_MS);

    status = read_data_u8( LR4_FIRMWARE_SYSTEM_STATUS );

    while ( ( status & 0x01 ) == 0 )
    {
        status = read_data_u8( LR4_FIRMWARE_SYSTEM_STATUS );
        vTaskDelay(10 / portTICK_PERIOD_MS);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        if ( cnt++ == 100 )
        {
            return LR4_RESP_FIRMWARE_TIMEOUT_ERROR;
        }
    }

    result = read_data_u16( LR4_PAD_I2C_HV_EXTSUP_CONFIG );
    result = ( result & 0xFE ) | 0x01;

    write_data_u16( LR4_PAD_I2C_HV_EXTSUP_CONFIG, result );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    fast_osc_frequency = read_data_u16( LR4_REG_FAST_OSC_FREQUENCY );
    osc_calibrate_val = read_data_u16( LR4_RESULT_OSC_CALIBRATE_VAL );

    write_data_u8( LR4_GPIO_TIO_HV_STATUS, 0x02 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SIGMA_ESTIMATOR_EFFECTIVE_PULSE_WIDTH_NS, 8 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SIGMA_ESTIMATOR_EFFECTIVE_AMBIENT_WIDTH_NS, 16 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_ALGO_CROSSTALK_COMPENSATION_VALID_HEIGHT_MM, 0x01 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_ALGO_RANGE_IGNORE_VALID_HEIGHT_MM, 0xFF );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_ALGO_RANGE_MIN_CLIP, 0 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_ALGO_CONSISTENCY_CHECK_TOLERANCE, 2 );
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // general config
    write_data_u16( LR4_SYSTEM_THRESH_RATE_HIGH, 0x0000 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u16( LR4_SYSTEM_THRESH_RATE_LOW, 0x0000 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_DSS_CONFIG_APERTURE_ATTENUATION, 0x38 );

    // timing config
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u16( LR4_RANGE_CONFIG_SIGMA_THRESH, 360 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u16( LR4_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT_MCPS, 192 );

    // dynamic config
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SYSTEM_GROUPED_PARAMETER_HOLD_0, 0x01 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SYSTEM_GROUPED_PARAMETER_HOLD_1, 0x01 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SD_CONFIG_QUANTIFIER, 2 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SYSTEM_GROUPED_PARAMETER_HOLD, 0x00 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SYSTEM_SEED_CONFIG, 1 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_SYSTEM_SEQUENCE_CONFIG, 0x8B );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u16( LR4_DSS_CONFIG_MANUAL_EFFECTIVE_SPADS_SELECT, 200 << 8 );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u8( LR4_DSS_CONFIG_ROI_MODE_CONTROL, 2 );
    vTaskDelay(10 / portTICK_PERIOD_MS);

    _data = read_data_u16( LR4_REG_OUTER_OFFSET_MM );
    vTaskDelay(10 / portTICK_PERIOD_MS);
    write_data_u16( LR4_REG_PART_TO_PART_RANGE_OFFSET_MM, _data * 4 );
    vTaskDelay(10 / portTICK_PERIOD_MS);

    return LR4_RESP_INIT_IS_SUCCESSFUL;
}

uint8_t lightranger4_set_distance_mode (uint8_t mode )
{
    switch ( mode )
    {
        case LR4_DISTANCE_MODE_SHORT:
        {
            write_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_A, 0x07);
            write_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_B, 0x05);
            write_data_u8( LR4_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
            write_data_u8( LR4_SD_CONFIG_WOI_SD0, 0x07);
            write_data_u8( LR4_SD_CONFIG_WOI_SD1, 0x05);
            write_data_u8( LR4_SD_CONFIG_INITIAL_PHASE_SD0, 6);
            write_data_u8( LR4_SD_CONFIG_INITIAL_PHASE_SD1, 6);

            break;
        }
        case LR4_DISTANCE_MODE_MEDIUM:
        {
            write_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_A, 0x0B);
            write_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_B, 0x09);
            write_data_u8( LR4_RANGE_CONFIG_VALID_PHASE_HIGH, 0x78);
            write_data_u8( LR4_SD_CONFIG_WOI_SD0, 0x0B);
            write_data_u8( LR4_SD_CONFIG_WOI_SD1, 0x09);
            write_data_u8( LR4_SD_CONFIG_INITIAL_PHASE_SD0, 10);
            write_data_u8( LR4_SD_CONFIG_INITIAL_PHASE_SD1, 10);
            break;
        }
        case LR4_DISTANCE_MODE_LONG:
        {
            write_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_A, 0x0F);
            write_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_B, 0x0D);
            write_data_u8( LR4_RANGE_CONFIG_VALID_PHASE_HIGH, 0xB8);
            write_data_u8( LR4_SD_CONFIG_WOI_SD0, 0x0F);
            write_data_u8( LR4_SD_CONFIG_WOI_SD1, 0x0D);
            write_data_u8( LR4_SD_CONFIG_INITIAL_PHASE_SD0, 14);
            write_data_u8( LR4_SD_CONFIG_INITIAL_PHASE_SD1, 14);     
            break;
        }
        default:
        {
            return LR4_RESP_WRONG_MODE;
        }
    }
    return LR4_RESP_MODE_SUCCESSFULLY_SET;
}

LIGHTRANGER4_RETVAL lightranger4_set_measurement_timing_budget (uint32_t budget_us )
{
    uint32_t range_config_timeout_us;
    uint32_t macro_period_us;
    uint32_t phasecal_timeout_mclks;
    uint16_t _data16;
    uint8_t _data8;

    if ( budget_us <= 4528 )
    {
        return LR4_RESP_INSUFFICIENT_BUDGET;
    }
    budget_us -= 4528;
    range_config_timeout_us = budget_us;

    if ( range_config_timeout_us > 1100000 )
    {
        return LR4_RESP_TOO_HIGH_BUDGET;
    }
    range_config_timeout_us /= 2;

    _data8 = read_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_A );
    macro_period_us = calc_macro_period( _data8 );

    phasecal_timeout_mclks = timeout_microseconds_to_mclks( 1000, macro_period_us );

    if ( phasecal_timeout_mclks > 0xFF )
    {
        phasecal_timeout_mclks = 0xFF;
    }

    write_data_u8( LR4_PHASECAL_CONFIG_TIMEOUT_MACROP, phasecal_timeout_mclks );

    _data16 = timeout_microseconds_to_mclks(1, macro_period_us);
    _data16 = encode_timeout( _data16 );
    write_data_u16( LR4_MM_CONFIG_TIMEOUT_MACROP_A_HI, _data16);

    _data16 = timeout_microseconds_to_mclks(range_config_timeout_us, macro_period_us);
    _data16 = encode_timeout( _data16 );
    write_data_u16( LR4_RANGE_CONFIG_TIMEOUT_MACROP_A_HI, _data16);

    _data8 = read_data_u8( LR4_RANGE_CONFIG_VCSEL_PERIOD_B );
    macro_period_us = calc_macro_period( _data8 );

    _data16 = timeout_microseconds_to_mclks(1, macro_period_us);
    _data16 = encode_timeout( _data16 );
    write_data_u16( LR4_MM_CONFIG_TIMEOUT_MACROP_B_HI, _data16 );

    _data16 =  timeout_microseconds_to_mclks(range_config_timeout_us, macro_period_us);
    _data16 = encode_timeout( _data16 );
    write_data_u16( LR4_RANGE_CONFIG_TIMEOUT_MACROP_B_HI, _data16);

    return LR4_RESP_BUDGET_IS_SUCCESSFULLY_SET;
}

void lightranger4_start_measurement (uint32_t period_ms )
{
    write_data_u32( LR4_SYSTEM_INTERMEASUREMENT_PERIOD, period_ms * osc_calibrate_val);
    printf("period_ms * osc_calibrate_val = %d\n",period_ms * osc_calibrate_val);
    write_data_u8( LR4_SYSTEM_INTERRUPT_CLEAR, 0x01);
    write_data_u8( LR4_SYSTEM_MODE_START, 0x40);
}

uint8_t lightranger4_new_data_ready ()
{
    uint8_t tmp;

    tmp = read_data_u8 ( LR4_GPIO_TIO_HV_STATUS);

    if ( tmp != 0x03 )
    {
        return 0;
    }
    return 1;
}

uint16_t lightranger4_get_distance ()
{
    uint16_t distance;

    distance = read_data_u16( LR4_RESULT_FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0 );
    return distance;
}

uint8_t lightranger4_get_range_status ()
{
    uint8_t m_status;

    m_status = read_data_u8( LR4_RESULT_RANGE_STATUS ) & 0x1F;

    return m_status;
}

uint32_t calc_macro_period (uint8_t vcsel_period )
{
    uint32_t pll_period_us;
    uint8_t vcsel_period_pclks;
    uint32_t macro_period_us;

    pll_period_us = ( ( uint32_t ) 0x01 << 30 ) / fast_osc_frequency;
    vcsel_period_pclks = ( vcsel_period + 1 ) << 1;
    macro_period_us = ( uint32_t ) 2304 * pll_period_us;
    macro_period_us >>= 6;
    macro_period_us *= vcsel_period_pclks;
    macro_period_us >>= 6;

    return macro_period_us;
}

uint32_t timeout_microseconds_to_mclks ( uint32_t timeout_us, uint32_t macro_period_us )
{
    return ( ( ( uint32_t ) timeout_us << 12 ) + ( macro_period_us >> 1 ) ) / macro_period_us;
}

uint16_t encode_timeout ( uint32_t timeout_mclks )
{
    uint32_t ls_byte = 0;
    uint16_t ms_byte = 0;

    if ( timeout_mclks > 0 )
    {
        ls_byte = timeout_mclks - 1;

        while ( ( ls_byte & 0xFFFFFF00 ) > 0 )
        {
            ls_byte >>= 1;
            ms_byte++;
        }
        return ( ms_byte << 8 ) | ( ls_byte & 0xFF );
    }
    return 0;
}

void lidar_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Lidar task started");
    uint8_t read_buffer[2] = {0};
    size_t read_size = sizeof(read_buffer);
    int ret;
    uint8_t m_status;
    lidar_init(); // Lidar   using IO expander
    while (1)
    {
        if(I2C_occupying == false)
        {
            I2C_occupying = true;
            //  Task implementation.
            /*
            timer_set_counter_value(TIMER_GROUP_0, TIMER_1, 0);
            timer_start(TIMER_GROUP_0, TIMER_1);
            
            while (lightranger4_new_data_ready() != 0)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
                ESP_LOGE(TAG, "lightranger4_new_data_ready() != 0\n");
            }*/
            //vTaskDelay(2 / portTICK_PERIOD_MS);
            

            distance = lightranger4_get_distance();
            ESP_LOGI(TAG, "Distance: %d mm ", distance);
            /*
            if(Lidar_read_error_flag == true && UVsensor_exist == true ) 
            {
                //vTaskSuspend(task_handles[send_thread_handle]);
                //vTaskSuspend(task_handles[udp_server_task_handle]);
                //vTaskDelay(800 / portTICK_PERIOD_MS);
                //i2c_master_read_from_device(I2C_MASTER_NUM, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
                //i2c_master_read_from_device(I2C_MASTER_NUM, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
                //i2c_master_read_from_device(I2C_MASTER_NUM, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
                //vTaskResume(task_handles[send_thread_handle]);
                //vTaskResume(task_handles[udp_server_task_handle]);
                Lidar_read_error_flag = false;
            }
            else if(Lidar_read_error_flag == false) //if(PRINTDATA==1)
            {
                distance = distance_buffer;
                ESP_LOGI(TAG, "Distance: %d mm ", distance);
            }
                */
            //vTaskDelay(2 / portTICK_PERIOD_MS);
            
            
            
            m_status = lightranger4_get_range_status();
            switch (m_status)
            {
                case LR4_MRESP_SIGNAL_FAIL:
                {
                    ESP_LOGI(TAG, "Signal fail.");
                    break;
                }
                case LR4_MRESP_PHASE_OUT_OF_VALID_LIMITS:
                {
                    ESP_LOGI(TAG, "Phase out of valid limits");
                    break;
                }
                case LR4_MRESP_SIGMA_FAIL:
                {
                    ESP_LOGI(TAG, "Sigma Fail. ");
                    break;
                }
                case LR4_MRESP_WRAP_TARGET_FAIL:
                {
                    ESP_LOGI(TAG, "Wrap target fail.");
                    break;
                }
                case LR4_MRESP_MINIMUM_DETECTION_THRESHOLD:
                {
                    ESP_LOGI(TAG, "Target is below minimum detection threshold. ");
                    break;
                }
            }
            //vTaskDelay(2 / portTICK_PERIOD_MS);
            /*if(Lidar_read_error_flag == true) 
            {
                vTaskDelay(200 / portTICK_PERIOD_MS);
                ESP_LOGW(TAG, "Delayed for 200 ms due to lidar reading error");
                Lidar_read_error_flag = false;
            }*/
            
            /*
            timer_pause(TIMER_GROUP_0, TIMER_1);
            timer_get_counter_value(TIMER_GROUP_0, TIMER_1, &Lidar_counter_value);
            Lidar_time_log_ms = (Lidar_counter_value )/ TIMER_SCALE_MS;
            printf("Lidar read time = %d\n",Lidar_time_log_ms);
            if(Lidar_time_log_ms >= 10)ESP_LOGE(TAG, "Lidar read time = %d ms\n",Lidar_time_log_ms);
            */
/*
            timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &Module_counter_value);
            Module_time_log = Module_counter_value / TIMER_SCALE;
            //printf("Module time = %d\n",Module_time_log);
            printf("time : %lld\n",Module_counter_value);
            */
            //printf("Module running time = %ds  UV light on time = %ds\n", Module_time_log, UV_time_log);
            //printf("Time   : %.8f s\r\n", (double) Module_time_log / TIMER_SCALE);

            
            /*

            // UV sensor

            if(UVsensor_exist == true)
            {
                cnt = 0;
                ret = i2c_master_read_from_device(I2C_MASTER_NUM, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
                if(ret == ESP_OK) uv_intensity = read_buffer[1];
                else if(ret != ESP_OK)ESP_LOGE(TAG, "UV sensor reading had a problem.");
                if (PRINTDATA == 1) 
                ESP_LOGI(TAG, "uv_intensity: %d", uv_intensity);
            }
            if(UVsensor_exist == true)
            {
                cnt = 0;
                ret = i2c_master_read_from_device(I2C_MASTER_NUM, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
                if(ret == ESP_OK) uv_intensity = read_buffer[1];
                else if(ret != ESP_OK)ESP_LOGE(TAG, "UV sensor reading had a problem.");
                if (PRINTDATA == 1) 
                ESP_LOGI(TAG, "uv_intensity: %d", uv_intensity);
            }*/
            if(UVsensor_exist == true)
            {
                //cnt = 0;
                ret = i2c_master_read_from_device(1, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
                if(ret == ESP_OK) uv_intensity = read_buffer[1];
                else if(ret != ESP_OK)ESP_LOGE(TAG, "UV sensor reading had a problem.");
                /*
                while (ret != ESP_OK && cnt <= 5) {
                    cnt ++;
                    ESP_LOGE(TAG, "UV sensor reading had a problem for %d time(s), ret = %d",cnt,ret);
                    ret = i2c_master_read_from_device(I2C_MASTER_NUM, 0x36, read_buffer, read_size, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
                }
                */
                if (PRINTDATA == 1) 
                ESP_LOGI(TAG, "uv_intensity: %d", uv_intensity);
            }
            I2C_occupying = false;
        }
        else ESP_LOGE(TAG, "Lidar reading skipped");
        if(watchdog_running == true)CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
        
        Lidar_task_cnt ++;
        vTaskDelay(SENSOR_PERIOD / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}