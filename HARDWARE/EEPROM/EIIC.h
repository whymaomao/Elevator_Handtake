#ifndef __EIIC_H
#define __EIIC_H
#include "sys.h" 


//IO方向设置
#define ESDA_IN()  {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=0<<5*2;}	
#define ESDA_OUT() {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=1<<5*2;} 	


//IO操作函数	 
#define EIIC_SCL    		PHout(4)	 	//SCL
#define EIIC_SDA    		PHout(5) 		//SDA	 
#define EREAD_SDA    	PHin(5)  		//输入SDA    

//IIC所有操作函数
void EIIC_Init(void);                //初始化IIC的IO口				 
void EIIC_Start(void);				//发送IIC开始信号
void EIIC_Stop(void);	  			//发送IIC停止信号
void EIIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 EIIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 EIIC_Wait_Ack(void); 				//IIC等待ACK信号
void EIIC_Ack(void);					//IIC发送ACK信号
void EIIC_NAck(void);				//IIC不发送ACK信号

void EIIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 EIIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
