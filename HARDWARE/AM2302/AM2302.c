#include "AM2302.h"
#include "delay.h"

//初始化AM2302，同时检测AM2302的存在
//返回1：不存在
//返回0：存在
u8 AM_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOB时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
		AM_Rst();
		return AM_Check();
}


void AM_Disable(void)
{
		//GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, DISABLE);
}
//检测是否存在AM2302
//返回1：未检测到AM的存在
//返回0：存在
//空闲时总线为高电平，通讯开始时MCU拉低总线1~10ms释放总线，延时20-40us后主机开始检测从机的响应信号
//从机的响应信号是一个80us左右的低电平，随后从机在拉高总线80us左右代表即将进入数据传送
void AM_Rst(void)
{
		AM_IO_OUT();
		AM_DQ_OUT = 0;
		delay_ms(20);
		AM_DQ_OUT = 1;
		delay_us(30);
}

u8 AM_Check()
{

		u8 retry = 0;
		AM_IO_IN();

		while(AM_DQ_IN && retry < 100 )    //AM2302拉低40~80us
		{
				retry ++;
				delay_us(1);
		};
		if(retry>=100)return 1;
		else retry=0;
		while(!AM_DQ_IN && retry < 100)  //AM2302拉低后会再次拉高40~80us
		{
				retry ++;
				delay_us(1);
		};
		if (retry >= 100) return 1;
		return 0;	
}

//读出一个位
//每1bit数据都是由一个低电平和一个高电平组成
//低电平是一个50us左右的低电平，较长的高电平为1，较短的高电平为0
u8 AM_Read_Bit(void)
{
		u8 retry = 0;
		while(AM_DQ_IN && retry < 100)
		{
				retry ++;
			  delay_us(1);
		}
		retry = 0;
		while(!AM_DQ_IN && retry <100)
		{
				retry ++;
				delay_us(1);
		}
		delay_us(40);
		if(AM_DQ_IN) return 1;
		else return 0;
}


//读出一个字节
//返回值为读到的数据
u8 AM_Read_Byte(void)
{
	   u8 i,dat;
		 dat=0;
		for (i=0;i<8;i++) 
		{
   		dat<<=1; 
	    dat|=AM_Read_Bit();
    }						    
    return dat;
}

//读取温湿度
u8 AM_Read_Data(int *temp,int *humi,u8 *check)
{
		u8 buf[5];
		u8 i;
//		GPIO_InitTypeDef  GPIO_InitStructure;
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
		AM_Rst();
		if(AM_Check() == 0)
		{
			for(i=0;i<5;i++)
			{
				buf[i]=AM_Read_Byte();
			}
			//if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
			//{
				*humi=(buf[0] << 8)|buf[1];
				*temp=(buf[2] << 8)|buf[3];
			//}
			
		}
		else return 1;
		return 0;

 
}


