#ifndef __DS1302_H
#define __DS1302_H

#include "sys.h"

#define DS_Port GPIOG

#define DS_IO GPIO_Pin_10
#define DS_SCLK  GPIO_Pin_11
#define DS_CE GPIO_Pin_12
//DS_IO --> PH11
//DS_SCLK --> PH10
//DS_CE --> PH3
//IO方向设置
//DS_IO有可能设置不对
//#define DS_IO_IN()  {GPIOH->MODER&=~(3<<(11*2));GPIOH->MODER|=0<<11*2;}	//PB9输入模式
//#define DS_IO_OUT() {GPIOH->MODER&=~(3<<(11*2));GPIOH->MODER|=1<<11*2;} //PB9输出模式

//#define DS_IO  PHout(11)
//#define Read_DS_IO PHin(11)

//#define DS_SCLK  PHout(10)
//#define DS_CE   PHout(3)

/////////各寄存器配置///////////////////////
#define SEC  0x80
#define MIN  0x82
#define HOUR 0x84
#define DAY  0x86
#define MONTH 0x88
#define WEEK  0x8A
#define YEAR  0x8C
#define CTRL  0x8E
#define CHRG  0x90
#define BRUST 0xBE
#define RAM   0xC0

#define CLKOFF 0x80
#define CLKON  0x00
#define T12    0x80
#define T24    0x00
#define LOCK   0x80
#define UNLOCK  0x00
#define LV6     0xA5  //0.7降压，2K阻值，2.15mA
#define LV5     0xA9  //1.4降压，2K阻值，1.80mA
#define LV4     0xA6  //0.7降压，4K阻值，1.07mA
#define LV3     0xAA  //1.7降压，4K阻值，0.90mA
#define LV2     0xA7	//0.7降压，8K阻值，0.50mA
#define LV1     0xAB	//1.4降压，8K阻值，0.45mA

//////time 日期时间结构体定义//////////
typedef struct 
{
		unsigned int year;   //年
		unsigned char mon;   //月
		unsigned char day;   //日
		unsigned char hour;  //时
		unsigned char min;   //分
		unsigned char sec;   //秒
		unsigned char week;  //星期
}sTime;


void DS1302_Byte_Write(unsigned char dat);  //发送一个字节到DS1302通信总线上
unsigned char DS1302_Byte_Read(void);    //从DS1302通信总线上读取一个字节
//用单次模式向DS1302的某一寄存器地址写入一字节数据
void DS1302_Single_Write(unsigned char reg, unsigned char dat); 
//用单次模式从DS1302的某一寄存器读取一字节数据，返回值为读取的字节数据
unsigned char DS1302_Single_Read(unsigned char reg);
//用突发模式向DS1302连续写8个寄存器数据,待写入数据指针为dat
void DS1302_Burst_Write(unsigned char *dat);
//用突发模式从DS1302连续读取8个寄存器的数据，数据接收指针为dat
void DS1302_Burst_Read(unsigned char *dat);
//获取实时时间
void GetRealTime( sTime *time);
//设置实时时间
void SetRealTime( sTime *time);
//DS1302初始化
void DS1302_Init(void);

void DS1302_test(void);
void DS1302_SetTime(sTime*time);

char* date2string(sTime*time);
#endif /*__DS1302_H*/

