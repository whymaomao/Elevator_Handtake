#ifndef __Voltage_H
#define __Voltage_H

#include "sys.h"
#include "usart.h"

/*register definition*/
#define baud  0x0004
#define type1 0x0000
#define type2 0x0001
#define volatege 0x0002
#define amper    0x0003
#define Read_Volatege 0x0048
#define Read_Amper    0x0049
#define Read_Power    0x004A
#define Read_Energy   0x004B


/*function definition*/
#define ReadFunction  0x03
#define WriteFunction 0x10
#define Mod_id   0x01

extern unsigned char hahah;
extern unsigned char UART_send_buf[100];
extern unsigned char UART_recv_buf[100]; 

extern volatile unsigned char HasMeg ;   //标记是否接收到了信息
extern unsigned char Tim_out  ;  //用于判断3.5个字节时间

extern  unsigned char Rcv_Complete ; //表示数据接收完成
extern unsigned char Send_Complete; //表示数据发送完成
extern unsigned char hahah;

extern unsigned char Busy ;  

extern unsigned char Res ;  
extern unsigned char Send_Data;

extern volatile unsigned char Rev_num ;
extern unsigned char Send_num;
extern unsigned char * PointToRcvBuf;
extern unsigned char * PointToSendBuf;

extern unsigned char result;

unsigned int CRC16_1(unsigned char* pchMsg, unsigned int wDataLen);

void voltage_recv(void);
void RS485_Send_Data(void);
void voltage_test(unsigned char msg,unsigned char len);
void Modbus_init(void);
void Message_Handler(float *result);
#endif /*__Voltage_H*/

