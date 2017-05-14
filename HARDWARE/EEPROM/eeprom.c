#include "eeprom.h"
#include "delay.h"


//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	EIIC_Init();//IIC��ʼ��
}


u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{                 
    u8 temp=0;                                                                               
    EIIC_Start();  
    if(EE_TYPE>AT24C16)
    {
        EIIC_Send_Byte(0XA0);       
        EIIC_Wait_Ack();
        EIIC_Send_Byte(ReadAddr>>8);      
    }else EIIC_Send_Byte(0XA0+((ReadAddr/256)<<1));        
    EIIC_Wait_Ack(); 
    EIIC_Send_Byte(ReadAddr%256);   
    EIIC_Wait_Ack();     
    EIIC_Start();           
    if(EE_TYPE>AT24C16)EIIC_Send_Byte(0XA1);        
    else EIIC_Send_Byte(0XA1+((ReadAddr/256)<<1));   
    EIIC_Wait_Ack();  
    temp=EIIC_Read_Byte(0);         
    EIIC_Stop();    
    return temp;
}

void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{                                                                                            
    EIIC_Start();  
    if(EE_TYPE>AT24C16)
    {
        EIIC_Send_Byte(0XA0);        
        EIIC_Wait_Ack();
        EIIC_Send_Byte(WriteAddr>>8);    
    }else EIIC_Send_Byte(0XA0+((WriteAddr/256)<<1));       
    EIIC_Wait_Ack();    
    EIIC_Send_Byte(WriteAddr%256);   
    EIIC_Wait_Ack();                                                        
    EIIC_Send_Byte(DataToWrite);                              
    EIIC_Wait_Ack();                    
    EIIC_Stop();
    delay_ms(10);    
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
		delay_ms(100);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c08Ϊ0~1024
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

u8 EEPROM_Test(void)
{
		u8 pbuffer[10] = {0,1,2,3,4,5,6,7,8,9};
		u8 temp[10];
		AT24CXX_Write(0,pbuffer,10);
		delay_ms(100);
		AT24CXX_Read(0,temp,10);
		for(u8 i = 0; i < 10 ; i++)
		{
				if(temp[i] != pbuffer[i])
					return 0;
		}
		return 1;
}