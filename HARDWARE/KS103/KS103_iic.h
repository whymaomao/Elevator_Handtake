#ifndef __KS103_iic_H
#define __KS103_iic_H
#include "sys.h" 


//KS103PCB�����������ô���
//IO:PG14:SCL;  PG13:SDA
//IO��������
#define KSDA_IN()  {GPIOG->MODER&=~(3<<(13*2));GPIOG->MODER|=0<<13*2;}	//PB9����ģʽ
#define KSDA_OUT() {GPIOG->MODER&=~(3<<(13*2));GPIOG->MODER|=1<<13*2;} //PB9���ģʽ
//IO��������	 
#define KIIC_SCL    PGout(14) //SCL
#define KIIC_SDA    PGout(13) //SDA	 
#define KREAD_SDA   PGin(13)  //����SDA 

//IIC���в�������
void KIIC_Init(void);                //��ʼ��IIC��IO��				 
void KIIC_Start(void);				//����IIC��ʼ�ź�
void KIIC_Stop(void);	  			//����IICֹͣ�ź�
void KIIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 KIIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 KIIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void KIIC_Ack(void);					//IIC����ACK�ź�
void KIIC_NAck(void);				//IIC������ACK�ź�
  
#endif
