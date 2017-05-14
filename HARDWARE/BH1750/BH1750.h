#ifndef __BH1750_H
#define __BH1750_H

#include "sys.h"
#include "IIC.h"

#define RD_SLAVE_ADDRESS  0x47  //ADDR脚接地时的读地址
#define WR_SLAVE_ADDRESS  0x46  //ADDR脚接地时的写地址

#define POWER_DOWN        0x00  //No actice state
#define POWER_ON          0x01  //Waiting for measurement command
#define Reset             0x07  //Reset Data register value. Reset command is not accepeted in power down mode
#define H_Mode_con				0x10  //Start measurement at 1lx resolution.Time is typically is 120ms
#define H_Mode2_con      	0x11  //Start measurement at 0.5lx resolution.
#define L_Mode_con        0x13
#define H_Mode_one        0x20  //It is automatically set to PowerDown after measurement
#define H_Mode2_one       0x21  //0.5lx

void BH1750_Init(void);     //Init
void BH1750_Write_REG(unsigned char REG_Address);
unsigned int BH1750_Read_REG(void);
double get_light(void);
void BH1750_AdjustSensitivity(uint8_t _ucMTReg);
void BH1750_ChangeMode(uint8_t _ucMode);
void BH1750_Disable(void);
#endif /*__BH1750_H*/
