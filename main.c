
#include "main.h"
#include "bmp.h"

Sensor *p_Sensor;
static u8 project = 0;
extern NAND_DRV nand0_drv;
extern NAND_DRV_CFG cfg;
extern int timer_count;
void Main_Init(void)
{
	delay_init(168);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	AM_Init();
//	KS103_Init();
//	BH1750_Init();
	DS1302_Init();
	KEY_Init();
//	WKUP_Init();
//	LED_Init();		//初始化LED
	OLED_Init();			//初始化OLED  
	//MyADC_Init();
	TIM4_Int_Init(5000-1,8400 -1);
	nand0_drv.Init(&cfg);
	AT24CXX_Init();
	//RS485_Init(4800);		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);//使能USART2时钟
	USART_Cmd(USART2, DISABLE); 
}

void AM2302_Init(void)
{
			char buf[20];
			OLED_ShowCHinese(18,2,0);//温
			OLED_ShowCHinese(36,2,1);//度
			sprintf(buf,"%4.1f",0.0);
				OLED_ShowString(60,2,buf);
				OLED_ShowCHinese(96,2,11);
				OLED_ShowCHinese(18,5,2);//湿
				OLED_ShowCHinese(36,5,1);//度
		sprintf(buf,"%4.1f",0.0);
		OLED_ShowString(60,5,buf);
		OLED_ShowString(96,5,"%");
}

void AM2302_Test(int *t, int *h)
{
		int temper = 0;
		int humi = 0;
		*t = 0;
		*h = 0;
		u8 check;
		char buf[20];
//		AM_Init();
////		OLED_Init();			//初始化OLED  
//		OLED_Clear()  	; 
//		while(1)
//		{
//				OLED_Clear()  	; 
				OLED_ShowCHinese(18,2,0);//温
				OLED_ShowCHinese(36,2,1);//度
				AM_Read_Data(&temper,&humi,&check);
				
				if(temper > (*t)) *t = temper;
				if(humi > (*h)) *h = humi;
				sprintf(buf,"%4.1f",(double)temper/10 - 6.5);
				//Sensor_setTemperature(p_Sensor,(double)temper/10);
				OLED_ShowString(60,2,buf);
				OLED_ShowCHinese(96,2,11);
				OLED_ShowCHinese(18,5,2);//湿
				OLED_ShowCHinese(36,5,1);//度
				sprintf(buf,"%4.1f",((double)humi/10));
			//	Sensor_setHumidity(p_Sensor,(double)humi/10);
				OLED_ShowString(60,5,buf);  
				OLED_ShowString(96,5,"%");
			//	OLED_ShowString(0,7,"-*-*-*-*-*-*-*-*");
			//	delay_ms(800);
				
//		}
}

void CONN_test(void)
{
	uart6_init(115200);
	for(int i = 0; i < 5; i ++)
	{
			UART6_Printf("Hello World!\r\n");
			delay_ms(500);
	}
	while(1);
	
}

void KS103_test(float*num)
{
//		unsigned char ID = 0;
//		unsigned char date = 0;
	//OLED_Clear();
		u8 i = 0;
	//	float num;
		char buf[10];
	

//		OLED_Init();			//初始化OLED  
//		OLED_Clear()  	; 
//		KS103_Init();
//		KS103_write(0xe8,2,0xC0);
//		delay_ms(2000);
//	//	printf("I can write");
//	
//	
//		ID = KS103_read_byte(KS103_Slave_Address,0);
//		//LCD_ShowNum(30,180,ID,3,16);
//		date = KS103_read_byte(KS103_Slave_Address,1);
		//LCD_ShowNum(30,210,date,3,16);
	
		
//		while(1)
//		{
				delay_ms(500);
				
				OLED_ShowCHinese(18,2,3);
				OLED_ShowCHinese(36,2,4);
				OLED_ShowCHinese(54,2,5);
				OLED_ShowCHinese(72,2,6);
				for( i = 0; i < 50; i ++)
				{
					*num+= KS103_read(KS103_Slave_Address,2,KS103_detect);
				}

				*num/= 50;
				//LCD_ShowNum(30+10*12,150+,num,6,16);
				sprintf(buf,"%4.1fmm",(float)*num);
				
				OLED_ShowString(36,5,buf);  
				
//				delay_ms(500);
//		}
}

void BH1750_test(float*light)
{

		char buf[10];
	//	double light = 0;
		float num;
		u8 i = 0;
//		BH1750_Init();
//		OLED_Init();			//初始化OLED  
//		OLED_Clear()  	; 
//		while(1)
//		{
		//		light = get_light(H_Mode_con);
				//LCD_ShowNum(30+10*12,150,light,5,16);
							Beep_Init();
				OLED_ShowCHinese(18,2,7);//高
				OLED_ShowCHinese(36,2,1);//度
				num = KS103_read(KS103_Slave_Address,2,KS103_detect);
				sprintf(buf,"%4.1fmm",num);
				if(num > 1500 && num < 1550)
				{
			
						for( i = 0; i < 200; i ++)
						{
								BEEP = ~BEEP;
								delay_ms(1);
						}
				}
				OLED_ShowString(54,2,buf);  
			//	OLED_ShowString(108,2,"m");
	
				OLED_ShowCHinese(18,5,12);//光
				OLED_ShowCHinese(36,5,13);//强
				*light = get_light();
				sprintf(buf, "%4.1flux", *light);
			//	Sensor_setLight(p_Sensor,light);
				OLED_ShowString(54,5,buf);
				//OLED_ShowString(108,5,"lux");
			//	delay_ms(800);
//		}
}


void DS1302_Test(void)
{
		char buf[30];
		sTime bufTime;
		DS1302_Init();
		OLED_Init();			//初始化OLED  
		OLED_Clear()  	; 
		while(1)
		{
				GetRealTime(&bufTime); 
				sprintf(buf, 
	          "%0.2d:%0.2d:%0.2d ", 
			  bufTime.hour, 
			  bufTime.min,
			  bufTime.sec);
				OLED_ShowString(0,3,buf);
			
//				sprintf(buf, 
//	          "20%0.2d/%0.2d/%0.2d", 
//			  bufTime.year, 
//			  bufTime.mon, 
//			  bufTime.day);
//				OLED_ShowString(16,3,buf);
				delay_ms(1000);
		}
		
		
}




void Voltage_Test(void)
{
		float result;
		char buf[10];

		//Modbus_init();
//		TIM2_Int_Init(100-1,8400-1);
//		TIM_Cmd(TIM2,DISABLE); //使能定时器2
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//		USART_Cmd(USART2, ENABLE); 
		Modbus_init();
		voltage_test(0x02,0x01);
		RS485_Send_Data();
		delay_ms(500);
		Message_Handler(&result);
		sprintf(buf,"A:%4.1fA",(float)result);
		OLED_ShowString(18,0,buf);
		nand0_drv.Init(&cfg);
}

void Amper_Test(void)
{
		float result;
		char buf[10];

		//Modbus_init();
//		TIM2_Int_Init(100-1,8400-1);
//		TIM_Cmd(TIM2,DISABLE); //使能定时器2
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//		USART_Cmd(USART2, ENABLE); 
		Modbus_init();
		voltage_test(0x49,0x01);
		RS485_Send_Data();
		delay_ms(500);
		Message_Handler(&result);
		sprintf(buf,"A:%4.1fA",(float)result);
		OLED_ShowString(18,0,buf);
		nand0_drv.Init(&cfg);
}

void OLED_Test(void)
{
		u8 t;
		OLED_Init();			//初始化OLED  
		OLED_Clear()  	; 
			t=' ';
	while(1) 
	{		
		OLED_Clear();
		OLED_ShowCHinese(0,0,0);//中
		OLED_ShowCHinese(18,0,1);//景
		OLED_ShowCHinese(36,0,2);//园
//		OLED_ShowCHinese(54,0,3);//电
//		OLED_ShowCHinese(72,0,4);//子
//		OLED_ShowCHinese(90,0,5);//科
//		OLED_ShowCHinese(108,0,6);//技
		OLED_ShowString(0,3,"1.3' OLED TEST");
		//OLED_ShowString(8,2,"ZHONGJINGYUAN");  
	 //	OLED_ShowString(20,4,"2014/05/01");  
		OLED_ShowString(0,6,"ASCII:");  
		OLED_ShowString(63,6,"CODE:");  
		OLED_ShowChar(48,6,t);//显示ASCII字符	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,6,t,3,16);//显示ASCII字符的码值 	
			
		
		delay_ms(8000);
		OLED_Clear();
		delay_ms(8000);
		//OLED_DrawBMP(0,0,128,8,BMP1);  //图片显示(图片显示慎用，生成的字表较大，会占用较多空间，FLASH空间8K以下慎用)
		delay_ms(8000);
	//	OLED_DrawBMP(0,0,128,8,BMP2);
		delay_ms(8000);
	}	  
}


void Key_Test(void)
{
			u8 key;
		OLED_Init();			//初始化OLED  
		OLED_Clear()  	; 
		KEY_Init();
	
		while(1)
		{
				key = KEY_Scan(0);
				if(key)
				{
						switch(key)
						{
							case KEY0_PRES:
								OLED_ShowNum(103,6,1,3,16);
								//AM2302_Test();
								break;
							case KEY1_PRES:
								OLED_ShowNum(103,6,2,3,16);
							//	BH1750_test();
								break;
							case KEY2_PRES:
								OLED_ShowNum(103,6,3,3,16);
								break;
							case KEY3_PRES:
								OLED_ShowNum(103,6,4,3,16);
								break;
							case KEY4_PRES:
								OLED_ShowNum(103,6,5,3,16);
								break; 
							case KEY5_RRES:
								OLED_ShowNum(103,6,6,3,16);
								break;
							case KEY6_PRES:
								OLED_ShowNum(103,6,7,3,16);
								break;
							case WKUP_PRES:
								OLED_ShowNum(103,6,8,3,16);
								break;
						}
				}else
				{
					delay_ms(10);
				}
		}
}

void Low_Power(void)
{
	Beep_Init();
	for(int i = 0; i < 500; i ++)
	{
		BEEP = ~BEEP;
		delay_ms(1);
	}
	OLED_Clear(); 
	OLED_ShowCHinese(36,3,23);//中
	OLED_ShowCHinese(54,3,24);//景
	OLED_ShowCHinese(72,3,25);//园
	OLED_ShowCHinese(90,3,26);//电
	delay_ms(500);
	OLED_Clear(); 
}

void Read_AmperTest(void)
{
		float result[3];
		char buf[10];
		RS485_Init(4800);	
		TIM2_Int_Init(100-1,8400-1);
		TIM_Cmd(TIM2,DISABLE); //使能定时器2
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//		USART_Cmd(USART2, ENABLE); 
	//	RS485_PD14_Set();
		Modbus_init();
		OLED_ShowCHinese(36,0,37);
		OLED_ShowCHinese(54,0,39);
	//A相电压
		voltage_test(0x49,0x01);
		RS485_Send_Data();
		delay_ms(1000);
		Message_Handler(&result[0]);
	sprintf(buf,"A: %4.3fA",(float)result[0]/100);
		OLED_ShowString(18,2,buf);
	
		Modbus_init();
		voltage_test(0x52,0x01);
		RS485_Send_Data();
		delay_ms(1000);
		Message_Handler(&result[1]);
	sprintf(buf,"B: %4.3fA",(float)result[1]/100);
	OLED_ShowString(18,4,buf);
	
			Modbus_init();
			voltage_test(0x5B,0x01);
		RS485_Send_Data();
		delay_ms(1000);
		Message_Handler(&result[2]);
	sprintf(buf,"C: %4.3fA",(float)result[2]/100);
		OLED_ShowString(18,6,buf);
		
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE);
//		TIM_Cmd(TIM2,DISABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);//使能USART2时钟
//		USART_Cmd(USART2, DISABLE); 

		Sensor_setAmper(p_Sensor,result);
		char*value = Sensor_to_String(p_Sensor);	
		project++;
	//	NandFlash_PD14_Set();
			nand0_drv.Init(&cfg);
		nand0_drv.PageWrite(100*128*1024+project*2048+1,value,&cfg);
			AT24CXX_WriteOneByte(2,project);
			

}

void Read_Voltage(void)
{
		float result[3];
		char buf[10];
		RS485_Init(4800);	
		TIM2_Int_Init(100-1,8400-1);
		TIM_Cmd(TIM2,DISABLE); //使能定时器2
//	
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//		USART_Cmd(USART2, ENABLE); 
//		RS485_PD14_Set();
		Modbus_init();
		//A相电压
		OLED_ShowCHinese(36,0,37);
		OLED_ShowCHinese(54,0,38);
		voltage_test(0x48,0x01);
		RS485_Send_Data();
		delay_ms(1000);
		Message_Handler(&result[0]);
	sprintf(buf,"A: %4.3fV",(float)result[0]/100);
		OLED_ShowString(18,2,buf);
	
		Modbus_init();
		voltage_test(0x51,0x01);
		RS485_Send_Data();
		delay_ms(1000);
		Message_Handler(&result[1]);
	sprintf(buf,"B: %4.3fV",(float)result[1]/100);
	OLED_ShowString(18,4,buf);
	
		Modbus_init();
			voltage_test(0x5A,0x01);
		RS485_Send_Data();
		delay_ms(1000);
		Message_Handler(&result[2]);
	sprintf(buf,"C: %4.3fV",(float)result[2]/100);
		OLED_ShowString(18,6,buf);
//		
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE);
//		TIM_Cmd(TIM2,DISABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);//使能USART2时钟
//		USART_Cmd(USART2, DISABLE); 
		
		
		Sensor_setVoltage(p_Sensor, result);
		char*value = Sensor_to_String(p_Sensor);
		project++;

	nand0_drv.PageWrite(100*128*1024+project*2048+1,value,&cfg);
		AT24CXX_WriteOneByte(2,project);
			
}




//int count = 0;
//int main(void)
//{
//	
//	delay_init(168);  //初始化延时函数
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	LED_Init();		//初始化LED
//	KEY_Init();
//	Beep_Init();
//	WKUP_Init();
//	SDRAM_Init();		
//	nand0_drv.Init(&cfg);
//	
//	//低功耗模式
////	while(1)
////	{
////		
////			LED0 = 0;
////		
////			count ++;
////			delay_ms(30);
////			if(count == 10)
////			{
////					Sys_Enter_Standby();
////					SYSCLKConfig_Stop();
////					Beep_Test();
////			}
////	}
//	OLED_Test();
//	//AM2302_Test();
//	//KS103_test();
//	//BH1750_test();
//	//DS1302_Test();
//	//Key_Test();
//	//Voltage_Test();
//	while(1);


//}

int Get_Power(void)
{
		char buf[10];
		int percent = 0;
//		MyADC_Init();
//		OLED_Init();	
//		OLED_Clear()  	; 
		percent = Get_Battery_Percentage();
	
		switch(percent)
		{
			case 100:
			case 90:
			case 80:
					OLED_DrawBMP(110,0,125,1,full);
					break;
			case 70:
			case 60:
			case 50:
					OLED_DrawBMP(110,0,125,1,half);
					break;
			case 40:
			case 30:
			case 20:
					OLED_DrawBMP(110,0,125,1,low);
					break;
			case 10:
			case 5:
			case 0:
					OLED_DrawBMP(110,0,125,1,Battery); 
					break;

		}
		return percent;
//		sprintf(buf,"%d",percent);
//		OLED_ShowString(0,3,buf);
		
		
}

static float round(float num)
{
		char buf[10];
		sprintf(buf,"%.2f",num);
		sscanf(buf,"%f",&num);
		return num;
}


static void ShowSaving(void)
{
		OLED_ShowCHinese(18,3,40);
		OLED_ShowCHinese(36,3,41);
		OLED_ShowCHinese(54,3,19);
		OLED_ShowCHinese(72,3,20);
		OLED_ShowCHinese(90,3,42);
		OLED_ShowString(108,3,"..");
}

static void ShowSaved(void)
{
		OLED_ShowCHinese(18,3,40);
		OLED_ShowCHinese(36,3,41);
		OLED_ShowCHinese(54,3,43);
		OLED_ShowCHinese(72,3,44);
		OLED_ShowChar(90,3,'!');
}

static void ShowNoDataSave(void)
{
		OLED_ShowCHinese(18,3,47);
		OLED_ShowCHinese(36,3,48);
		OLED_ShowCHinese(54,3,19);
		OLED_ShowCHinese(72,3,20);
		OLED_ShowCHinese(90,3,40);
		OLED_ShowCHinese(108,3,41);
	
}

static void ShowSending(void)
{
		OLED_ShowCHinese(18,3,45);
		OLED_ShowCHinese(36,3,46);
		OLED_ShowCHinese(54,3,19);
		OLED_ShowCHinese(72,3,20);
		OLED_ShowCHinese(90,3,42);
		OLED_ShowString(108,3,"..");
}

static void ShowSendOk(void)
{
		OLED_ShowCHinese(18,3,45);
		OLED_ShowCHinese(36,3,46);
		OLED_ShowCHinese(54,3,43);
		OLED_ShowCHinese(72,3,44);
		OLED_ShowChar(90,3,'!');
}

static void ShowNoDataSend(void)
{
		OLED_ShowCHinese(18,3,47);
		OLED_ShowCHinese(36,3,48);
		OLED_ShowCHinese(54,3,19);
		OLED_ShowCHinese(72,3,20);
		OLED_ShowCHinese(90,3,45);
		OLED_ShowCHinese(108,3,46);

}

/**
 * Env demo.
 */
static void test_env(void) {
    uint32_t i_boot_times = NULL;
		uint32_t m_boot_times = NULL;
    char *c_old_boot_times, c_new_boot_times[11] = {0};
		char* m_old_boot_times, m_new_boot_times[11] = {0};

    /* get the boot count number from Env */
    c_old_boot_times = ef_get_env("temp");
    assert_param(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);
    /* boot count +1 */
    i_boot_times ++;
    UART6_Printf("The system temp %d times\n\r", i_boot_times);
    /* interger to string */
    sprintf(c_new_boot_times,"%ld", i_boot_times);
    /* set and store the boot count number to Env */
    ef_set_env("temp", c_new_boot_times);
    ef_save_env();
		
		    /* get the boot count number from Env */
    m_old_boot_times = ef_get_env("humi");
    assert_param(m_old_boot_times);
    m_boot_times = atol(m_old_boot_times);
    /* boot count +1 */
    m_boot_times ++;
    UART6_Printf("The system now humi %d times\n\r", m_boot_times);
    /* interger to string */
    sprintf(m_new_boot_times,"%ld", i_boot_times);
    /* set and store the boot count number to Env */
    ef_set_env("humi", m_new_boot_times);
    ef_save_env();
}

void test_easyflash(void)
{
	nand0_drv.BlockErase(0,&cfg);
		Sensor *sensor1;
		Sensor *sensor2;
	uint8_t RxBuffer[NAND_PAGE_SIZE];
	sensor1 = Sensor_Create();
	Sensor_setTemperature(sensor1,15.00);
	Sensor_setHumidity(sensor1,25.3);
	Sensor_setLight(sensor1,1234);
	char*value = Sensor_to_String(sensor1);
	save_Sensor(sensor1,"sensor1");
//		nand0_drv.BlockErase(0,&cfg);
//		nand0_drv.PageWrite(0,value,&cfg);
	//	delay_ms(500);
		
//	test_env();
	sensor2 = Sensor_Create();
	Sensor_setTemperature(sensor2,45.00);
	Sensor_setHumidity(sensor2,78.3);
	Sensor_setLight(sensor2,567);

	char*value2 = Sensor_to_String(sensor2);
	//	nand0_drv.BlockErase(4096,&cfg);
	//	nand0_drv.PageWrite(4096,value2,&cfg);
	//	delay_ms(500);
	
//	nand0_drv.PageRead(0,RxBuffer,&cfg);
//		UART6_Printf(RxBuffer);
//		nand0_drv.PageRead(4096,RxBuffer,&cfg);
//		UART6_Printf(RxBuffer);
	save_Sensor(sensor2,"sensor2");
		
}

void get_sensor(void)
{
//		char* boot = ef_get_env("temp");
//		UART6_Printf(boot);
//		char* qq = ef_get_env("humi");
//		UART6_Printf(qq);
		char*value;
		Sensor* p = get_Sensor("sensor1");
		value = Sensor_to_String(p);
		UART6_Printf(value);
//	
		Sensor* q = get_Sensor("sensor2");
		value = Sensor_to_String(q);
		UART6_Printf(value);
//	
}


////////////////////////////////////////////////////////
static void show_message(void)
{
				OLED_Clear(); 
				OLED_ShowCHinese(0,0,14);//中
				OLED_ShowCHinese(18,0,15);//景
				OLED_ShowCHinese(36,0,16);//园
				OLED_ShowCHinese(54,0,17);//电
				OLED_ShowCHinese(72,0,18);//子
				OLED_ShowCHinese(90,0,19);//科
				OLED_ShowCHinese(108,0,20);//技
	
				OLED_ShowChar(18,2,'1');
				OLED_ShowChar(36,2,':');
				OLED_ShowCHinese(54,2,21);
				OLED_ShowChar(72,2,'2');
				OLED_ShowChar(90,2,':');
				OLED_ShowCHinese(108,2,22);
		
}

static void SetLowPower(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
														RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);
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
}
void DrawWelcome(void)
{
		OLED_Clear(); 
		//OLED_ShowString(0,1,"-*-*-*-*-*-*-*-*");
		OLED_ShowCHinese(18,2,27); //电
		OLED_ShowCHinese(36,2,28); //梯
		OLED_ShowCHinese(54,2,29); //安
		OLED_ShowCHinese(72,2,30); //全
		OLED_ShowCHinese(90,2,31); //检
	  OLED_ShowCHinese(108,2,32); //测
		OLED_ShowCHinese(36,4,33); //手
		OLED_ShowCHinese(54,4,34); //持
		OLED_ShowCHinese(72,4,35); //设
		OLED_ShowCHinese(90,4,36); //备
		OLED_ShowString(0,7," * * * * * * * *");
}
//////////////////////////////////////////////////////////////////////

int stop = 0;
u8 key = 0;

int main()
{
		u8 key = 0;
		int count = 0;
		int humi = 0;
		int temper = 0;
		float light = 0;
		float height = 0;
		float base = 0;
		sTime stime;
		char buf[10];
		uint8_t RxBuffer[NAND_PAGE_SIZE];
		int power;
		u8 Clear_flag = 0;
		//用于在循环中要多次按键的情况
		u8 time = 0;
		SetLowPower();
		delay_init(168);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
		OLED_Init();			//初始化OLED  
		OLED_Clear()  	; 
		DS1302_Init();
	//	MyADC_Init();
		KEY_Init();
	//	Beep_Init();
	//	WKUP_Init();
		//LED_Init();		//初始化LED
		//Main_Init();
		//uart6_init(115200);
		RS485_Init(4800);	
		TIM2_Int_Init(100-1,8400-1);
		TIM_Cmd(TIM2,DISABLE); //使能定时器2

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);//使能USART2时钟
		USART_Cmd(USART2, DISABLE); 		
		nand0_drv.Init(&cfg);

	//用于数据备份区的存储
	
		AT24CXX_Init();
		my_mem_init(CCM);	
		my_mem_init(SRAMIN);

		p_Sensor = Sensor_Create();

		//读取备份区，看是否有数据
		
		u8 p = AT24CXX_ReadOneByte(2);	
		nand0_drv.PageRead(100*128*1024+p*2048+1,RxBuffer,&cfg);
		if(p != 0)
		{
			show_message();
			for(u8 i = 0; i < 400; i ++)
			{			
					key = KEY_Scan(0);
					if(key == KEY0_PRES)
					{
							p_Sensor = get_Stuct(RxBuffer);
							project = p;
							key = 0;
							Clear_flag  = 1;
							break;
					}
					else if(key == KEY1_PRES)
					{
							nand0_drv.BlockErase(100*128*1024,&cfg);
							AT24CXX_WriteOneByte(2,0);
							key = 0;
							break;
					}
					else continue;
					delay_ms(10);
			}
		}
		
		if(Clear_flag == 0)
		{
				nand0_drv.BlockErase(100*128*1024,&cfg);
				AT24CXX_WriteOneByte(2,0);
				key = 0;
		}
		TIM4_Int_Init(5000-1,8400 -1);
	
		DrawWelcome();
		
		while(1)
		{
			
			if(key == 0)
			{
					timer_count = 0;
					TIM_Cmd(TIM4,DISABLE);
					TIM_SetCounter(TIM4,0);
					TIM_Cmd(TIM4,ENABLE);
					DrawWelcome();
					//OLED_DrawBMP(0,0,128,8,BMP1); 
					while(1)
					{
							
							key = KEY_Scan(0);
							count ++;
							if(count == 500)
							{
									power = Get_Battery_Percentage();
									if(power == 5)
									{
											Low_Power();
											//OLED_DrawBMP(0,0,128,8,BMP1); 
											DrawWelcome();
									}
									count = 0;
							}
							if(key && key != WKUP_PRES) break;
							if(stop == 1)
							{
									Sys_Enter_Standby();
									Main_Init();
									stop = 0;
									key = 0;
									break;
							}
							delay_ms(10);
						
					}
			}
			else if(key == KEY0_PRES)
			{
					
					OLED_Clear();
					count = 0;
					timer_count = 0;
					power = Get_Power();
					AM_Init();
					//AM2302_Test(&temper,&humi);
					AM2302_Init();
					TIM_Cmd(TIM4,DISABLE);
					TIM_SetCounter(TIM4,0);
					TIM_Cmd(TIM4,ENABLE);
				//	OLED_ShowNum(103,6,Get_Battery_Percentage(),3,16);
					
					while(1)
					{
							//OLED_Clear();
							
							count ++;
							if(count%100 == 0)
							{
								
								AM2302_Test(&temper,&humi);
								//count = 0;
							}
							//1分钟测一次电量，如果等于10就报警
							if(count == 500)
							{
									power = Get_Power();
									if(power == 5)
									{
											Low_Power();
									}
									count = 0;
							}
							key = KEY_Scan(0);
							if(key != 1 && key != 0 && key != WKUP_PRES) 
							{
								project ++;
								Sensor_setTemperature(p_Sensor,(float)temper/10 - 6.5);
								Sensor_setHumidity(p_Sensor,(float)humi/10);
								char*value = Sensor_to_String(p_Sensor);
								nand0_drv.PageWrite(100*128*1024+project*2048+1,value,&cfg);
								AT24CXX_WriteOneByte(2,project);
								//AM_Disable();
								break;
							}
							if(stop == 1)
							{
									Sys_Enter_Standby();
									Main_Init();
									stop = 0;
									key = 1;
									break;
							}
							delay_ms(10);
					}
			}
			else if(key == KEY1_PRES)
			{
				OLED_Clear();
				//AM_Disable();
				count = 0;
				timer_count = 0;
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
				KS103_Init();
				BH1750_Init();
				BH1750_test(&light);

				TIM_Cmd(TIM4,ENABLE);
				power = Get_Power();
			//	OLED_ShowNum(103,6,Get_Battery_Percentage(),3,16);
					while(1)
					{
						
							time = 1;
							//OLED_Clear();
						//	OLED_ShowNum(103,6,2,3,16);
							count ++;
							if(count%30 == 0)
							{
								BH1750_test(&light);
								//count = 0;
							}
							if(count == 500)
							{
									OLED_Clear();
									power = Get_Power();
							}
							if(count == 4000)
							{
									power = Get_Power();
									if(power == 5)
									{
											Low_Power();
									}
									count = 0;
							}
							key = KEY_Scan(0);
							if(key == 2)
							{
									TIM_Cmd(TIM4,DISABLE);
									TIM_SetCounter(TIM4,0);
									project ++;
									BH1750_test(&light);
									Sensor_setLight(p_Sensor,(light));
									char*value = Sensor_to_String(p_Sensor);
									nand0_drv.PageWrite(100*128*1024+project*2048+1,value,&cfg);
									AT24CXX_WriteOneByte(2,project);
									TIM_Cmd(TIM4,ENABLE);
							}
							if(key != 2&& key != 0&& key != WKUP_PRES)								
							{
							//	KS103_Disable();
							//	BH1750_Disable();
								break;
							}
							if(stop == 1)
							{
									Sys_Enter_Standby();
									Main_Init();
									stop = 0;
									key = 2;
									break;
							}
							delay_ms(10);							
					}
			}
			else if(key == KEY2_PRES)
			{
				count = 0;
				timer_count = 0;
//				AM_Disable();
//				BH1750_Disable();
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
				
				KS103_Init();
				OLED_Clear();
				base = 0;
				power = Get_Power();
				KS103_test(&base);

				TIM_Cmd(TIM4,ENABLE);
				
				//OLED_ShowNum(103,6,Get_Battery_Percentage(),3,16);
			//	KS103_test();
					while(1)
					{
							key = KEY_Scan(0);
							count ++;
//							if(count == 500)
//							{
//									OLED_Clear();
//							}
							if(count == 4000)
							{
									
									power = Get_Power();
									if(power == 5)
									{
											Low_Power();
									}
									count = 0;
							}
							if(key == 3)
							{
									TIM_Cmd(TIM4,DISABLE);
									TIM_SetCounter(TIM4,0);
									project ++;
									height = 0;
									KS103_test(&height);
									sprintf(buf,"%4.1fmm",(float)(height-base));
									OLED_ShowString(36,5,buf);  
									Sensor_setDistance(p_Sensor,height - base);
									
									char*value = Sensor_to_String(p_Sensor);
									nand0_drv.PageWrite(100*128*1024+project*2048+1,value,&cfg);
									AT24CXX_WriteOneByte(2,project);
									power = Get_Power();
									
									TIM_Cmd(TIM4,ENABLE);
							}
							if(key != 3&& key != 0&& key != WKUP_PRES)
							{
									//KS103_Disable();
									break;
							}
							if(stop == 1)
							{
									Sys_Enter_Standby();
									Main_Init();
									stop = 0;
									key = 3;
									break;
							}
							delay_ms(10);							
					}
			}
			else if(key == KEY3_PRES)
			{
				timer_count = 0;
				OLED_Clear();
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
				power = Get_Power();
		//		RS485_Init(4800);		
				Read_Voltage();
			//	RS485_PD14_Set();
				//Voltage_Test();
				
			//	OLED_ShowNum(103,6,Get_Battery_Percentage(),3,16);
				
				
				TIM_Cmd(TIM4,ENABLE);
					while(1)
					{
							key = KEY_Scan(0);
							count ++;
							if(count == 4000)
							{
									power = Get_Power();
									if(power == 5)
									{
											Low_Power();
									}
									count = 0;
							}
							if(key != 4&& key != 0&& key != WKUP_PRES) break;
							if(stop == 1)
							{
									Sys_Enter_Standby();
									Main_Init();
									stop = 0;
									key = 4;
									break;
							}
							delay_ms(10);
					}
			}
			else if(key == KEY4_PRES)
			{
				timer_count = 0;
				OLED_Clear();
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
				power = Get_Power();
			//	RS485_Init(4800);				
				Read_AmperTest();
			//	Voltage_Test();
			//	Amper_Test();
				
			//	OLED_ShowNum(103,6,Get_Battery_Percentage(),3,16);
				TIM_Cmd(TIM4,ENABLE);
				while(1)
						{
							//OLED_Clear();
							//OLED_ShowNum(103,6,5,3,16);
							key = KEY_Scan(0);
							count ++;
							if(count == 4000)
							{
									power = Get_Power();
									if(power == 5)
									{
											Low_Power();
									}
									count = 0;
							}
							if(key != 5&& key != 0&& key != WKUP_PRES) break;
							if(stop == 1)
							{
									Sys_Enter_Standby();
									Main_Init();
									stop = 0;
									key = 5;
									break;
							}
							delay_ms(10);
						}
			}
			else if(key == KEY5_RRES)
			{
				OLED_Clear();
				timer_count = 0;
				power = Get_Power();
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
				
			//	OLED_ShowNum(103,6,Get_Battery_Percentage(),3,16);
			//	OLED_ShowString(18,4,"Saving...");
				u8 p = AT24CXX_ReadOneByte(2);	
				if(p == 0)
				{
						ShowNoDataSave();
						delay_ms(3000);
						key = 0;
				}
				else
				{
						
						ShowSaving();
						
						GetRealTime(&stime);	
						char* date = date2string(&stime);
						Sensor_setDate(p_Sensor,date);
						char*value = Sensor_to_String(p_Sensor);
						int index = AT24CXX_ReadOneByte(0);		
						index = index +1;
						sprintf(buf,"%d",index);
						nand0_drv.PageWrite(index*2048+1,value,&cfg);
						AT24CXX_WriteOneByte(0,(u8)index);
						AT24CXX_WriteOneByte(2,0);
						delay_ms(500);
						OLED_Clear();
					//	OLED_ShowString(18,4,"Save Okay!");
						ShowSaved();
						power = Get_Power();
						//如果存储成功，就将备份区的数据删除
						Sensor_Clean(p_Sensor);
						nand0_drv.BlockErase(100*128*1024,&cfg);
						//新增加的语句
						project = 0;
						TIM_Cmd(TIM4,ENABLE);	
						delay_ms(1000);
						key = 0;
				}
				
			}
			else if(key == KEY6_PRES)
			{
				char* char_value;
				OLED_Clear();
				timer_count = 0;
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
				power = Get_Power();
				u8 index = AT24CXX_ReadOneByte(2);
				if(index == 0)
				{
						ShowNoDataSend();
						delay_ms(3000);
						key = 0;
				}
				else
				{
						uart6_init(115200);
						ShowSending();
						
						///////////////单次发送加入保存的代码///////////////////////
						GetRealTime(&stime);	
						char* date = date2string(&stime);
						Sensor_setDate(p_Sensor,date);
						char*value = Sensor_to_String(p_Sensor);
						UART6_Printf(value);
//						int index = AT24CXX_ReadOneByte(0);		
//						index = index +1;
//						sprintf(buf,"%d",index);
//						nand0_drv.PageWrite(index*2048+1,value,&cfg);
//						AT24CXX_WriteOneByte(0,(u8)index);
						AT24CXX_WriteOneByte(2,0);
						Sensor_Clean(p_Sensor);
						nand0_drv.BlockErase(100*128*1024,&cfg);
					
//						for(u8 i = index; i >= 1; i --)
//						{
//							nand0_drv.PageRead(i*2048+1,RxBuffer,&cfg);
//							UART6_Printf(RxBuffer);
//							memset(RxBuffer,0,sizeof(RxBuffer));
//							delay_ms(500);
//							sprintf(buf,"Sending...%d",i);
//							OLED_ShowString(18,5,buf);
//						}

						delay_ms(500);
						OLED_Clear();
				//		OLED_ShowString(18,4,"Send Okay!");
						power = Get_Power();
						ShowSendOk();
						
						
						//当发送成功后删除存储区的值
//						nand0_drv.BlockErase(0,&cfg);
//						AT24CXX_WriteOneByte(0,0);
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,DISABLE);
						USART_Cmd(USART6, DISABLE);
						TIM_Cmd(TIM4,ENABLE);	
						delay_ms(1000);
						key = 0;
				}
				
				
				
			//	OLED_ShowNum(103,6,Get_Battery_Percentage(),3,16);
			//	OLED_ShowString(18,4,"Sending...");
			
//					while(1)
//					{							
//							key = KEY_Scan(0);
//							count ++;
//							if(count == 4000)
//							{
//									power = Get_Power();
//									if(power == 5)
//									{
//											Low_Power();
//									}
//									count = 0;
//							}
//							if(key != 7&& key != 0&& key != WKUP_PRES) break;
//							if(stop == 1)
//							{
//									Sys_Enter_Standby();
//									Main_Init();
//									stop = 0;
//									key = 0;
//									break;
//							}
//							delay_ms(10);
//					}
			}


		}
}
