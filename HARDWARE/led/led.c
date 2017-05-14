#include "led.h"
#include "delay.h"

//LED:PD13
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_13);

}

void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
}

void KEY_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP对应引脚PA0
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_15;
		GPIO_Init(GPIOD,&GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOE,&GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
		GPIO_Init(GPIOF,&GPIO_InitStructure);
	}

	
	
u8 KEY_Scan(u8 mode)
{
		static u8 key_up = 1;
		if(mode ) key_up = 1;
		if(key_up&&(KEY0 == 0 || KEY1 == 0|| KEY2 == 0 || KEY3 == 0
				|| KEY4 == 0 || KEY5 == 0 || KEY6 == 0 || WK_UP == 0))
		{
				delay_ms(10); //去抖动
				key_up = 0;
				if(KEY0 == 0) return 1;
				else if (KEY1 == 0) return 2;
				else if (KEY2 == 0) return 3;
				else if (KEY3 == 0) return 4;
				else if (KEY4 == 0) return 5;
				else if (KEY5 == 0) return 6;
				else if (KEY6 == 0) return 7;
				else if (WK_UP == 0) return 8;
			
		}else if((KEY0 == 1 && KEY1 == 1&& KEY2 == 1 && KEY3 == 1
				&& KEY4 == 1 && KEY5 == 1 && KEY6 == 1 && WK_UP == 1))
			key_up = 1;
		return 0;
}