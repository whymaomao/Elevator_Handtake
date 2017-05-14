#include "timer.h"
#include "Voltage.h"
#include "WAKEUP.h"

//电压电流采集使用定时器2
void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器2更新中断
	TIM_Cmd(TIM2,DISABLE); //使能定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void TIM2_IRQHandler(void)
{ 
 
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
		Tim_out = 1;
		TIM_Cmd(TIM2,DISABLE);
		TIM_SetCounter(TIM2,0);
		Rcv_Complete = 1;
		
		UART_recv_buf[0] = Rev_num;
		if(HasMeg != 0 && Rev_num >4)
		{
				Busy = 1;
			
		}
		
	}
 
}


void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM4时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM4,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

extern int stop;
int timer_count = 0;
void TIM4_IRQHandler(void)
{
		//int count = 0;
		if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
		{
			timer_count ++;
			TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
			if(timer_count == 100)
			{
				stop = 1;
				timer_count = 0;
				TIM_Cmd(TIM4,DISABLE);
			}
			
		}
}