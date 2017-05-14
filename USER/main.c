#include "main.h"
#include "bmp.h"

//用于发送采集数据的结构体
Sensor *p_Sensor;
//用于显示备份区的页面
static u8 project = 0;
//nandflash结构体
extern NAND_DRV nand0_drv;
extern NAND_DRV_CFG cfg;
//计时器计数
extern int timer_count;
//用于指示是否进入低功耗
int stop = 0;
//用于指示当前的按键值


///////////////////////////////功能函数//////////////////////////
////HY::用于在唤醒后重新初始化必要的外设
static void Main_Init(void)
{
	
	delay_init(168);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	DS1302_Init();
	KEY_Init();
	OLED_Init();			
	TIM4_Int_Init(5000-1,8400 -1);
	nand0_drv.Init(&cfg);
	AT24CXX_Init();	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);
	USART_Cmd(USART2, DISABLE); 
	
}

////HY::用于设置低功耗模式
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

////HY::初始化温湿度传感器///////////////////////
static void AM2302_Init(void)
{
	char buf[20];
	OLED_ShowCHinese(18,2,0);//
	OLED_ShowCHinese(36,2,1);//
	sprintf(buf,"%4.1f",0.0);
	OLED_ShowString(60,2,buf);
	OLED_ShowCHinese(96,2,11);
	OLED_ShowCHinese(18,5,2);//
	OLED_ShowCHinese(36,5,1);//
	sprintf(buf,"%4.1f",0.0);
	OLED_ShowString(60,5,buf);
	OLED_ShowString(96,5,"%");
}


////HY:用于温湿度传感器采集
static void AM2302_Test(int *t, int *h)
{
		int temper = 0;
		int humi = 0;
		*t = 0;
		*h = 0;
		u8 check;
		char buf[20];

		OLED_ShowCHinese(18,2,0);//
		OLED_ShowCHinese(36,2,1);//
		AM_Read_Data(&temper,&humi,&check);
				
		if(temper > (*t)) *t = temper;
		if(humi > (*h)) *h = humi;
		sprintf(buf,"%4.1f",(double)temper/10 - 6.5);
				
		OLED_ShowString(60,2,buf);
		OLED_ShowCHinese(96,2,11);
		OLED_ShowCHinese(18,5,2);//
		OLED_ShowCHinese(36,5,1);//
		sprintf(buf,"%4.1f",((double)humi/10));
			
		OLED_ShowString(60,5,buf);  
		OLED_ShowString(96,5,"%");
}


////HY::用于光照传感器采集数据
void BH1750_test(float*light)
{

		char buf[10];
	//	double light = 0;
		float num;
		u8 i = 0;
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
		OLED_ShowCHinese(18,5,12);//光
		OLED_ShowCHinese(36,5,13);//强
		*light = get_light();
		sprintf(buf, "%4.1flux", *light);
		OLED_ShowString(54,5,buf);
}

////HY::用于超声波传感器采集
static void KS103_test(float*num)
{

	u8 i = 0;
	char buf[10];

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
	sprintf(buf,"%4.1fmm",(float)*num);
	OLED_ShowString(36,5,buf);  
}


////HY::用于读取电流
static void Read_AmperTest(void)
{
	float result[3];
	char buf[10];
	RS485_Init(4800);	
	TIM2_Int_Init(100-1,8400-1);
	TIM_Cmd(TIM2,DISABLE); //

	OLED_ShowCHinese(36,0,37);
	OLED_ShowCHinese(54,0,39);

	Modbus_init();
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
		
	Sensor_setAmper(p_Sensor,result);
	char*value = Sensor_to_String(p_Sensor);	
	project++;

	nand0_drv.Init(&cfg);
	nand0_drv.PageWrite(100*128*1024+project*2048+1,value,&cfg);
	AT24CXX_WriteOneByte(2,project);
			
}



////HY::用于读取电压
static void Read_Voltage(void)
{
	float result[3];
	char buf[10];
	RS485_Init(4800);	
	TIM2_Int_Init(100-1,8400-1);
	TIM_Cmd(TIM2,DISABLE); //


	OLED_ShowCHinese(36,0,37);
	OLED_ShowCHinese(54,0,38);

	Modbus_init();
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

	Sensor_setVoltage(p_Sensor, result);
	char*value = Sensor_to_String(p_Sensor);
	project++;

	nand0_drv.PageWrite(100*128*1024+project*2048+1,value,&cfg);
	AT24CXX_WriteOneByte(2,project);	
}


////////////////////////显示功能函数//////////////////////////
////HY::用于在电量低时的显示
static void Low_Power(void)
{
	Beep_Init();
	for(int i = 0; i < 500; i ++)
	{
		BEEP = ~BEEP;
		delay_ms(1);
	}
	OLED_Clear(); 
	OLED_ShowCHinese(36,3,23);//
	OLED_ShowCHinese(54,3,24);//
	OLED_ShowCHinese(72,3,25);//
	OLED_ShowCHinese(90,3,26);//
	delay_ms(500);
	OLED_Clear(); 
}


////HY::用于显示电池电量图片
int Get_Power(void)
{
		char buf[10];
		int percent = 0;

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
}

////HY::用于显示正在发送
static void ShowSending(void)
{
		OLED_ShowCHinese(18,3,45);
		OLED_ShowCHinese(36,3,46);
		OLED_ShowCHinese(54,3,19);
		OLED_ShowCHinese(72,3,20);
		OLED_ShowCHinese(90,3,42);
		OLED_ShowString(108,3,"..");
}

////HY::用于显示发送成功
static void ShowSendOk(void)
{
		OLED_ShowCHinese(18,3,45);
		OLED_ShowCHinese(36,3,46);
		OLED_ShowCHinese(54,3,43);
		OLED_ShowCHinese(72,3,44);
		OLED_ShowChar(90,3,'!');
}

////HY::用于显示无数据发送
static void ShowNoDataSend(void)
{
		OLED_ShowCHinese(18,3,47);
		OLED_ShowCHinese(36,3,48);
		OLED_ShowCHinese(54,3,19);
		OLED_ShowCHinese(72,3,20);
		OLED_ShowCHinese(90,3,45);
		OLED_ShowCHinese(108,3,46);

}


////HY::用于显示开机前是否要更新数据
static void show_message(void)
{
	OLED_Clear(); 
	OLED_ShowCHinese(0,1,14);//
	OLED_ShowCHinese(18,1,15);//
	OLED_ShowCHinese(36,1,16);//
	OLED_ShowCHinese(54,1,17);//
	OLED_ShowCHinese(72,1,18);//
	OLED_ShowCHinese(90,1,19);//
	OLED_ShowCHinese(108,1,20);//
	
	OLED_ShowChar(36,4,'1');
	OLED_ShowChar(54,4,':');
	OLED_ShowCHinese(72,4,21);
	OLED_ShowChar(36,6,'2');
	OLED_ShowChar(54,6,':');
	OLED_ShowCHinese(72,6,22);
		
}

////HY::用于显示确认发送数据
static void AskSendData(void)
{
	OLED_Clear(); 
	OLED_ShowCHinese(0,1,49);
	OLED_ShowCHinese(18,1,50);
	OLED_ShowCHinese(36,1,45);
	OLED_ShowCHinese(54,1,46);
	OLED_ShowCHinese(72,1,19);
	OLED_ShowCHinese(90,1,20);
	OLED_ShowChar(108,1,'?');
	
	OLED_ShowChar(36,4,'1');
	OLED_ShowChar(54,4,':');
	OLED_ShowCHinese(72,4,21);
	OLED_ShowChar(36,6,'2');
	OLED_ShowChar(54,6,':');
	OLED_ShowCHinese(72,6,22);
}
////HY::用于显示开机界面
static void DrawWelcome(void)
{
		OLED_Clear(); 
		//OLED_ShowString(0,1,"-*-*-*-*-*-*-*-*");
		OLED_ShowCHinese(18,2,27); //
		OLED_ShowCHinese(36,2,28); //
		OLED_ShowCHinese(54,2,29); //
		OLED_ShowCHinese(72,2,30); //
		OLED_ShowCHinese(90,2,31); //
	  OLED_ShowCHinese(108,2,32); //
		OLED_ShowCHinese(36,4,33); //
		OLED_ShowCHinese(54,4,34); //
		OLED_ShowCHinese(72,4,35); //
		OLED_ShowCHinese(90,4,36); //
		OLED_ShowString(0,7," * * * * * * * *");
}


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
		
		u8 time = 0;
		SetLowPower();
		delay_init(168);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//
		OLED_Init();			// 
		OLED_Clear()  	; 
		DS1302_Init();
		KEY_Init();

		RS485_Init(4800);	
		TIM2_Int_Init(100-1,8400-1);
		TIM_Cmd(TIM2,DISABLE); //

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);//
		USART_Cmd(USART2, DISABLE); 		
		nand0_drv.Init(&cfg);
		AT24CXX_Init();
		my_mem_init(CCM);	
		my_mem_init(SRAMIN);

		p_Sensor = Sensor_Create();

		
		
		u8 p = AT24CXX_ReadOneByte(2);	
		nand0_drv.PageRead(100*128*1024+p*2048+1,RxBuffer,&cfg);
		if(p != 0)
		{
			show_message();
			for(u8 i = 0; i < 200; i ++)
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
					DrawWelcome();
					timer_count = 0;
					TIM_Cmd(TIM4,DISABLE);
					TIM_SetCounter(TIM4,0);
					TIM_Cmd(TIM4,ENABLE);
				//	DrawWelcome();
		
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
					AM2302_Init();
					TIM_Cmd(TIM4,DISABLE);
					TIM_SetCounter(TIM4,0);
					TIM_Cmd(TIM4,ENABLE);
					
					while(1)
					{
	
							count ++;
							if(count%100 == 0)
							{
								
								AM2302_Test(&temper,&humi);
								//count = 0;
							}
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

					while(1)
					{
						
							time = 1;
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
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
				
				KS103_Init();
				OLED_Clear();
				base = 0;
				power = Get_Power();
				KS103_test(&base);

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
				Read_Voltage();				
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
				Read_AmperTest();
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
			else if(key == KEY6_PRES)
			{
				TIM_Cmd(TIM4,DISABLE);
				timer_count = 0;
				Sys_Enter_Standby();
				SYSCLKConfig_Stop();
				Main_Init();
				stop = 0;
				key = 0;
			}
			else if(key == KEY5_RRES)
			{
				char* char_value;
				OLED_Clear();
				power = Get_Power();
				timer_count = 0;
				TIM_Cmd(TIM4,DISABLE);
				TIM_SetCounter(TIM4,0);
			
				u8 index = AT24CXX_ReadOneByte(2);
				if(index == 0)
				{
						ShowNoDataSend();
						delay_ms(1000);
						key = 0;
				}
				else
				{
					AskSendData();
					
					for(int i = 0; i < 300; i ++)
					{
							key = KEY_Scan(0);
							if(key == KEY1_PRES)
							{
									key = 0;
									break;
							}
							else if(key == KEY0_PRES)
							{
									uart6_init(115200);
									OLED_Clear();
									ShowSending();
									GetRealTime(&stime);	
									char* date = date2string(&stime);
									Sensor_setDate(p_Sensor,date);
									char*value = Sensor_to_String(p_Sensor);
									UART6_Printf(value);

									AT24CXX_WriteOneByte(2,0);
									Sensor_Clean(p_Sensor);
									nand0_drv.BlockErase(100*128*1024,&cfg);
									
								

									delay_ms(500);
									OLED_Clear();
									power = Get_Power();
									ShowSendOk();

									RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,DISABLE);
									USART_Cmd(USART6, DISABLE);
									TIM_Cmd(TIM4,ENABLE);	
									delay_ms(1000);
									key = 0;
									break;
							}
							delay_ms(10);
					}
					key = 0;
				}

			}
		}
}