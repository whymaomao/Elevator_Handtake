#include "AD.h"
#include "delay.h"

//ͨ����ѹ������﮵������
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



/*ADC��ʼ�� PA5 --> ADC12_IN5*/
void MyADC_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		ADC_CommonInitTypeDef ADC_CommonInitStructure;
		ADC_InitTypeDef ADC_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	//ʹ��GPIOAʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);   //ʹ��ADCʱ��
	
		//��ʼ��AD��GPIO��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  //ģ������
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE); //ADC1��λ
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);//ADC1��λ����
	
		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; //����ģʽ
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; //Ԥ��Ƶ4��Ƶ��ADCCLK = PCLK2/84 = 84/4 = 21Mhz
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //���������׶�֮���ӳ�5��ʱ��
		ADC_CommonInit(&ADC_CommonInitStructure);
		
		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //12λģʽ
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //��ɨ��ģʽ
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //�Ҷ���
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		ADC_InitStructure.ADC_NbrOfConversion = 1;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE; //��ɨ��ģʽ
		ADC_Init(ADC1, &ADC_InitStructure);
		
		ADC_Cmd(ADC1,ENABLE);
		
}

//���ADCֵ
//ch:ADC_Channels
//ͨ��ֵ0~16ȡֵΪ��ADC_Channel0~ADC_Channel16
//����ֵ��ת�����
u16  Get_Adc(u8 ch)
{
		MyADC_Init();
		//����ָ����ADC������ͨ����һ�����У�����ʱ��
		ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_480Cycles);  //ADC1,480������
		ADC_SoftwareStartConv(ADC1);  //ʹ��ָ����ADC1���ת����������
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));  //�ȴ�ת������
		return ADC_GetConversionValue(ADC1);  //����ת�����
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,DISABLE); 
			ADC_Cmd(ADC1,DISABLE);
}

//��ȡͨ��ch��ת��ֵ��ȡtimes,Ȼ��ƽ��
//ch:ͨ�����
//times:��ȡ����
//����ֵ��ͨ��ch��times��ת�����ֵ
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

//��ȡ����ش�ʱ�ĵ�ѹֵ
float Get_Battery_Voltage(void)
{
		u16 adcx;
		adcx = Get_Adc_Average(ADC_Channel_5,20); //20��ȡƽ��
		return (float)2*adcx*(3.3/4096);
}

//���ص�ǰ�ĵ��������ʹ�ò��ʵ��
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

