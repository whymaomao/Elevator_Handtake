#include "sdram.h"
#include "delay.h"
/* Status variables */
__IO uint32_t uwWriteReadStatus ;
/* Counter index */
uint32_t uwIndex ;
uint32_t aTxBuffer[BUFFER_SIZE];
uint32_t aRxBuffer[BUFFER_SIZE];

/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+--------------------+--------------------+
 +                       SDRAM pins assignment                                      +
 +-------------------+--------------------+--------------------+--------------------+
 | PD0  <-> FMC_D2   | PE0  <-> FMC_NBL0  | PF0  <-> FMC_A0    | PG0  <-> FMC_A10   |
 | PD1  <-> FMC_D3   | PE1  <-> FMC_NBL1  | PF1  <-> FMC_A1    | PG1  <-> FMC_A11   |
 | PD8  <-> FMC_D13  | PE7  <-> FMC_D4    | PF2  <-> FMC_A2    | PG2  <-> FMC_A12   |
 | PD9  <-> FMC_D14  | PE8  <-> FMC_D5    | PF3  <-> FMC_A3    | PG8  <-> FMC_SDCLK |
 | PD10 <-> FMC_D15  | PE9  <-> FMC_D6    | PF4  <-> FMC_A4    | PG15 <-> FMC_NCAS  |
 | PD14 <-> FMC_D0   | PE10 <-> FMC_D7    | PF5  <-> FMC_A5    | PG4  <-> FMC_A14BA0|   
 | PD15 <-> FMC_D1   | PE11 <-> FMC_D8    | PF11 <-> FMC_NRAS  | PG5  <-> FMC_A15BA1|
 +-------------------| PE12 <-> FMC_D9    | PF12 <-> FMC_A6    | --------------------
                     | PE13 <-> FMC_D10   | PF13 <-> FMC_A7    |    
                     | PE14 <-> FMC_D11   | PF14 <-> FMC_A8    |
                     | PE15 <-> FMC_D12   | PF15 <-> FMC_A9    |
 +-------------------+--------------------+--------------------+
 | PB5 <-> FMC_SDCKE1| 
 | PB6 <-> FMC_SDNE1 | 
 | PC0 <-> FMC_SDNWE |
 +-------------------+  
  
*/

void SDRAM_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
		FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|\
														RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG,ENABLE);
		RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC,ENABLE);//使能FSMC时钟
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//普通输出模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;//100MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	
		/* GPIOB configuration */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		/* GPIOC configuration */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		/* GPIOD configuration */
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_8 |
                                GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                GPIO_Pin_15;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		 /* GPIOE configuration */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_7 |
                                GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		/* GPIOF configuration */
		GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_2 | 
                                GPIO_Pin_3  | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;  
	  GPIO_Init(GPIOF, &GPIO_InitStructure);		
		
		/* GPIOG configuration */
	  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 |
                                GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_15;
		GPIO_Init(GPIOG,&GPIO_InitStructure);
		
		//复用输出
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_FMC);		
		
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource0,GPIO_AF_FMC);
		
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FMC);
		
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FMC);
	  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FMC);
		
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource2,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource5,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource11,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource13,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource14,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOF,GPIO_PinSource15,GPIO_AF_FMC);
		
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource4,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource5,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource8,GPIO_AF_FMC);
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource15,GPIO_AF_FMC);
		
		/*--------------FMC Configurarion--------------*/
		FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 6;
		FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay = 2;
		FMC_SDRAMTimingInitStructure.FMC_RCDDelay = 2;
		FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay = 6;
		FMC_SDRAMTimingInitStructure.FMC_RPDelay = 2;
		FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime = 4;
		FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime = 2;
		
		FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank2_SDRAM;
		FMC_SDRAMInitStructure.FMC_CASLatency = FMC_CAS_Latency_3;
		FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_9b;
		FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
		FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_Enable;
		FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_0;
		FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_13b;
		/*设置SDCLK的时钟频率*/
		FMC_SDRAMInitStructure.FMC_SDClockPeriod = FMC_SDClock_Period_2;
		FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
		FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;
		FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
		
		
		FMC_SDRAMInit(&FMC_SDRAMInitStructure);
		SDRAM_Sequence();
		
}


void SDRAM_Sequence(void)
{
		 __IO uint32_t tmpmrd = 0;
		FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
		//使能时钟
		FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
		FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
		FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
		FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	
		/*Wait Until the SDRAM controller is ready*/
		while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy)!=RESET);
		 /* Send the command */  
		FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure); 
		/* Insert 100 us minimum delay */ 
		/* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
		delay_ms(1);
		/* Configure a PALL (precharge all) command */  
		FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
		FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
		FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
		FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
		while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy)!=RESET);
		FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure); 
		
		/* Configure a Auto-Refresh command */ 
		FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 8;
		FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
		FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
		FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
		
		 /* Send the  first command */
		FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
		while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy)!=RESET);
		/* Send the second command */  
		FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure); 
		
		/* Program the external memory mode register */
		tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1       |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
		FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
		FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
		FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
		FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpmrd;
		while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy)!=RESET);
		FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure); 
		
		 /* Set the refresh rate counter */  
		/* (7.81 us x Freq) - 20 */
		/* Set the device refresh counter */  
		FMC_SetRefreshCount(1292);
		while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET);
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
		 uint32_t tmpIndex = 0;
  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

u8 SDRAM_Test(void)
{
	uwWriteReadStatus = 0;
	uwIndex = 0;
	
		/* Fill the buffer to write */
		Fill_Buffer(aTxBuffer, BUFFER_SIZE, 0);   
		/* Write data to the SDRAM memory */
  for (uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++)
  {
    *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex) = aTxBuffer[uwIndex];
  }    
    /* Read back data from the SDRAM memory */
  for (uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++)
  {
    aRxBuffer[uwIndex] = *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex);
  } 
	/*##-3- Checking data integrity ############################################*/    
  
  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
  {
    if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex])
    {
      uwWriteReadStatus++;
    }
  }	
	if(uwWriteReadStatus) return 0;
	else return 1;
}
