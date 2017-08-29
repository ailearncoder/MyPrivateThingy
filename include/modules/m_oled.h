#ifndef __OLED_H__
#define __OLED_H__
#include <stdint.h>
#include "drv_oled.h"
typedef struct
{
    nrf_drv_twi_t         const * p_twi_instance;
}m_oled_init_t;
uint32_t m_oled_init(m_oled_init_t * p_params);
void m_oled_welcome(void);

#endif
