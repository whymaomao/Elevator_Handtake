#ifndef __WKUP_H
#define __WKUP_H

#include "sys.h"

u8 Check_WKUP(void);  //���WKUP�ŵ��ź�
void WKUP_Init(void);   //WKUP���ѳ�ʼ��
void Sys_Enter_Standby(void);  //ϵͳ�������ģʽ
void SYSCLKConfig_Stop(void);

#endif /*__WKUP_H*/