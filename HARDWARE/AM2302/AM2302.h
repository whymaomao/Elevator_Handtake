#ifndef __AM2302_H
#define __AM2302_H

#include "sys.h"

//AM2302 data:PC5

//IO��������
#define AM_IO_IN()  {GPIOC->MODER &= ~(3<<5*2);GPIOC->MODER |= (0<<5*2);}  //����IO����Ϊ���뷽��
#define AM_IO_OUT() {GPIOC->MODER &= ~(3<<5*2);GPIOC->MODER |= (1<<5*2);}  //����IO����Ϊ�������

//IO��������
#define AM_DQ_OUT  PCout(5)
#define AM_DQ_IN   PCin(5)

u8 AM_Init(void);//��ʼ��AM2302
u8 AM_Read_Data(int *temp,int *humi,u8 *check);//��ȡ��ʪ��
u8 AM_Read_Byte(void);//����һ���ֽ�
u8 AM_Read_Bit(void);//����һ��λ
u8 AM_Check(void);//����Ƿ����AM2302
void AM_Rst(void); //��λAM2302
void AM_Disable(void);


#endif /*__AM2302_H*/
