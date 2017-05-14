#ifndef __AM2302_H
#define __AM2302_H

#include "sys.h"

//AM2302 data:PC5

//IO方向设置
#define AM_IO_IN()  {GPIOC->MODER &= ~(3<<5*2);GPIOC->MODER |= (0<<5*2);}  //设置IO方向为输入方向
#define AM_IO_OUT() {GPIOC->MODER &= ~(3<<5*2);GPIOC->MODER |= (1<<5*2);}  //设置IO方向为输出方向

//IO操作函数
#define AM_DQ_OUT  PCout(5)
#define AM_DQ_IN   PCin(5)

u8 AM_Init(void);//初始化AM2302
u8 AM_Read_Data(int *temp,int *humi,u8 *check);//读取温湿度
u8 AM_Read_Byte(void);//读出一个字节
u8 AM_Read_Bit(void);//读出一个位
u8 AM_Check(void);//检测是否存在AM2302
void AM_Rst(void); //复位AM2302
void AM_Disable(void);


#endif /*__AM2302_H*/
