#ifndef __AD_H
#define __AD_H

#include "sys.h" 

/****************************************
Ê¹ÓÃPA1£ºADC123_1²É¼¯µç³ØµçÑ¹
****************************************/

void MyADC_Init(void);
u16  Get_Adc(u8 ch); 				//»ñµÃÄ³¸öÍ¨µÀÖµ 
u16 Get_Adc_Average(u8 ch,u8 times);//µÃµ½Ä³¸öÍ¨µÀ¸ø¶¨´ÎÊı²ÉÑùµÄÆ½¾ùÖµ  
float Get_Battery_Voltage(void);  //µÃµ½³äµçµç³Ø´ËÊ±µÄµçÑ¹Ö
int Get_Battery_Percentage(void);      //µÃµ½³äµçµç³ØµÄÊ£ÓàÈİÁ¿
#endif /*__AD_H*/

