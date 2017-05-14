#include "EIIC.h"
#include "delay.h"

//��ʼ��IIC
void EIIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);//ʹ��GPIOBʱ��

  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOH, &GPIO_InitStructure);//��ʼ��
	EIIC_SCL=1;
	EIIC_SDA=1;
}
//����IIC��ʼ�ź�
void EIIC_Start(void)
{
	ESDA_OUT();     //sda�����
	EIIC_SDA=1;	  	  
	EIIC_SCL=1;
	delay_us(4);
 	EIIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	EIIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void EIIC_Stop(void)
{
	ESDA_OUT();//sda�����
	EIIC_SCL=0;
	EIIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	EIIC_SCL=1; 
	EIIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 EIIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	ESDA_IN();      //SDA����Ϊ����  
	EIIC_SDA=1;delay_us(1);	   
	EIIC_SCL=1;delay_us(1);	 
	while(EREAD_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			EIIC_Stop();
			return 1;
		}
	}
	EIIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void EIIC_Ack(void)
{
	EIIC_SCL=0;
	ESDA_OUT();
	EIIC_SDA=0;
	delay_us(2);
	EIIC_SCL=1;
	delay_us(2);
	EIIC_SCL=0;
}
//������ACKӦ��		    
void EIIC_NAck(void)
{
	EIIC_SCL=0;
	ESDA_OUT();
	EIIC_SDA=1;
	delay_us(2);
	EIIC_SCL=1;
	delay_us(2);
	EIIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void EIIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	ESDA_OUT(); 	    
    EIIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        EIIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
			
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		EIIC_SCL=1;
		delay_us(2); 
		EIIC_SCL=0;	
		delay_us(2);
		delay_ms(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 EIIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	ESDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        EIIC_SCL=0; 
        delay_us(2);
		EIIC_SCL=1;
        receive<<=1;
        if(EREAD_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        EIIC_NAck();//����nACK
    else
        EIIC_Ack(); //����ACK   
    return receive;
}
