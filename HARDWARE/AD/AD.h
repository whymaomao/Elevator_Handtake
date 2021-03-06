#ifndef __AD_H
#define __AD_H

#include "sys.h" 

/****************************************
使用PA1：ADC123_1采集电池电压
****************************************/

void MyADC_Init(void);
u16  Get_Adc(u8 ch); 				//获得某个通道值 
u16 Get_Adc_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  
float Get_Battery_Voltage(void);  //得到充电电池此时的电压�
int Get_Battery_Percentage(void);      //得到充电电池的剩余容量
#endif /*__AD_H*/

