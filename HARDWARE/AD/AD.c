#include "AD.h"
#include "delay.h"

//通过电压法测量锂电池容量
struct battery_voltage 
{
		int percentage;
		float v;
};

struct battery_voltage bv[] = 
{
		{100,4.2},{90,4.06},{80,3.98},{70,3.92},
		{60,3.87},{50,3.82},{40,3.79},{30,3.77},
		{20,3.74},{10,3.68},{5,3.45},{0,3.00},
};



/*ADC初始化 PA5 --> ADC12_IN5*/
void MyADC_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		ADC_CommonInitTypeDef ADC_CommonInitStructure;
		ADC_InitTypeDef ADC_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	//使能GPIOA时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);   //使能ADC时钟
	
		//初始化AD的GPIO口
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  //模拟输入
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE); //ADC1复位
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);//ADC1复位结束
	
		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; //预分频4分频。ADCCLK = PCLK2/84 = 84/4 = 21Mhz
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //两个采样阶段之间延迟5个时钟
		ADC_CommonInit(&ADC_CommonInitStructure);
		
		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //12位模式
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //非扫描模式
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //右对齐
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		ADC_InitStructure.ADC_NbrOfConversion = 1;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE; //非扫描模式
		ADC_Init(ADC1, &ADC_InitStructure);
		
		ADC_Cmd(ADC1,ENABLE);
		
}

//获得ADC值
//ch:ADC_Channels
//通道值0~16取值为：ADC_Channel0~ADC_Channel16
//返回值：转换结果
u16  Get_Adc(u8 ch)
{
		MyADC_Init();
		//设置指定的ADC规则组通道，一个序列，采样时间
		ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_480Cycles);  //ADC1,480个周期
		ADC_SoftwareStartConv(ADC1);  //使能指定的ADC1软件转换启动功能
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));  //等待转换结束
		return ADC_GetConversionValue(ADC1);  //返回转换结果
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,DISABLE); 
			ADC_Cmd(ADC1,DISABLE);
}

//获取通道ch的转换值，取times,然后平均
//ch:通道编号
//times:获取次数
//返回值：通道ch的times次转换结果值
u16 Get_Adc_Average(u8 ch, u8 times)
{
		u32 temp_val = 0;
		u8 t;
		for(t = 0; t < times; t ++)
	{
			temp_val += Get_Adc(ch);
			delay_ms(5);
	}
	return temp_val/times;
}

//获取充电电池此时的电压值
float Get_Battery_Voltage(void)
{
		u16 adcx;
		adcx = Get_Adc_Average(ADC_Channel_5,20); //20次取平均
		return (float)2*adcx*(3.3/4096);
}

//返回当前的电池容量，使用查表法实现
int Get_Battery_Percentage(void)
{
		unsigned int i;
		float vv = Get_Battery_Voltage();
	  for( i = 0; i < 11; i ++)
		{
				if(bv[i].v >= vv && bv[i+1].v < vv)
					return bv[i].percentage;
					
		}
		return 0;	
}

