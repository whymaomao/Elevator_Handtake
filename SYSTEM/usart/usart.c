#include "sys.h"
#include "usart.h"	
#include "led.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif


////////////////////////////////////////////////////////////////////////////////// 	  
 

unsigned char connected;
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch; 
	
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOF,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
	
	//CTS和RTS配置
	//CTS<->PF6，与模块RTS连接，CTS为输入
	//RTS<->PF7,与模块CTS连接，RTS为输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
//	GPIO_SetBits(GPIOF,GPIO_Pin_6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF,&GPIO_InitStructure);

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
//	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET) 
//	{
//			USART_SendData(USART1, 'a');
//			USART_ITConfig(USART1, USART_IT_TC, DISABLE);
//			LED0 = !LED0;
//	}
		
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		

		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
  } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	


#if EN_USART6_RX
	//串口1中断服务程序
	//注意,读取USARTx->SR能避免莫名其妙的错误   	
	u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
	//接收状态
	//bit15，	接收完成标志
	//bit14，	接收到0x0d
	//bit13~0，	接收到的有效字节数目
//	u16 USART_RX_STA=0;       //接收状态标记	
	//初始化IO 串口6 
	//bound:波特率
//usart6:tx PC6  rx:PC7
	void uart6_init(u32 bound)
	{
			//GPIO端口设置
			 GPIO_InitTypeDef GPIO_InitStructure;
			 USART_InitTypeDef USART_InitStructure;
			 NVIC_InitTypeDef NVIC_InitStructure;
			//使能时钟
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART6时钟
		
			//串口6引脚复用
			GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOC6复用为USART6
			GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC7复用为USART6
		
			//串口6引脚设置
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOC6与GPIOC7
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
			GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC6，PC7
		
			//USART6 初始化设置
			USART_InitStructure.USART_BaudRate = bound;//波特率设置
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
			USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
			USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
			USART_Init(USART6, &USART_InitStructure); //初始化串口6
			
			USART_Cmd(USART6, ENABLE);  //使能串口6

			#if EN_USART6_RX 	
			USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断

			//Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn ;//串口1中断通道
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

			#endif
			
	}
		void USART6_IRQHandler(void)                	//串口1中断服务程序
	{
		u8 Res;
	#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
		OSIntEnter();    
	#endif
		if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			Res =USART_ReceiveData(USART6);//(USART1->DR);	//读取接收到的数据
			
			if((USART_RX_STA&0x8000)==0)//接收未完成
			{
				if(USART_RX_STA&0x4000)//接收到了0x0d
				{
					if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
					else USART_RX_STA|=0x8000;	//接收完成了 
				}
				else //还没收到0X0D
				{	
					if(Res==0x0d)USART_RX_STA|=0x4000;
					else
					{
						USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
						USART_RX_STA++;
						if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
		} 
	#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
		OSIntExit();  											 
	#endif
} 
	

#endif

//串口发送缓冲区
__align(8) u8 UART6_TX_BUF[UART6_MAX_SEND_LEN];
	//串口7,printf函数
//确保一次发送数据不超过UART7_MAX_SEND_LEN个字节
void UART6_Printf(char *fmt,...)
{
		u16 i,j;
		va_list ap;
		va_start(ap,fmt);
		vsprintf((char*)UART6_TX_BUF,fmt,ap);
		va_end(ap);
		
		i = strlen((const char*) UART6_TX_BUF);
		for(j =0; j < i; j++)
	{
			while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
			USART_SendData(USART6,(uint8_t)UART6_TX_BUF[j]); 
	}
		
}

void UART6_SendChar(u8 num)
{
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
			USART_SendData(USART6,num); 
}

 //初始化IO 串口2
//bound:波特率	  
void RS485_Init(u32 bound)
{  	 
	
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	
  //串口2引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
	
	//USART2    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2，PA3
	
	//PD14推挽输出，485模式控制  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PG8
	
	

   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
  USART_Cmd(USART2, ENABLE);  //使能串口 2
//	
//	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

	GPIO_ResetBits(GPIOD,GPIO_Pin_14);

}

void RS485_PD14_Set(void)
{
			GPIO_InitTypeDef GPIO_InitStructure;
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //GPIOG8
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
			GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PG8
			GPIO_ResetBits(GPIOD,GPIO_Pin_14);
}



