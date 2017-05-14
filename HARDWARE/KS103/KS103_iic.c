#include "KS103_iic.h"
#include "delay.h"

//��ʼ��IIC
void KIIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOBʱ��

  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
	KIIC_SCL=1;
	KIIC_SDA=1;
}
//����IIC��ʼ�ź�
void KIIC_Start(void)
{
	KSDA_OUT();     //sda�����
	KIIC_SDA=1;	  	  
	KIIC_SCL=1;
	delay_us(50);
 	KIIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(50);
	KIIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void KIIC_Stop(void)
{
	KSDA_OUT();//sda�����
	KIIC_SCL=0;
	KIIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(50);
	KIIC_SCL=1; 
	KIIC_SDA=1;//����I2C���߽����ź�
	delay_us(50);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 KIIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	KSDA_IN();      //SDA����Ϊ����  
	KIIC_SDA=1;delay_us(50);	   
	KIIC_SCL=1;delay_us(50);	 
	while(KREAD_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			KIIC_Stop();
			return 1;
		}
	}
	KIIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void KIIC_Ack(void)
{
	KIIC_SCL=0;
	KSDA_OUT();
	KIIC_SDA=0;
	delay_us(2);
	KIIC_SCL=1;
	delay_us(2);
	KIIC_SCL=0;
}
//������ACKӦ��		    
void KIIC_NAck(void)
{
	KIIC_SCL=0;
	KSDA_OUT();
	KIIC_SDA=1;
	delay_us(50);
	KIIC_SCL=1;
	delay_us(50);
	KIIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void KIIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	KSDA_OUT(); 	    
    KIIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        KIIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(50);   //��TEA5767��������ʱ���Ǳ����
		KIIC_SCL=1;
		delay_us(50); 
		KIIC_SCL=0;	
		delay_us(50);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 KIIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	KSDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        KIIC_SCL=0; 
        delay_us(50);
		KIIC_SCL=1;
        receive<<=1;
        if(KREAD_SDA)receive++;   
		delay_us(50); 
    }					 
    if (!ack)
        KIIC_NAck();//����nACK
    else
      KIIC_Ack(); //����ACK   
    return receive;
}



























