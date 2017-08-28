#include "drv_i2c.h"

#define high 1
#define low 0 
#define SCL m_params.scl
#define SDA m_params.sda
//#define DRV_I2C_DELAY

static drv_i2c_init_t m_params;

void i2c_init(drv_i2c_init_t * p_params)
{
	m_params.scl=p_params->scl;
	m_params.sda=p_params->sda;
}

#ifdef DRV_I2C_DELAY 
#include "nrf_delay.h"
void delay()
{
	nrf_delay_us(10);
}	
#endif	

void i2c_start()
{
	 nrf_gpio_cfg_output(SCL);
	 nrf_gpio_cfg_output(SDA);
	 nrf_gpio_pin_write(SCL,high);
   //SCL = high;
   #ifdef DRV_I2C_DELAY 
	 delay(); 
	 #endif	
	 nrf_gpio_pin_write(SDA,high);	
   //SDA = high;
   #ifdef DRV_I2C_DELAY 
	 delay(); 
	 #endif	
	 nrf_gpio_pin_write(SDA,low);
   //SDA = low;
	 #ifdef DRV_I2C_DELAY
 	 delay(); 
	 #endif
	 nrf_gpio_pin_write(SCL,low);
   //SCL = low;
	 #ifdef DRV_I2C_DELAY 
	 delay(); 
	 #endif
}

void i2c_stop()
{
	 nrf_gpio_cfg_output(SCL);
	 nrf_gpio_cfg_output(SDA);
   nrf_gpio_pin_write(SCL,low);
   #ifdef DRV_I2C_DELAY 
	 delay(); 
	 #endif
   nrf_gpio_pin_write(SDA,low);
   #ifdef DRV_I2C_DELAY 
	 delay(); 
	 #endif
   nrf_gpio_pin_write(SCL,high);
   #ifdef DRV_I2C_DELAY 
	 delay(); 
	 #endif
	 nrf_gpio_cfg_input(SDA,NRF_GPIO_PIN_PULLUP);
   //nrf_gpio_pin_write(SDA,high);
   #ifdef DRV_I2C_DELAY 
	 delay(); 
	 #endif
}

void i2c_write_byte(uint8_t i2c_byte)
{
	nrf_gpio_cfg_output(SCL);
	nrf_gpio_cfg_output(SDA);
	unsigned char i;
	for(i=0;i<8;i++)		
	{
		if(i2c_byte & 0x80)		//1?0?
		{		  
			nrf_gpio_pin_write(SDA,high);
		}
		else 
		{		  
			nrf_gpio_pin_write(SDA,low);
		}
		#ifdef DRV_I2C_DELAY 
		delay(); 
		#endif
		nrf_gpio_pin_write(SCL,high);
		#ifdef DRV_I2C_DELAY 
		delay(); 
		#endif
		nrf_gpio_pin_write(SCL,low);
		#ifdef DRV_I2C_DELAY 
		delay(); 
		#endif
		i2c_byte<<=1;			//loop
	}
	nrf_gpio_cfg_input(SDA,NRF_GPIO_PIN_PULLUP);
	//nrf_gpio_pin_write(SDA,high);
	#ifdef DRV_I2C_DELAY 
	delay(); 
	#endif
	nrf_gpio_pin_write(SCL,high);	
	#ifdef DRV_I2C_DELAY 
	delay(); 
	#endif
	while(nrf_gpio_pin_read(SDA))
		;//wait ack
	nrf_gpio_pin_write(SCL,low);
	#ifdef DRV_I2C_DELAY 
	delay(); 
	#endif
}
