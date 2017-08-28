#include "drv_oled.h"

drv_oled_init_t m_params;

static void Write_IIC_Command(uint8_t command)
{
	i2c_start();
  i2c_write_byte(m_params.twi_addr);      //Slave address,SA0=0
  i2c_write_byte(0x00);			              //write command
  i2c_write_byte(command); 
  i2c_stop();
}
/**********************************************
// IIC Write Data
**********************************************/
static void Write_IIC_Data(uint8_t data)
{
   i2c_start();
   i2c_write_byte(m_params.twi_addr);			
   i2c_write_byte(0x40);			//write data
   i2c_write_byte(data);
   i2c_stop();
}
uint32_t drv_oled_init(drv_oled_init_t * p_params)
{
	drv_i2c_init_t  drv_i2c_init_params;
	
	m_params.twi_addr=p_params->twi_addr;
	m_params.p_twi_cfg=p_params->p_twi_cfg;
	
	drv_i2c_init_params.scl=(p_params->p_twi_cfg)->scl;
	drv_i2c_init_params.sda=(p_params->p_twi_cfg)->sda;
	
	i2c_init(&drv_i2c_init_params);
	
	Write_IIC_Command(0xAE);   //display off
	Write_IIC_Command(0x20);	//Set Memory Addressing Mode	
	Write_IIC_Command(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Write_IIC_Command(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	Write_IIC_Command(0xc8);	//Set COM Output Scan Direction
	Write_IIC_Command(0x00);	//---set low column address
	Write_IIC_Command(0x10);	//---set high column address
	Write_IIC_Command(0x40);	//--set start line address
	Write_IIC_Command(0x81);	//--set contrast control register
	Write_IIC_Command(0x7f);
	Write_IIC_Command(0xa1);	//--set segment re-map 0 to 127
	Write_IIC_Command(0xa6);	//--set normal display
	Write_IIC_Command(0xa8);	//--set multiplex ratio(1 to 64)
	Write_IIC_Command(0x3F);	//
	Write_IIC_Command(0xa4);	//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Write_IIC_Command(0xd3);	//-set display offset
	Write_IIC_Command(0x00);	//-not offset
	Write_IIC_Command(0xd5);	//--set display clock divide ratio/oscillator frequency
	Write_IIC_Command(0xf0);	//--set divide ratio
	Write_IIC_Command(0xd9);	//--set pre-charge period
	Write_IIC_Command(0x22); //
	Write_IIC_Command(0xda);	//--set com pins hardware configuration
	Write_IIC_Command(0x12);
	Write_IIC_Command(0xdb);	//--set vcomh
	Write_IIC_Command(0x20);	//0x20,0.77xVcc
	Write_IIC_Command(0x8d);	//--set DC-DC enable
	Write_IIC_Command(0x14);	//
	Write_IIC_Command(0xaf);	//--turn on oled panel
	
	return 0;
}
/********************************************
// fill_Picture
********************************************/
static void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		Write_IIC_Command(0xb0+m);		//page0-page1
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n=0;n<128;n++)
		{
				Write_IIC_Data(fill_Data);
		}
	}
}
void drv_oled_test()
{
	fill_picture(0x00);
	nrf_delay_ms(1000);
	fill_picture(0xFF);
	nrf_delay_ms(1000);
}	
