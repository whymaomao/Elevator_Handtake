#ifndef __EIIC_H
#define __EIIC_H
#include "sys.h" 


//IO��������
#define ESDA_IN()  {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=0<<5*2;}	
#define ESDA_OUT() {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=1<<5*2;} 	


//IO��������	 
#define EIIC_SCL    		PHout(4)	 	//SCL
#define EIIC_SDA    		PHout(5) 		//SDA	 
#define EREAD_SDA    	PHin(5)  		//����SDA    

//IIC���в�������
void EIIC_Init(void);                //��ʼ��IIC��IO��				 
void EIIC_Start(void);				//����IIC��ʼ�ź�
void EIIC_Stop(void);	  			//����IICֹͣ�ź�
void EIIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 EIIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 EIIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void EIIC_Ack(void);					//IIC����ACK�ź�
void EIIC_NAck(void);				//IIC������ACK�ź�

void EIIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 EIIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
