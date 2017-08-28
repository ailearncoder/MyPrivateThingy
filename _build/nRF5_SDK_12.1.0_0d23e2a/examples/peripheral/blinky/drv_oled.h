#ifndef __DRV_OLED_H__
#define __DRV_OLED_H__
#include <stdint.h>
#include "drv_i2c.h"
#include "nrf_delay.h"
typedef struct
{
    uint32_t            scl;                 ///< SCL pin number.
    uint32_t            sda;                 ///< SDA pin number.
} nrf_drv_twi_config_t;

typedef struct
{
  uint8_t                      twi_addr;          ///< TWI address.
	nrf_drv_twi_config_t const * p_twi_cfg;         ///< The TWI configuration to use while the driver is enabled.
} drv_oled_init_t;

uint32_t drv_oled_init(drv_oled_init_t * p_params);
void drv_oled_test(void);
#endif
