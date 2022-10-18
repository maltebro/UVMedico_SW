/*!
 * @file expander.h
 * @brief This file contains API for Expand 9 Click Driver.
 */

#ifndef EXPANDER_H
#define EXPANDER_H

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"


#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       500

#define FAN_DUTY_CYCLE  50
#define OSCIO_GPIO      2

/*!
 * @addtogroup expand9 Expand 9 Click Driver
 * @brief API for configuring and manipulating Expand 9 Click driver.
 * @{
 */

/**
 * @defgroup expand9_reg Expand 9 Registers List
 * @brief List of registers of Expand 9 Click driver.
 */

/**
 * @addtogroup expand9_reg
 * @{
 */

/**
 * @brief Expand 9 description register.
 * @details Specified register for description of Expand 9 Click driver.
 */
#define EXPAND9_REG_INPUT_DISABLE_B	             0x00
#define EXPAND9_REG_INPUT_DISABLE_A	             0x01
#define EXPAND9_REG_LONG_SLEW_B	                 0x02
#define EXPAND9_REG_LONG_SLEW_A                  0x03
#define EXPAND9_REG_LOW_DRIVE_B                  0x04
#define EXPAND9_REG_LOW_DRIVE_A                  0x05
#define EXPAND9_REG_PULL_UP_B                    0x06
#define EXPAND9_REG_PULL_UP_A                    0x07
#define EXPAND9_REG_PULL_DOWN_B                  0x08
#define EXPAND9_REG_PULL_DOWN_A                  0x09
#define EXPAND9_REG_OPEN_DRAIN_B                 0x0A
#define EXPAND9_REG_OPEN_DRAIN_A                 0x0B
#define EXPAND9_REG_POLARITY_B                   0x0C
#define EXPAND9_REG_POLARITY_A                   0x0D
#define EXPAND9_REG_DIR_B                        0x0E
#define EXPAND9_REG_DIR_A                        0x0F
#define EXPAND9_REG_DATA_B                       0x10
#define EXPAND9_REG_DATA_A                       0x11
#define EXPAND9_REG_INTERRUPT_MASK_B             0x12
#define EXPAND9_REG_INTERRUPT_MASK_A             0x13
#define EXPAND9_REG_SENSE_HIGH_B                 0x14
#define EXPAND9_REG_SENSE_LOW_B                  0x15
#define EXPAND9_REG_SENSE_HIGH_A                 0x16
#define EXPAND9_REG_SENSE_LOW_A                  0x17
#define EXPAND9_REG_INTERRUPT_SOURCE_B           0x18
#define EXPAND9_REG_INTERRUPT_SOURCE_A           0x19
#define EXPAND9_REG_EVENT_STATUS_B               0x1A
#define EXPAND9_REG_EVENT_STATUS_A               0x1B
#define EXPAND9_REG_LEVEL_SHIFTER_1              0x1C
#define EXPAND9_REG_LEVEL_SHIFTER_2              0x1D
#define EXPAND9_REG_CLOCK                        0x1E
#define EXPAND9_REG_MISC                         0x1F
#define EXPAND9_REG_LED_DRIVER_ENABLE_B          0x20
#define EXPAND9_REG_LED_DRIVER_ENABLE_A          0x21
#define EXPAND9_REG_DEBOUNCE_CONFIG              0x22
#define EXPAND9_REG_DEBOUNCE_ENABLE_B            0x23
#define EXPAND9_REG_DEBOUNCE_ENABLE_A            0x24
#define EXPAND9_REG_KEY_CONFIG_1                 0x25
#define EXPAND9_REG_KEY_CONFIG_2                 0x26
#define EXPAND9_REG_KEY_DATA_1                   0x27
#define EXPAND9_REG_KEY_DATA_2                   0x28
#define EXPAND9_REG_T_ON_0                       0x29
#define EXPAND9_REG_I_ON_0                       0x2A
#define EXPAND9_REG_OFF_0                        0x2B
#define EXPAND9_REG_T_ON_1                       0x2C
#define EXPAND9_REG_I_ON_1                       0x2D
#define EXPAND9_REG_OFF_1                        0x2E
#define EXPAND9_REG_T_ON_2                       0x2F
#define EXPAND9_REG_I_ON_2                       0x30
#define EXPAND9_REG_OFF_2                        0x31
#define EXPAND9_REG_T_ON_3                       0x32
#define EXPAND9_REG_I_ON_3                       0x33
#define EXPAND9_REG_OFF_3                        0x34
#define EXPAND9_REG_T_ON_4                       0x35
#define EXPAND9_REG_I_ON_4                       0x36
#define EXPAND9_REG_OFF_4                        0x37
#define EXPAND9_REG_T_RISE_4                     0x38
#define EXPAND9_REG_T_FALL_4                     0x39
#define EXPAND9_REG_T_ON_5                       0x3A
#define EXPAND9_REG_I_ON_5                       0x3B
#define EXPAND9_REG_OFF_5                        0x3C
#define EXPAND9_REG_T_RISE_5                     0x3D
#define EXPAND9_REG_T_FALL_5                     0x3E
#define EXPAND9_REG_T_ON_6                       0x3F
#define EXPAND9_REG_I_ON_6                       0x40
#define EXPAND9_REG_OFF_6                        0x41
#define EXPAND9_REG_T_RISE_6                     0x42
#define EXPAND9_REG_T_FALL_6                     0x43
#define EXPAND9_REG_T_ON_7                       0x44
#define EXPAND9_REG_I_ON_7                       0x45
#define EXPAND9_REG_OFF_7                        0x46
#define EXPAND9_REG_T_RISE_7                     0x47
#define EXPAND9_REG_T_FALL_7                     0x48
#define EXPAND9_REG_T_ON_8                       0x49
#define EXPAND9_REG_I_ON_8                       0x4A
#define EXPAND9_REG_OFF_8                        0x4B
#define EXPAND9_REG_T_ON_9                       0x4C
#define EXPAND9_REG_I_ON_9                       0x4D
#define EXPAND9_REG_OFF_9                        0x4E
#define EXPAND9_REG_T_ON_10                      0x4F
#define EXPAND9_REG_I_ON_10                      0x50
#define EXPAND9_REG_OFF_10                       0x51
#define EXPAND9_REG_T_ON_11                      0x52
#define EXPAND9_REG_I_ON_11                      0x53
#define EXPAND9_REG_OFF_11                       0x54
#define EXPAND9_REG_T_ON_12                      0x55
#define EXPAND9_REG_I_ON_12                      0x56
#define EXPAND9_REG_OFF_12                       0x57
#define EXPAND9_REG_T_RISE_12                    0x58
#define EXPAND9_REG_T_FALL_12                    0x59
#define EXPAND9_REG_T_ON_13                      0x5A
#define EXPAND9_REG_I_ON_13                      0x5B
#define EXPAND9_REG_OFF_13                       0x5C
#define EXPAND9_REG_T_RISE_13                    0x5D
#define EXPAND9_REG_T_FALL_13                    0x5E
#define EXPAND9_REG_T_ON_14                      0x5F
#define EXPAND9_REG_I_ON_14                      0x60
#define EXPAND9_REG_OFF_14                       0x61
#define EXPAND9_REG_T_RISE_14                    0x62
#define EXPAND9_REG_T_FALL_14                    0x63
#define EXPAND9_REG_T_ON_15                      0x64
#define EXPAND9_REG_I_ON_15                      0x65
#define EXPAND9_REG_OFF_15                       0x66
#define EXPAND9_REG_T_RISE_15                    0x67
#define EXPAND9_REG_T_FALL_15                    0x68
#define EXPAND9_REG_HIGH_INPUT_B                 0x69
#define EXPAND9_REG_HIGH_INPUT_A                 0x6A
#define EXPAND9_REG_RESET                        0x7D
#define EXPAND9_REG_TEST_1                       0x7E
#define EXPAND9_REG_TEST_2                       0x7F

/*! @} */ // expand9_reg

/**
 * @defgroup expand9_set Expand 9 Registers Settings
 * @brief Settings for registers of Expand 9 Click driver.
 */

/**
 * @addtogroup expand9_set
 * @{
 */

/**
 * @brief Expand 9 description setting.
 * @details Specified setting for description of Expand 9 Click driver.
 */
#define EXPAND9_CHANGE                           0x01
#define EXPAND9_FALLING                          0x02
#define EXPAND9_RISING                           0x03
#define EXPAND9_FREQ_DIV_1                       0x01
#define EXPAND9_FREQ_DIV_2                       0x02
#define EXPAND9_FREQ_DIV_3                       0x03
#define EXPAND9_FREQ_DIV_4                       0x04
#define EXPAND9_FREQ_DIV_5                       0x05
#define EXPAND9_FREQ_DIV_6                       0x06
#define EXPAND9_FREQ_DIV_7                       0x07
#define EXPAND9_LED_MODE_LINEAR                  0x00
#define EXPAND9_LED_MODE_LOGARITHMIC             0x01

/**
 * @brief Expand 9 device address setting.
 * @details Specified setting for device slave address selection of
 * Expand 9 Click driver.
 */
#define EXPAND9_SET_DEV_ADDR_GND                 0x3E
#define EXPAND9_SET_DEV_ADDR_VCC                 0x70


/**
 * @brief Expand 9 Click direction value data.
 * @details Predefined enum values for driver direction values.
 */
typedef enum
{
   DIRECTION_INPUT = 0,
   DIRECTION_OUTPUT =  1

} expand9_direction_value_t;

/**
 * @brief Expand 9 Click channel state value data.
 * @details Predefined enum values for driver channel state values.
 */
typedef enum
{
   CH_OUTPUT_OFF = 0,
   CH_OUTPUT_ON =  1

} expand9_ch_state_value_t;

/**
 * @brief Read a sequence of bytes from a IO Expander register
 */
esp_err_t expander_register_read(uint8_t reg_addr, uint8_t *rx_buf, size_t rx_len);

/**
 * @brief Write a sequence of bytes to a IO Expander register
 */
esp_err_t expander_register_write(uint8_t reg_addr, uint8_t *tx_buf, uint8_t tx_len);

void expander_init (void);

/**
 * @brief Expand 9 software reset function.
 * @details This function performs software reset of the SX1509QB,
 * World’s Lowest Voltage Level Shifting GPIO with LED Driver and Keypad Engine
 * on the Expand 9 click board™.
 */
void expand9_soft_reset();

/**
 * @brief Expand 9 set channel direction function.
 * @details This function sets desired direction  of the desired channel of the SX1509QB,
 * World’s Lowest Voltage Level Shifting GPIO with LED Driver and Keypad Engine
 * on the Expand 9 click board™.
 */
void expand9_set_ch_direction (uint8_t ch_pos, expand9_direction_value_t direction );

void expand9_set_ch_output_state (uint8_t ch_pos, expand9_ch_state_value_t ch_state );

uint8_t expand9_get_ch_output_state (uint8_t ch_pos);

void expand9_write_ch (uint8_t ch_pos, expand9_ch_state_value_t ch_state );

void expand9_read_ch(uint8_t ch_pos, expand9_ch_state_value_t *ch_state );

void expand9_led_driver_config (uint8_t ch_pos, uint8_t freq_div, uint8_t lin_log );

void expand9_set_intensity (uint8_t ch_pos, uint8_t intensity );

void expand9_set_all_ch_output();

void expand9_set_all_ch_input();

void expand9_lcd_send_cmd ();

void expand9_lcd_send_data ();

void pwm_gpio_initialize(void);

void oscio_pwm_init(void);

void light_task(void *pvParameter);
#endif // EXPANDER_H