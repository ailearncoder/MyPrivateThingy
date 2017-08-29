#include "m_oled.h"
#include "pca20020.h"
uint32_t m_oled_init(m_oled_init_t * p_params)
{
	uint32_t err_code;
	drv_oled_init_t init_params;
	
	static const nrf_drv_twi_config_t twi_cfg =
	{
			.scl                = TWI_SCL_EXT,
			.sda                = TWI_SDA_EXT,
			.frequency          = NRF_TWI_FREQ_400K,
	};
	init_params.twi_addr=0x78;
	init_params.p_twi_instance=p_params->p_twi_instance;
	init_params.p_twi_cfg=&twi_cfg;
	err_code=drv_oled_init(&init_params);
	RETURN_IF_ERROR(err_code);
	OLED_Clear();
	return NRF_SUCCESS;
}
void m_oled_welcome()
{
	OLED_ShowString(0,0,"Hello Panpan!",16);
}
