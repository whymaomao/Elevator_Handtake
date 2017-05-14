#include "BH1750.h"
#include "delay.h"
#include "IIC.h"

static uint8_t s_MTReg;    //灵敏度倍率
static uint8_t s_Mode;     //测量模式
void BH1750_Init(void)
{
		IIC_Init();//IIC初始化
		BH1750_Write_REG(POWER_ON);
		BH1750_ChangeMode(2);
		BH1750_AdjustSensitivity(69);
}

void BH1750_Disable(void)
{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, DISABLE);
}

void BH1750_Write_REG(unsigned char REG_Address)
{
		IIC_Start();
		IIC_Send_Byte(WR_SLAVE_ADDRESS);
		IIC_Wait_Ack();	
		IIC_Send_Byte(REG_Address);
		IIC_Wait_Ack();	
		IIC_Stop();
}


unsigned int BH1750_Read_REG(void)
{
		unsigned int temp = 0;
		IIC_Start();
		IIC_Send_Byte(RD_SLAVE_ADDRESS);
		IIC_Wait_Ack();
		temp = IIC_Read_Byte(1);
		temp = ((temp << 8) + IIC_Read_Byte(0));
		IIC_Stop();
		return temp;
		
}

/**********************************************
*@func:BH1750_AdjustSensivity
*@discription:调节BH1750测量灵敏度
*@param：ucMTReg:量程倍率， 值域[31,254];值越大，灵敏度越高
*@retval:none
**********************************************/
void BH1750_AdjustSensitivity(uint8_t _ucMTReg)
{
		if(_ucMTReg <= 31)
		{
				_ucMTReg = 31;
		}
		else if(_ucMTReg >= 254)
		{
				_ucMTReg = 254;
		}
		s_MTReg = _ucMTReg;
		BH1750_Write_REG(0x40 + (s_MTReg >> 5));  //更改高3bit
		BH1750_Write_REG(0x60 + (s_MTReg & 0x1F));  //更改低5bit
		
		BH1750_ChangeMode(s_Mode);
}
/**********************************************
*@func:BH1750_ChangeMode
*@discription:修改BH1750测量模式,决定测量分辨率
*@param: ucMode:测量模式，1,2,3
*@retval: none
**********************************************/
void BH1750_ChangeMode(uint8_t _ucMode)
{
		if(_ucMode == 1)
		{
				BH1750_Write_REG(H_Mode_con); /*测量模式1，分辨率1 lux*/
				s_Mode = 1;
		}
		else if(_ucMode == 2)
		{
				BH1750_Write_REG(H_Mode2_con); /*测量模式2，分辨率0.5lux*/
				s_Mode = 2 ;
		}
		else if(_ucMode == 3)
		{
				BH1750_Write_REG(L_Mode_con); /*测量模式3，分辨率4 lux*/
				s_Mode = 3;
		}
}
double get_light(void)
{
		double lux = 0;
		unsigned int light_val = 0;
	//	BH1750_Write_REG(REG_Address);
		delay_ms(180);
		//light_val = (float)((float)BH1750_Read_REG()/1.2);
		light_val = BH1750_Read_REG();
	  lux = (double)(light_val * 5 * 69) / (6 * s_MTReg);
	
		if(s_Mode == 2)
		{
				lux = lux /2;
		}
		else
		{
			;
		}
		return lux;
		
		  
}

