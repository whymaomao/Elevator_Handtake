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
//IO��������
//DS_IO�п������ò���
//#define DS_IO_IN()  {GPIOH->MODER&=~(3<<(11*2));GPIOH->MODER|=0<<11*2;}	//PB9����ģʽ
//#define DS_IO_OUT() {GPIOH->MODER&=~(3<<(11*2));GPIOH->MODER|=1<<11*2;} //PB9���ģʽ

//#define DS_IO  PHout(11)
//#define Read_DS_IO PHin(11)

//#define DS_SCLK  PHout(10)
//#define DS_CE   PHout(3)

/////////���Ĵ�������///////////////////////
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
#define LV6     0xA5  //0.7��ѹ��2K��ֵ��2.15mA
#define LV5     0xA9  //1.4��ѹ��2K��ֵ��1.80mA
#define LV4     0xA6  //0.7��ѹ��4K��ֵ��1.07mA
#define LV3     0xAA  //1.7��ѹ��4K��ֵ��0.90mA
#define LV2     0xA7	//0.7��ѹ��8K��ֵ��0.50mA
#define LV1     0xAB	//1.4��ѹ��8K��ֵ��0.45mA

//////time ����ʱ��ṹ�嶨��//////////
typedef struct 
{
		unsigned int year;   //��
		unsigned char mon;   //��
		unsigned char day;   //��
		unsigned char hour;  //ʱ
		unsigned char min;   //��
		unsigned char sec;   //��
		unsigned char week;  //����
}sTime;


void DS1302_Byte_Write(unsigned char dat);  //����һ���ֽڵ�DS1302ͨ��������
unsigned char DS1302_Byte_Read(void);    //��DS1302ͨ�������϶�ȡһ���ֽ�
//�õ���ģʽ��DS1302��ĳһ�Ĵ�����ַд��һ�ֽ�����
void DS1302_Single_Write(unsigned char reg, unsigned char dat); 
//�õ���ģʽ��DS1302��ĳһ�Ĵ�����ȡһ�ֽ����ݣ�����ֵΪ��ȡ���ֽ�����
unsigned char DS1302_Single_Read(unsigned char reg);
//��ͻ��ģʽ��DS1302����д8���Ĵ�������,��д������ָ��Ϊdat
void DS1302_Burst_Write(unsigned char *dat);
//��ͻ��ģʽ��DS1302������ȡ8���Ĵ��������ݣ����ݽ���ָ��Ϊdat
void DS1302_Burst_Read(unsigned char *dat);
//��ȡʵʱʱ��
void GetRealTime( sTime *time);
//����ʵʱʱ��
void SetRealTime( sTime *time);
//DS1302��ʼ��
void DS1302_Init(void);

void DS1302_test(void);
void DS1302_SetTime(sTime*time);

char* date2string(sTime*time);
#endif /*__DS1302_H*/

