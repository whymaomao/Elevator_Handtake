#ifndef __SDRAM_H
#define __SDRAM_H
#include "sys.h"

#define BUFFER_SIZE         ((uint32_t)0x0100)

#define WRITE_READ_ADDR     ((uint32_t)0x1FFFC00)//256Mb
#define SDRAM_BANK_ADDR                 ((uint32_t)0xD0000000)
/* Read/Write Buffers */

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 


void SDRAM_Init(void);   //sdram初始化
void SDRAM_Sequence(void);   //初始化sdram各种操作
u8 SDRAM_Test(void);      //测试SDRAM是否工作正常;1:工作正常；0：不正常
#endif /*__SDRAM_H*/

