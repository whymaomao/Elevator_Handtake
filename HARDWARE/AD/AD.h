#ifndef __AD_H
#define __AD_H

#include "sys.h" 

/****************************************
ʹ��PA1��ADC123_1�ɼ���ص�ѹ
****************************************/

void MyADC_Init(void);
u16  Get_Adc(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
float Get_Battery_Voltage(void);  //�õ�����ش�ʱ�ĵ�ѹ�
int Get_Battery_Percentage(void);      //�õ�����ص�ʣ������
#endif /*__AD_H*/

