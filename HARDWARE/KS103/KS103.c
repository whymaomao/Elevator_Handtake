#include "KS103.h"
#include "KS103_iic.h"
#include "delay.h"

void KS103_Init(void)
{
		KIIC_Init();
}

void KS103_write(unsigned char address, unsigned char buffer, unsigned char command)
{
		KIIC_Start();
		KIIC_Send_Byte(address);
		KIIC_Wait_Ack();
		KIIC_Send_Byte(buffer);
		KIIC_Wait_Ack();
		KIIC_Send_Byte(command);
		KIIC_Wait_Ack();
		KIIC_Stop();

}

void KS103_Disable(void)
{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, DISABLE);
}

double KS103_read(unsigned address, unsigned char buffer, unsigned char command)
{
		unsigned int dat;
		double  distance;
		KS103_write(address,buffer, command);

		delay_ms(1);
	
		delay_ms(20);
	
		dat = KS103_read_byte(address,2);
		dat = (dat << 8) ;
		dat += KS103_read_byte(address,3);
		distance = (double)(dat);
		
		return distance;
}

unsigned char KS103_read_byte(unsigned char address, unsigned char buffer)
{
		unsigned char temp = 0;
		KIIC_Start();
		KIIC_Send_Byte(address);
		KIIC_Wait_Ack();
		KIIC_Send_Byte(buffer);
		KIIC_Wait_Ack();
	
		KIIC_Start();
		KIIC_Send_Byte(address + 1);
		KIIC_Wait_Ack();
	
		delay_ms(1);
		temp = KIIC_Read_Byte(0);
		KIIC_Stop();
		return temp;
}

unsigned int detect_KS103(unsigned char address, unsigned char command)
{
		unsigned int distance;
		KS103_write(address,2,command);
		delay_ms(1);
		//IIC_SCL = 0;
		//while(!IIC_SCL);
		delay_ms(80);
		distance = KS103_read_byte(address,2);
		distance<<= 8;
		distance += KS103_read_byte(address,3);
		delay_ms(5);
		return distance;
		
		
}



