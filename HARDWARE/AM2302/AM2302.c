#include "AM2302.h"
#include "delay.h"

//��ʼ��AM2302��ͬʱ���AM2302�Ĵ���
//����1��������
//����0������
u8 AM_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOBʱ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
		AM_Rst();
		return AM_Check();
}


void AM_Disable(void)
{
		//GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, DISABLE);
}
//����Ƿ����AM2302
//����1��δ��⵽AM�Ĵ���
//����0������
//����ʱ����Ϊ�ߵ�ƽ��ͨѶ��ʼʱMCU��������1~10ms�ͷ����ߣ���ʱ20-40us��������ʼ���ӻ�����Ӧ�ź�
//�ӻ�����Ӧ�ź���һ��80us���ҵĵ͵�ƽ�����ӻ�����������80us���Ҵ������������ݴ���
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

		while(AM_DQ_IN && retry < 100 )    //AM2302����40~80us
		{
				retry ++;
				delay_us(1);
		};
		if(retry>=100)return 1;
		else retry=0;
		while(!AM_DQ_IN && retry < 100)  //AM2302���ͺ���ٴ�����40~80us
		{
				retry ++;
				delay_us(1);
		};
		if (retry >= 100) return 1;
		return 0;	
}

//����һ��λ
//ÿ1bit���ݶ�����һ���͵�ƽ��һ���ߵ�ƽ���
//�͵�ƽ��һ��50us���ҵĵ͵�ƽ���ϳ��ĸߵ�ƽΪ1���϶̵ĸߵ�ƽΪ0
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


//����һ���ֽ�
//����ֵΪ����������
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

//��ȡ��ʪ��
u8 AM_Read_Data(int *temp,int *humi,u8 *check)
{
		u8 buf[5];
		u8 i;
//		GPIO_InitTypeDef  GPIO_InitStructure;
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//		GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
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


