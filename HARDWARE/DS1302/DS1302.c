#include "DS1302.h"
#include "Delay.h"
#include "string.h"
///////////////���ú���////////////////////
//����д��������
static void Protect()
{
		DS1302_Single_Write(CTRL,LOCK);
}

//���д��������
static void UnProtect()
{
		DS1302_Single_Write(CTRL,UNLOCK);
}

//DS1302��������
static void Clock_on()
{
		DS1302_Single_Write(SEC,CLKON);
}

//DS1302ֹͣ����
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
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);  //��ʱ��ģ��
	
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

void DS1302_Byte_Write(unsigned char dat) //����һ���ֽڵ�DS1302ͨ��������
{
		unsigned char i = 0;
		//ͨ��д����,������SCLK������
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
		GPIO_SetBits(DS_Port,DS_IO);  //���ȷ���ͷ�IO����
		
}	

unsigned char DS1302_Byte_Read(void)   //��DS1302ͨ�������϶�ȡһ���ֽ�
{
	//DS1302 ��ʱ�����½��ض�����
	//д��ַ�ֽںͶ������ֽڶ��Ǵ�LSB��ʼ
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
//�õ���ģʽ��DS1302��ĳһ�Ĵ�����ַд��һ�ֽ�����
void DS1302_Single_Write(unsigned char reg, unsigned char dat)
{
		
		//ֻ����SCLKΪ�͵�ƽʱ�����ܽ�RST��Ϊ�ߵ�ƽ
		GPIO_ResetBits(DS_Port,DS_CE);
		GPIO_ResetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_SetBits(DS_Port,DS_CE);
		delay_us(2);
		DS1302_Byte_Write((reg)|0x80);//����д�Ĵ���ָ����ƿճ������λ��дλ
		DS1302_Byte_Write(dat);
		GPIO_SetBits(DS_Port,DS_SCLK);
		delay_us(1);
		GPIO_ResetBits(DS_Port,DS_CE);
		
		delay_us(1);
}
//�õ���ģʽ��DS1302��ĳһ�Ĵ�����ȡһ�ֽ����ݣ�����ֵΪ��ȡ���ֽ�����
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
//��ͻ��ģʽ��DS1302����д8���Ĵ�������,��д������ָ��Ϊdat
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
//��ͻ��ģʽ��DS1302������ȡ8���Ĵ��������ݣ����ݽ���ָ��Ϊdat
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

//��ȡʵʱʱ��
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


//����ʵʱʱ��
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
		};  //2016��10��8��12:30:00���ڶ�
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
//DS1302��ʼ��
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
