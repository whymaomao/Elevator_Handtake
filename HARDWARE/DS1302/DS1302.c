#include "DS1302.h"
#include "Delay.h"
#include "string.h"
///////////////配置函数////////////////////
//开启写保护函数
static void Protect()
{
		DS1302_Single_Write(CTRL,LOCK);
}

//解除写保护函数
static void UnProtect()
{
		DS1302_Single_Write(CTRL,UNLOCK);
}

//DS1302启动函数
static void Clock_on()
{
		DS1302_Single_Write(SEC,CLKON);
}

//DS1302停止函数
static void Clock_Off()
{
		DS1302_Single_Write(SEC,CLKOFF);
}


static void DS_IO_IN(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOG,&GPIO_InitStructure);
}

static void DS_IO_OUT(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_Init(GPIOG,&GPIO_InitStructure);
		
}
static void DS1302_GPIO_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);  //打开时钟模块
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_Init(GPIOG, &GPIO_InitStructure);
		GPIO_ResetBits(DS_Port,DS_SCLK);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOG,&GPIO_InitStructure);
		
	
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOG,&GPIO_InitStructure);		
}

void DS1302_Byte_Write(unsigned char dat) //发送一个字节到DS1302通信总线上
{
		unsigned char i = 0;
		//通信写数据,数据在SCLK上升沿
//		DS_SCLK = 0;
//		delay_us(2);
		DS_IO_OUT();
		GPIO_ResetBits(DS_Port,DS_SCLK);
		for(i = 0; i < 8; i ++)
		{
				
				if(dat&0x01)
							GPIO_SetBits(DS_Port,DS_IO);
				else 	GPIO_ResetBits(DS_Port,DS_IO);
				delay_us(2);
				GPIO_SetBits(DS_Port,DS_SCLK);
				
				delay_us(5);
				GPIO_ResetBits(DS_Port,DS_SCLK);
				dat >>= 1;
		}
		GPIO_SetBits(DS_Port,DS_IO);  //最后确保释放IO引脚
		
}	

unsigned char DS1302_Byte_Read(void)   //从DS1302通信总线上读取一个字节
{
	//DS1302 在时钟线下降沿读数据
	//写地址字节和读数据字节都是从LSB开始
	unsigned char i;
	unsigned char temp = 0x00;
	DS_IO_IN();
	GPIO_ResetBits(DS_Port,DS_SCLK);
	for(i = 0; i < 8; i ++)
	{
		
			temp >>= 1;
			
			if(GPIO_ReadInputDataBit(DS_Port,DS_IO)==1)
			{
					temp = temp|0x80;
			}
			delay_us(5);
			GPIO_SetBits(DS_Port,DS_SCLK);
			delay_us(5);
			
			
			GPIO_ResetBits(DS_Port,DS_SCLK);
	}
	return temp;
}
//用单次模式向DS1302的某一寄存器地址写入一字节数据
void DS1302_Single_Write(unsigned char reg, unsigned char dat)
{
		
		//只有在SCLK为低电平时，才能将RST置为高电平
		GPIO_ResetBits(DS_Port,DS_CE);
		GPIO_ResetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_CE);
		delay_us(2);
		DS1302_Byte_Write((reg)|0x80);//发送写寄存器指令，左移空出来最低位读写位
		DS1302_Byte_Write(dat);
		GPIO_SetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_ResetBits(DS_Port,DS_CE);
		
		delay_us(1);
}
//用单次模式从DS1302的某一寄存器读取一字节数据，返回值为读取的字节数据
unsigned char DS1302_Single_Read(unsigned char reg)
{
		unsigned char dat;
		GPIO_ResetBits(DS_Port,DS_CE);
		GPIO_ResetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_CE);
		delay_us(2);
		DS1302_Byte_Write((reg)|0x81);
		dat = DS1302_Byte_Read();
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_ResetBits(DS_Port,DS_CE);
		
		return dat;
}
//用突发模式向DS1302连续写8个寄存器数据,待写入数据指针为dat
void DS1302_Burst_Write(unsigned char* dat)
{
		unsigned char i;
		GPIO_ResetBits(DS_Port,DS_CE);
		GPIO_ResetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_CE);
		delay_us(2);
		DS1302_Byte_Write(BRUST);
		for( i = 0; i < 8; i ++)
		{
				DS1302_Byte_Write(dat[i]);
		}
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_ResetBits(DS_Port,DS_CE);
}
//用突发模式从DS1302连续读取8个寄存器的数据，数据接收指针为dat
void DS1302_Burst_Read(unsigned char *dat)
{
		unsigned char i;
		GPIO_ResetBits(DS_Port,DS_CE);
		GPIO_ResetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_CE);
		delay_us(2);
		DS1302_Byte_Write(BRUST|0x81);
		for(i = 0; i < 8; i ++)
		{
				dat[i] = DS1302_Byte_Read();
		}
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_ResetBits(DS_Port,DS_CE);
		
}


static unsigned char BCD2Dec(unsigned char num)
{
		unsigned char Dec= 0;
		Dec = num%16 + num/16*10;
		return Dec;
}

//获取实时时间
void GetRealTime(sTime *time)
{
		unsigned char buf[8];
		DS1302_Burst_Read(buf);
		time->year = BCD2Dec(buf[6]) ;
		time->mon = BCD2Dec(buf[4]);
		time->day = BCD2Dec(buf[3]);
		time->hour = BCD2Dec(buf[2]);
		time->min = BCD2Dec(buf[1]);
		time->sec = BCD2Dec(buf[0]);
		time->week = BCD2Dec(buf[5]);		
}


//设置实时时间
void SetRealTime(sTime *time)
{
		unsigned char buf[8];
		buf[7] = 0;
		buf[6] = time->year;
		buf[5] = time->week;
		buf[4] = time->mon;
		buf[3] = time->day;
		buf[2] = time->hour;
		buf[1] = time->min;
		buf[0] = time->sec;
		DS1302_Burst_Write(buf);

}

static void DS1302_Reg_Init(void)
{
		unsigned char dat;
		sTime InitTime = 
		{
				0x16,0x10,0x08,0x12,0x30,0x00,0x02
		};  //2016年10月8日12:30:00星期二
			GPIO_ResetBits(DS_Port,DS_CE);
			GPIO_ResetBits(DS_Port,DS_SCLK);
			UnProtect();
			SetRealTime(&InitTime);
			Protect();
}

void DS1302_SetTime(sTime* time)
{
		GPIO_ResetBits(DS_Port,DS_CE);
		GPIO_ResetBits(DS_Port,DS_SCLK);
		UnProtect();
		SetRealTime(time);
		Protect();
}
//DS1302初始化
void DS1302_Init(void)
{
		DS1302_GPIO_Init();
		DS1302_Reg_Init();
}


 char* date2string(sTime*time)
{
		static char date[20];
	sprintf(date,"20%0.2d/%0.2d/%0.2d %0.2d:%0.2d:%0.2d",
					time->year,
					time->mon,
					time->day,
					time->hour,
					time->min,
					time->sec);
	return date;
}
