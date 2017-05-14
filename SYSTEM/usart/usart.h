#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART6_RX      1   //ʹ�ܣ�1��/��ֹ��0������2����
#define EN_USART2_RX      1 

#define UART6_MAX_SEND_LEN  512 
extern u8 connected;	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void RS485_PD14_Set(void);
void uart6_init(u32 bound);
void RS485_Init(u32 bound);
void UART6_Printf(char *fmt,...);
#define RS485_TX_EN		PDout(14)	//485ģʽ����.0,����;1,����.
#endif


