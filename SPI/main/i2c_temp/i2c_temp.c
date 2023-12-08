#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_types.h" // for `shorts`

#define SDA_GPIO 26
#define SCL_GPIO 25
#define LM75A_ADDRESS 0X48

void i2c_temp_main()
{   // required to use PU-resistors for i2c on both SDA&SCL, to avoid hardware, we can use internal pullups. 
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000}; // note master/slave struct distinction
    i2c_param_config(I2C_NUM_0, &i2c_config);

    // remember: 2 i2c buses on ESP32. 0s are for slave config
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0); 
  
    
    uint8_t raw[2];
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);

    // why writing if want read temp?
    // this is essentially sending a chip command to tell it to prepare to be read. 
    // ack_en is getting an acknowledgement response back confirming successful command send
    i2c_master_write_byte(cmd_handle, (LM75A_ADDRESS << 1) | I2C_MASTER_READ, true);
    
    // read 2 bytes from i2s buffer into raw[] and get acknowledgement
    i2c_master_read(cmd_handle, (uint8_t *)&raw, 2, I2C_MASTER_ACK);
    i2c_master_stop(cmd_handle);

    // at this stage, we've added a series of instructions to our command handle, and now we are executing those instructions.
    i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);

    // if forget, check: 
    //  1. https://learnesp32.com/videos/i2c-spi-uart/i2c-introduction
    //  2. https://learnesp32.com/videos/i2c-spi-uart/i2c-lm75a
    bool isNeg = false;
    if (raw[0] & 0x80) // check if the MSB is set
    {
        isNeg = true;
        raw[0] = raw[0] & 0x7f; 
    }

    int16_t data = (raw[0] << 8 | raw[1]) >> 5;
    float temperature = (data * 0.125) * (isNeg? -1 : 1);
    printf("temperature %f\n", temperature);
}