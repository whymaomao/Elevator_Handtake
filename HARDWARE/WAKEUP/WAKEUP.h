#ifndef __WKUP_H
#define __WKUP_H

#include "sys.h"

u8 Check_WKUP(void);  //检测WKUP脚的信号
void WKUP_Init(void);   //WKUP唤醒初始化
void Sys_Enter_Standby(void);  //系统进入待机模式
void SYSCLKConfig_Stop(void);

#endif /*__WKUP_H*/