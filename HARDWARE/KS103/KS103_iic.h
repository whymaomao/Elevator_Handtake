#ifndef __KS103_iic_H
#define __KS103_iic_H
#include "sys.h" 


//KS103PCB板上引脚设置错了
//IO:PG14:SCL;  PG13:SDA
//IO方向设置
#define KSDA_IN()  {GPIOG->MODER&=~(3<<(13*2));GPIOG->MODER|=0<<13*2;}	//PB9输入模式
#define KSDA_OUT() {GPIOG->MODER&=~(3<<(13*2));GPIOG->MODER|=1<<13*2;} //PB9输出模式
//IO操作函数	 
#define KIIC_SCL    PGout(14) //SCL
#define KIIC_SDA    PGout(13) //SDA	 
#define KREAD_SDA   PGin(13)  //输入SDA 

//IIC所有操作函数
void KIIC_Init(void);                //初始化IIC的IO口				 
void KIIC_Start(void);				//发送IIC开始信号
void KIIC_Stop(void);	  			//发送IIC停止信号
void KIIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 KIIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 KIIC_Wait_Ack(void); 				//IIC等待ACK信号
void KIIC_Ack(void);					//IIC发送ACK信号
void KIIC_NAck(void);				//IIC不发送ACK信号
  
#endif
