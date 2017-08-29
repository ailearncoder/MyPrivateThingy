#include "drv_oled.h"
#include "oled_font.h"

drv_oled_init_t m_params;

static uint32_t Write_IIC_Command(uint8_t command)
{
	#ifdef DRV_OLED_USE_TWI
	ret_code_t err_code;
	uint8_t reg[2] = {0x00, command};
	err_code = nrf_drv_twi_tx(m_params.p_twi_instance, m_params.twi_addr>>1, reg, 2, false);
	RETURN_IF_ERROR(err_code);
	#else
	i2c_start();
  i2c_write_byte(m_params.twi_addr);      //Slave address,SA0=0
  i2c_write_byte(0x00);			              //write command
  i2c_write_byte(command); 
  i2c_stop();
	#endif
	return NRF_SUCCESS;
}
/**********************************************
// IIC Write Data
**********************************************/
static void Write_IIC_Data(uint8_t data)
{
	#ifdef DRV_OLED_USE_TWI
	ret_code_t err_code;
	/* Writing to LM75B_REG_CONF "0" set temperature sensor in NORMAL mode. */
	uint8_t reg[2] = {0x40, data};
	err_code = nrf_drv_twi_tx(m_params.p_twi_instance, m_params.twi_addr>>1, reg, 2, false);
	APP_ERROR_CHECK(err_code);
	#else
   i2c_start();
   i2c_write_byte(m_params.twi_addr);			
   i2c_write_byte(0x40);			//write data
   i2c_write_byte(data);
   i2c_stop();
	#endif
}
uint32_t drv_oled_init(drv_oled_init_t * p_params)
{
	m_params.twi_addr       = p_params->twi_addr;
	m_params.p_twi_instance = p_params->p_twi_instance;
	m_params.p_twi_cfg      = p_params->p_twi_cfg;	
	
	#ifdef DRV_OLED_USE_TWI
	ret_code_t err_code;
	err_code = nrf_drv_twi_init(m_params.p_twi_instance, m_params.p_twi_cfg, NULL, NULL);
	RETURN_IF_ERROR(err_code);
	nrf_drv_twi_enable(m_params.p_twi_instance);
	#else
	drv_i2c_init_t  drv_i2c_init_params;	
	drv_i2c_init_params.scl=(p_params->p_twi_cfg)->scl;
	drv_i2c_init_params.sda=(p_params->p_twi_cfg)->sda;
	i2c_init(&drv_i2c_init_params);
	#endif
	
	
	err_code = Write_IIC_Command(0xAE);   //display off
	RETURN_IF_ERROR(err_code);
	err_code = Write_IIC_Command(0x20);	//Set Memory Addressing Mode	
	err_code = Write_IIC_Command(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	err_code = Write_IIC_Command(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	err_code = Write_IIC_Command(0xc8);	//Set COM Output Scan Direction
	err_code = Write_IIC_Command(0x00);	//---set low column address
	err_code = Write_IIC_Command(0x10);	//---set high column address
	err_code = Write_IIC_Command(0x40);	//--set start line address
	err_code = Write_IIC_Command(0x81);	//--set contrast control register
	err_code = Write_IIC_Command(0x7f);
	err_code = Write_IIC_Command(0xa1);	//--set segment re-map 0 to 127
	err_code = Write_IIC_Command(0xa6);	//--set normal display
	err_code = Write_IIC_Command(0xa8);	//--set multiplex ratio(1 to 64)
	err_code = Write_IIC_Command(0x3F);	//
	err_code = Write_IIC_Command(0xa4);	//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	err_code = Write_IIC_Command(0xd3);	//-set display offset
	err_code = Write_IIC_Command(0x00);	//-not offset
	err_code = Write_IIC_Command(0xd5);	//--set display clock divide ratio/oscillator frequency
	err_code = Write_IIC_Command(0xf0);	//--set divide ratio
	err_code = Write_IIC_Command(0xd9);	//--set pre-charge period
	err_code = Write_IIC_Command(0x22); //
	err_code = Write_IIC_Command(0xda);	//--set com pins hardware configuration
	err_code = Write_IIC_Command(0x12);
	err_code = Write_IIC_Command(0xdb);	//--set vcomh
	err_code = Write_IIC_Command(0x20);	//0x20,0.77xVcc
	err_code = Write_IIC_Command(0x8d);	//--set DC-DC enable
	err_code = Write_IIC_Command(0x14);	//
	err_code = Write_IIC_Command(0xaf);	//--turn on oled panel
	
	return err_code;
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

#define OLED_CMD  1
#define OLED_DATA 0
uint8_t OLED_GRAM[128][8];
//向SSD1106写入一个字节,IO模拟
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
static void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
	if(cmd)
	  Write_IIC_Command(dat);
	else
	  Write_IIC_Data(dat);
}
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		
//设置显示方式
//1正常显示  0反白显示
void OLED_SetShowInverse(uint8_t Mode)
{
	if(Mode)
		OLED_WR_Byte(0XA6&~(1<<0),OLED_CMD);		
	else
		OLED_WR_Byte(0XA6|(1<<0),OLED_CMD);//设置显示方式;bit0:1,反相显示;0,正常显示	
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][i] = 0;
			OLED_WR_Byte(0,OLED_DATA); 
		}
	} 
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
			OLED_DrawPoint(x,y,dot);
	}													    
}

//OLED刷新缓存区
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
	}
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)
		return;//超出范围了.
	pos  = y/8;
	bx   = y%8;
	temp = 1<<(bx);
	if(t)
		OLED_GRAM[x][pos] |= temp;
	else 
		OLED_GRAM[x][pos] &= ~temp;	
}
//OLED划线
void OLED_Draw_Line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{   
		OLED_DrawPoint(uRow,uCol,1);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
void OLED_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)		   				//画矩形
{
	OLED_Draw_Line(x1,y1,x2,y1);
	OLED_Draw_Line(x1,y1,x1,y2);
	OLED_Draw_Line(x1,y2,x2,y2);
	OLED_Draw_Line(x2,y1,x2,y2);
}

//画圆函数
void LCDDrawCir(uint16_t x0,uint16_t y0,uint8_t r,uint8_t orFill)
{
	int i;
	int a;//对象限的分割线
	int b;//
	int di;
	a=0;b=r;
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		if(orFill)
		{
			for(i=x0;i<x0+a;i++)
			{
				OLED_DrawPoint(i,y0-a,1);          //1   
				OLED_DrawPoint(i,y0-b,1);
			}
			for(i=x0+a;i<x0+b;i++)
			{
				OLED_DrawPoint(i,y0-a,1);       //2         
			}
			for(i=x0+a;i<x0+b;i++)
			{
				OLED_DrawPoint(i,y0+a,1);         //3
			}
			for(i=x0;i<x0+a;i++)
			{
				OLED_DrawPoint(i,y0+a,1);         //4
				OLED_DrawPoint(i,y0+b,1);
			}
			for(i=x0-a;i<x0;i++)
			{
				OLED_DrawPoint(i,y0+a,1);         //5
				OLED_DrawPoint(i,y0+b,1);
			}
			for(i=x0-b;i<=x0-a;i++)
			{
				OLED_DrawPoint(i,y0+a,1);         //6
			}
			for(i=x0-b;i<x0-a;i++)
			{
				OLED_DrawPoint(i,y0-a,1);         //7
			}
			for(i=x0-a;i<x0;i++)
			{
				OLED_DrawPoint(i,y0-a,1);      //8
				OLED_DrawPoint(i,y0-b,1);
			}
		}
		else
		{
			OLED_DrawPoint(x0+a,y0-b,1);             //5
			OLED_DrawPoint(x0+b,y0-a,1);             //0           
			OLED_DrawPoint(x0+b,y0+a,1);             //4               
			OLED_DrawPoint(x0+a,y0+b,1);             //6 
			OLED_DrawPoint(x0-a,y0+b,1);             //1       
			OLED_DrawPoint(x0-b,y0+a,1);             
			OLED_DrawPoint(x0-a,y0-b,1);             //2             
			OLED_DrawPoint(x0-b,y0-a,1);             //7
		}
		a++;
		//使用Bresenham算法画圆
		if(di<0)
			di += 4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		} 						    
	}
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(int8_t x,int8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      	
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)
			temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)
			temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)
			temp=asc2_2412[chr][t];	//调用2412字体
		else 
			return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				if(mode==0XFF)
					OLED_DrawPoint(x,y,0);  //清除显示
				else
					OLED_DrawPoint(x,y,mode);
			}
			else
				OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    } 
}
//m^n函数
uint32_t OLED_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)
		result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,mode);
				continue;
			}
			else 
				enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,mode); 
	}
} 
//显示一个标准字符串不包含中文汉字
//输入参数：坐标位置，字符串指针，字符大小
void OLED_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size)
{
	uint8_t *p = (uint8_t *)chr;
	while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2)))
		{
			x=0;
			y+=size;
		}
        if(y>(64-size))
		{
			y=x=0;
			OLED_Clear();
		}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    } 
}
//显示单个汉字
//坐标值，汉字在数组中的偏移量，字体大小，显示模式
//mode:0xff清空
void OLED_ShowOneHZ(uint8_t x,uint8_t y,uint8_t OFFSet,uint8_t size,uint8_t mode)
{
	uint8_t *pFont=0;
	uint8_t temp,t,t1;
	uint8_t y0 = y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size);		//得到字体一个字符对应点阵集所占的字节数		 
    if(size!=12&&size!=16&&size!=24)
		return;
	switch(size)
	{
		case 12:{pFont = (uint8_t *)&HZLib12X12[OFFSet][0];}break;
		case 16:{pFont = (uint8_t *)&HZLib16X16[OFFSet][0];}break;
		case 24:{pFont = (uint8_t *)&HZLib24X24[OFFSet][0];}break;
		default:return;
	}
	for(t=0;t<csize;t++)
    {
		temp = *pFont;
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				if(mode==0XFF)
					OLED_DrawPoint(x,y,0);  //清除显示
				else
					OLED_DrawPoint(x,y,mode);
			}
			else if(mode==0)
				OLED_DrawPoint(x,y,!mode);  //反白显示
			temp <<= 1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}
		pFont += 1;		
    } 
}
#if 0
uint8_t const HZQueue12[]= "顾成龙";   //数组下表位置和字体在和中文字库中的存储位置一样，
uint8_t const HZQueue16[]= "℃通道网络系统状态";  
uint8_t const HZQueue24[]= "我的滑板鞋";
//显示字符汉字混合
//输入参数：坐标，显示宽度，显示高度，要显示的字符串，字体大小，显示模式
void OLED_ShowMixStr(int8_t x,int8_t y,uint8_t width,uint8_t height,uint8_t *str,uint8_t size,uint8_t mode)
{
	uint8_t i;
	int8_t x0 = x;
	uint16_t ByteLen = 0; //汉字顺序表中汉字的字节长度
	int8_t y0 = y;
	uint8_t bHz = 0;
	uint8_t Offset;
	while(*str!=0)
	{
		if(!bHz)
		{
			if(*str>0x7e)
				bHz=1;//中文 
			else
			{
				if(x>(x0+width+30-size/2))//换行  加30为屏幕上显示不下的余量值
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))
					break;		//越界返回      
		        if(*str==13)	//换行符号
		        {
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else
					OLED_ShowChar(x,y,*str,size,mode);
				str++; 
		        x+=size/2; //字符,为全字的一半 
			}
		}
		else
		{
			bHz=0;//有汉字库
			if(x>(x0+width-size))//换行
			{
				y += size;
				x = x0;
			}
	        if(y>(y0+height-size))
				break;		//越界返回 
			switch(size)
			{
				case 12:
				{
					ByteLen = strlen(HZQueue12)*2;
					for(i=0;i<ByteLen;i+=2)  //一个中文字符占用两个字节
					{
						if(*str==HZQueue12[i])  //寻找字体
						{
							Offset = i/2;  //得到偏移表中实际的字体位置
						}
					}
				}break;
				case 16:
				{
					ByteLen = strlen(HZQueue16)*2;
					for(i=0;i<ByteLen;i+=2)  //一个中文字符占用两个字节
					{
						if(*str==HZQueue16[i])  //寻找字体
						{
							Offset = i/2;  //得到偏移表中实际的字体位置
						}
					}				
				}break;
				case 24:
				{
					ByteLen = strlen(HZQueue24)*2;
					for(i=0;i<ByteLen;i+=2)  //一个中文字符占用两个字节
					{
						if(*str==HZQueue24[i])  //寻找字体
						{
							Offset = i/2;  //得到偏移表中实际的字体位置
						}
					}
				}break;
				default:break;
			}
			
			OLED_ShowOneHZ(x,y,Offset,size,mode);
	        str +=2;
	        x += size;	//下一个汉字偏移	  		
		}
	}	
}
#endif
//显示几个汉字
//坐标，汉字在内存中的偏移表，字体大小,汉字个数显示模式
void OLED_ShowNumHZ(uint8_t x,uint8_t y,uint8_t const *OffsetTab,uint8_t size,uint8_t HZNum,uint8_t mode)
{
	uint8_t i;
	uint8_t x0=x;
	uint8_t Len = HZNum;
	for(i=0;i<Len;i++)
	{
		OLED_ShowOneHZ(x0,y,OffsetTab[i],size,1);
		x0 += size;
	}
}
//OLED显示一个简图在指定位置
//输入参数：坐标，显示区域大小，位图存贮指针，显示模式(是否填充)
void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t BmpXSize,uint8_t BmpYSize,uint8_t *buf,uint8_t ClrFill)
{
	uint16_t temp,t,t1;
	uint16_t y0=y;		
	uint16_t Area_Size = BmpXSize*BmpYSize/8;  //总的字节大小
    for(t=0;t<Area_Size;t++)
    {
		temp=buf[t];  //得到图片的一个字节数据	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				OLED_DrawPoint(x,y,ClrFill); 
			else 
				OLED_DrawPoint(x,y,0);		   
			temp<<=1;
			y++;
			if((y-y0)==BmpYSize)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }  
}
