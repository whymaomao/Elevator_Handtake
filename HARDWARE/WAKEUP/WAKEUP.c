#include "WAKEUP.h"
#include "led.h"
#include "delay.h"

extern int stop;

void Sys_Enter_Standby(void)
{
		//while(!WK_UP);   //等待WK_UP按键松开
//		RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
//		RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);  //复位所有IO口
	//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
													//	RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,DISABLE);
	  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_md.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file 
     */ 
     /* Configure all unused GPIO port pins in Analog Input mode (floating input
     trigger OFF), this will reduce the power consumption and increase the device
     immunity against EMI/EMC *************************************************/
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
														RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);
		/* config all IO to Analog Input to reduce parasite power consumption */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		GPIO_Init(GPIOB,&GPIO_InitStructure);	
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		GPIO_Init(GPIOD,&GPIO_InitStructure);
		GPIO_Init(GPIOE,&GPIO_InitStructure);
		GPIO_Init(GPIOF,&GPIO_InitStructure);
		GPIO_Init(GPIOG,&GPIO_InitStructure);
		GPIO_Init(GPIOH,&GPIO_InitStructure);
		GPIO_Init(GPIOI,&GPIO_InitStructure);		
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
														RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,DISABLE);
		// 关闭串口，计时器和AD
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);//使能USART2时钟
		USART_Cmd(USART2, DISABLE); 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_USART6,DISABLE);
		USART_Cmd(USART1, DISABLE);
		USART_Cmd(USART6, DISABLE);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM4,DISABLE);
		TIM_Cmd(TIM4,DISABLE);
		TIM_Cmd(TIM2,ENABLE);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,DISABLE);
		ADC_Cmd(ADC1,DISABLE);
		WKUP_Init();
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE); //使能PWR时钟
		//PWR_FlashPowerDownCmd(ENABLE);
		PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
}



//中断，检测到PA0脚的一个下降沿
void EXTI0_IRQHandler(void)
{
		
		if(EXTI_GetITStatus(EXTI_Line0) != RESET)
		{
				
					EXTI_ClearITPendingBit(EXTI_Line0);
					if(stop == 1)
					{
						SYSCLKConfig_Stop();
					}
		}	
		//SYSCLKConfig_Stop();
}

//PA0 WKUP唤醒初始化
void WKUP_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //中断事件
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}


void SYSCLKConfig_Stop(void)
{
	RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
	RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,DISABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
													//	RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
}





