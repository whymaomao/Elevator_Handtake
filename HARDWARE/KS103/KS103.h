#ifndef __KS103_H
#define __KS103_H

#include "sys.h"
#include "KS103_iic.h"


#define KS103_Slave_Address  0xe8
#define KS103_Register2      0x02
#define KS103_Register3      0x03

#define KS103_detect         0xb0
 

void KS103_Init(void);
void KS103_write(unsigned char address, unsigned char buffer, unsigned char command);
double KS103_read(unsigned address, unsigned char buffer, unsigned char command);
unsigned char KS103_read_byte(unsigned char address, unsigned char buffer);
unsigned int detect_KS103(unsigned char address, unsigned char command);
void KS103_Disable(void);

#endif
