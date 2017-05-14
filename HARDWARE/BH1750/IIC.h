#ifndef __IIC_H
#define __IIC_H
#include "sys.h" 

//BH1750_SCL:PF6
//BH1750_SDA:PF7


//IO��������
#define SDA_IN()  {GPIOF->MODER&=~(3<<(7*2));GPIOF->MODER|=0<<7*2;}	//PB9����ģʽ
#define SDA_OUT() {GPIOF->MODER&=~(3<<(7*2));GPIOF->MODER|=1<<7*2;} //PB9���ģʽ
//IO��������	 
#define IIC_SCL    PFout(6) //SCL
#define IIC_SDA    PFout(7) //SDA	 
#define READ_SDA   PFin(7)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
