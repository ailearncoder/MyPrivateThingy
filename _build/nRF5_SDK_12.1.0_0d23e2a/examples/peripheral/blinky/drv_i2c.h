#ifndef __DEV_I2C_H__
#define __DEV_I2C_H__
#include <stdint.h>
#include "nrf_gpio.h"
typedef struct
{
	  uint8_t            scl;               ///< SCL pin number.
    uint8_t            sda;               ///< SDA pin number.
}drv_i2c_init_t;

void i2c_init(drv_i2c_init_t * p_params);
void i2c_start(void);
void i2c_stop(void);
void i2c_write_byte(uint8_t i2c_byte);
#endif
