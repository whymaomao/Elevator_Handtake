#ifndef __LED_H
#define __LED_H

#include "sys.h"

void LED_Init(void);
void KEY_Init(void);
void Beep_Init(void);
#define LED0 PDout(13)
#define BEEP PCout(4)


//   KEY0:PD0
//   KEY1:PD15
//   KEY2:PE7
//   KEY3:PD1
//   KEY4:PF2
//   KEY5:PF1
//   WK_UP: PA0
//   KEY6:PF3
#define WK_UP 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	
#define KEY0    GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0)
#define KEY1    GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)
#define KEY2    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)
#define KEY3    GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)
#define KEY4    GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_2)
#define KEY5    GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)
#define KEY6    GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_3)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define KEY2_PRES 3
#define KEY3_PRES 4
#define KEY4_PRES 5
#define KEY5_RRES 6
#define KEY6_PRES 7
#define WKUP_PRES 8

u8 KEY_Scan(u8);

#endif  /*__LED_H*/

