#ifndef __DRV_OLED_H__
#define __DRV_OLED_H__
#define DRV_OLED_USE_TWI
#include <stdint.h>
#include "string.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "macros_common.h"

#ifdef DRV_OLED_USE_TWI
#include "nrf_drv_twi.h"
#else
#include "drv_i2c.h"
typedef struct
{
    uint32_t            scl;                 ///< SCL pin number.
    uint32_t            sda;                 ///< SDA pin number.
} nrf_drv_twi_config_t;
#endif

typedef struct
{
  uint8_t                      twi_addr;          ///< TWI address.
	nrf_drv_twi_t        const * p_twi_instance;
	nrf_drv_twi_config_t const * p_twi_cfg;         ///< The TWI configuration to use while the driver is enabled.
} drv_oled_init_t;

uint32_t drv_oled_init(drv_oled_init_t * p_params);
void drv_oled_test(void);

//OLED控制用函数
void OLED_Display_On(void);
void OLED_Display_Off(void);	
void OLED_SetShowInverse(uint8_t Mode);
void OLED_Refresh_Gram(void);
void OLED_Clear(void);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void LCDDrawCir(uint16_t x0,uint16_t y0,uint8_t r,uint8_t orFill);
void OLED_Draw_Line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);

void OLED_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   				//画矩形
void OLED_ShowChar(int8_t x,int8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowOneHZ(uint8_t x,uint8_t y,uint8_t OFFSet,uint8_t size,uint8_t mode);
void OLED_ShowNumHZ(uint8_t x,uint8_t y,uint8_t const *OffsetTab,uint8_t size,uint8_t HZNum,uint8_t mode);
void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t BmpXSize,uint8_t BmpYSize,uint8_t *buf,uint8_t ClrFill);
void OLED_ShowMixStr(int8_t x,int8_t y,uint8_t width,uint8_t height,uint8_t *str,uint8_t size,uint8_t mode);

#endif
