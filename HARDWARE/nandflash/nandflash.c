#include "nandflash.h"

uint32_t Index ;
uint8_t TxBuffer[NAND_PAGE_SIZE];
uint8_t RxBuffer[NAND_PAGE_SIZE];
__IO uint32_t WriteReadStatus ;
/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+--------------------+--------------------+
 +                       NANDFlash pins assignment                                  +
 +-------------------+--------------------+--------------------+--------------------+
 |I/O0 <-> PD14_FMC_D0 | WP <-> 3.3V           |
 |I/O1 <-> PD15_FMC_D1 | #WE <-> PD5_FMC_NWE   |
 |I/O2 <-> PD0_FMC_D2  | ALE <-> PD12_FMC_A17  |
 |I/O3 <-> PD1_FMC_D3  | CLE <-> PD11_FMC_A16  |
 |I/O4 <-> PE7_FMC_D4  | #CE <-> PG9_FMC_NCE3  |
 |I/O5 <-> PE8-FMC_D5  | #RE <-> PD4_FMC_NOE   |
 |I/O6 <-> PE9_FMC_D6  | RB  <-> PD6_FMC_NWAIT |
 |I/O7 <-> PE10_FMC_D7 |                       |
 +---------------------+-----------------------+
 */
 


/*-----------------------------------------------------------------------------
 *      NAND driver prototypes
 *----------------------------------------------------------------------------*/
static u32 Init         (NAND_DRV_CFG *cfg);
static u32 UnInit       (NAND_DRV_CFG *cfg);
static u32 PageRead     (u32 row, u8 *buf, NAND_DRV_CFG *cfg);
static u32 PageWrite    (u32 row, u8 *buf, NAND_DRV_CFG *cfg);
static u32 BlockErase   (u32 row, NAND_DRV_CFG *cfg);
static u32 RandomWrite  (u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len);
static u32 RandomRead   (u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len);

/*-----------------------------------------------------------------------------
  NAND Device Driver Control Block
 *----------------------------------------------------------------------------*/
const NAND_DRV nand0_drv = {
  Init,
  UnInit,
  PageRead,
  PageWrite,
	RandomRead,
	RandomWrite,
  BlockErase,

};

NAND_PG_LAY nand_pg_lay={
0,0,0,0,0,0,0,};

const NAND_DRV_CFG cfg = {
		&nand_pg_lay,
		NAND_DEVICE_SIZE,
		NAND_BLOCK_SIZE,
		NAND_PAGE_SIZE,
		0,
		0,
		5,
		0,
		0,
};

/*-----------------------------------------------------------------------------
 *      Initialise Flexible Static Memory Controler (FSMC)
 * Configure: - Data Lines    [D0 ÷ D16]
 *            - Address Lines [A0 ÷ A17]
 *            - Signal Lines NWE/NOE/CLK/NL/NWAIT
 *
 * cs = FSMC Chip Select (NOR/PSRAM, NAND, PC Card chip select)
 *
 *  Return: RTV_NOERR - Init successful
 *----------------------------------------------------------------------------*/
u32 FSMC_Init (u32 cs) {

  /* Enable system configuration controller clock */
  RCC->APB2ENR |= 1 << 14;

  /* Enable FSMC clock */
  RCC->AHB3ENR |= 1;

  /* Enable clocks and configure GPIO */
  RCC->AHB1ENR |= (1 << 6) |        /* GPIO Port G clock enable */   
                  (1 << 5) |        /* GPIO Port F clock enable */ 
                  (1 << 4) |        /* GPIO Port E clock enable */ 
                  (1 << 3) |        /* GPIO Port D clock enable */
                  (1 << 1) |        /* GPIO Port B clock enable */
                  (1 << 0) ;        /* GPIO Port A clock enable */

  /* PD.0, 1, 3, 4, 5, 8, 9, 10, 11, 12, 14, 15 */
  GPIOD->MODER   &= ~0xF3FF0FCF;
  GPIOD->MODER   |=  0xA2AA0A8A;    /* Pins to alternate function    */  
  GPIOD->OTYPER  &= ~0x0000DF3B;    /* Configure as push-pull pins   */
  GPIOD->PUPDR   &= ~0xF3FF0FCF;
  GPIOD->PUPDR   |=  0x51550545;    /* Pull-ups on pins              */
  GPIOD->OSPEEDR |=  0xF3FF0FCF;    /* Pins output speed to 100MHz   */
  GPIOD->AFR[0]  &= ~0x00FFF0FF;
  GPIOD->AFR[0]  |=  0x00CCC0CC;
  GPIOD->AFR[1]  &= ~0xFF0FFFFF;
  GPIOD->AFR[1]  |=  0xCC0CCCCC;    /* Pins assigned to AF12 (FSMC)  */


  /* PE.7, 8, 9, 10, 11, 12, 13, 14, 15 */
  GPIOE->MODER   &= ~0xFFFFC000;
  GPIOE->MODER   |=  0xAAAA8000;    /* Pins to alternate function    */  
  GPIOE->OTYPER  &= ~0x0000FF80;    /* Configure as push-pull pins   */
  GPIOE->PUPDR   &= ~0xFFFFC000;
  GPIOE->PUPDR   |=  0x55554000;    /* Pull-ups on pins              */
  GPIOE->OSPEEDR |=  0xFFFFC000;    /* Pins output speed to 100MHz   */
  GPIOE->AFR[0]  &= ~0xF0000000;
  GPIOE->AFR[0]  |=  0xC0000000;
  GPIOE->AFR[1]   =  0xCCCCCCCC;    /* Pins assigned to AF12 (FSMC)  */


//  /* PF.0, 1, 2, 3, 4, 5, 12, 13, 14, 15 */
//  GPIOF->MODER   &= ~0xFF000FFF;
//  GPIOF->MODER   |=  0xAA000AAA;    /* Pins to alternate function    */  
//  GPIOF->OTYPER  &= ~0x0000F03F;    /* Configure as push-pull pins   */
//  GPIOF->PUPDR   &= ~0xFF000FFF;
//  GPIOF->PUPDR   |=  0x55000555;    /* Pull-ups on pins              */
//  GPIOF->OSPEEDR |=  0xFF000FFF;    /* Pins output speed to 100MHz   */
//  GPIOF->AFR[0]  &= ~0x00FFFFFF;
//  GPIOF->AFR[0]  |=  0x00CCCCCC;
//  GPIOF->AFR[1]  &= ~0xFFFF0000;
//  GPIOF->AFR[1]  |=  0xCCCC0000;    /* Pins assigned to AF12 (FSMC)  */


  /* PG.0, 1, 2, 3, 4, 5 */
  GPIOG->MODER   &= ~0x00000FFF;
  GPIOG->MODER   |=  0x00000AAA;    /* Pins to alternate function    */  
  GPIOG->OTYPER  &= ~0x0000003F;    /* Configure as push-pull pins   */
  GPIOG->PUPDR   &= ~0x00000FFF;
  GPIOG->PUPDR   |= ~0x00000555;    /* Pull-ups on pins              */
  GPIOG->OSPEEDR |=  0x00000FFF;    /* Pins output speed to 100MHz   */
  GPIOG->AFR[0]  &= ~0x00FFFFFF;
  GPIOG->AFR[0]  |=  0x00CCCCCC;    /* Pins assigned to AF12 (FSMC)  */

	/* PG9 = Chip Enable (NCE3) */
	GPIOG->MODER   &= ~GPIO_MODER_MODER9;
	GPIOG->MODER   |=  GPIO_MODER_MODER9_1; /* Pin to alternate function    */
	GPIOG->OTYPER  &= ~GPIO_OTYPER_OT_9;    /* Configure as push-pull pin   */
	GPIOG->OSPEEDR |=  GPIO_OSPEEDER_OSPEEDR9_0 |
										 GPIO_OSPEEDER_OSPEEDR9_1 ;  /* Pin speed is 100MHz   */
	GPIOG->AFR[1]  &= ~0x000000F0;
	GPIOG->AFR[1]  |=  0x000000C0;          /* Pin assigned to AF12 (FSMC)  */

	/* PD6 = Ready/Busy Input */
	GPIOD->MODER   &= ~GPIO_MODER_MODER6;
	GPIOD->MODER   |=  GPIO_MODER_MODER6_1; /* Pin to alternate function    */
	GPIOD->OTYPER  &= ~GPIO_OTYPER_OT_6;    /* Configure as push-pull pin   */
	GPIOD->OSPEEDR |=  GPIO_OSPEEDER_OSPEEDR6_0 |
										 GPIO_OSPEEDER_OSPEEDR6_1 ;  /* Pin speed is 100MHz   */
//	GPIOG->AFR[0]  &= ~0x0F000000;
//	GPIOG->AFR[0]  |=  0x0C000000;          /* Pin assigned to AF12 (FSMC)  */

	/* Configure ECC to 512 bytes */
	FMC_Bank3->PCR3 =  FMC_PCR3_ECCPS_1 | /* ECC page size 512 bytes        */
										 FMC_PCR3_TAR     | /* ALE to RE delay: x HCLK cycles */
										 FMC_PCR3_TCLR    | /* CLE to RE delay: x HCLK cycles */
										 FMC_PCR3_ECCEN   | /* ECC logic is enabled           */
										 FMC_PCR3_PTYP    | /* Memory type is NAND Flash      */
										 FMC_PCR3_PBKEN   ; /* Memory bank is enabled         */
	
	/* Configure common memory space timing */
	FMC_Bank3->PMEM3 =  NAND_CO_HIZ_TIME   |    /* Set HiZ time             */
											NAND_CO_HOLD_TIME  |    /* Set hold time            */
											NAND_CO_WAIT_TIME  |    /* Set wait time            */
											NAND_CO_SETUP_TIME ;    /* Set setup time           */

	/* Configure attribute memory space timing */
	FMC_Bank3->PATT3 =  NAND_AT_HIZ_TIME   |    /* Set HiZ time             */
											NAND_AT_HOLD_TIME  |    /* Set hold time            */
											NAND_AT_WAIT_TIME  |    /* Set wait time            */
											NAND_AT_SETUP_TIME ;    /* Set setup time           */
	return RTV_NOERR;
}



void NandFlash_PD14_Set(void)
{
		  /* PD.0, 1, 3, 4, 5, 8, 9, 10, 11, 12, 14, 15 */
  GPIOD->MODER   &= ~0xF3FF0FCF;
  GPIOD->MODER   |=  0xA2AA0A8A;    /* Pins to alternate function    */  
  GPIOD->OTYPER  &= ~0x0000DF3B;    /* Configure as push-pull pins   */
  GPIOD->PUPDR   &= ~0xF3FF0FCF;
  GPIOD->PUPDR   |=  0x51550545;    /* Pull-ups on pins              */
  GPIOD->OSPEEDR |=  0xF3FF0FCF;    /* Pins output speed to 100MHz   */
  GPIOD->AFR[0]  &= ~0x00FFF0FF;
  GPIOD->AFR[0]  |=  0x00CCC0CC;
  GPIOD->AFR[1]  &= ~0xFF0FFFFF;
  GPIOD->AFR[1]  |=  0xCC0CCCCC;    /* Pins assigned to AF12 (FSMC)  */
}
/*-----------------------------------------------------------------------------
        Write command
 *----------------------------------------------------------------------------*/
__inline static void NAND_WrCmd (u32 cmd) {
  NAND_CMD_W8 = (u8)cmd;
}
__inline static void NAND_RegCmd (u32 reg) {
  NAND_ADR_W8 = (u8)reg;
}


/*-----------------------------------------------------------------------------
 *      Wait until NAND is busy (Ready bit in status register is 0)
 *
 *  Return: __TRUE            - NAND Ready before timeout
 *          __FALSE           - NAND Busy, timeout expired
 *----------------------------------------------------------------------------*/
__inline static BOOL NAND_WaitReady (void) {
  u32 i;

  for (i = NAND_TIMEOUT; i; i--) {
    /* Read Status Register */
    NAND_CMD_W8 = NAND_CMD_STATUS;
    if (NAND_DATA_W8 & NAND_STAT_RDY) {
      return (__TRUE);
    }
  }
  return (__FALSE);
}

/*-----------------------------------------------------------------------------
 *      NAND initialization
 * Default page layout is used.
 *
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR        - Init successful
 *          ERR_NAND_HW_TOUT - Reset failed / NAND Unlock failed 
 *----------------------------------------------------------------------------*/
static u32 FSMC_NAND_Init (NAND_DRV_CFG *cfg) {  
  
  /* Reset NAND chip */
  NAND_WrCmd (NAND_CMD_RESET);

  /* Wait until ready */
  if (NAND_WaitReady() == __FALSE) {
    /* Reset failed */
    return ERR_NAND_HW_TOUT;
  }
  
  /* NAND is ready */
  return RTV_NOERR;
}

void NAND_Read_ID()
{
		u8* buf;
		NAND_WrCmd (NAND_CMD_READID);
		NAND_RegCmd(NAND_CMD_IDADDR);
		buf[0] = NAND_DATA_W8;
		buf[1] = NAND_DATA_W8;
		buf[2] = NAND_DATA_W8;
		buf[3] = NAND_DATA_W8;
		buf[4] = NAND_DATA_W8;

}
/*-----------------------------------------------------------------------------
 *      Initialise NAND flash driver
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR         - NAND Flash Initialisation successful
 *          ERR_NAND_HW_TOUT  - NAND Flash Reset Command failed
 *          ERR_INVALID_PARAM - Invalid parameter
 *----------------------------------------------------------------------------*/
static u32 Init (NAND_DRV_CFG *cfg) {
	
      FSMC_Init (FSMC_NCE3);                            /* Init FSMC Bank 3   */
      return FSMC_NAND_Init (cfg);                      /* Init NAND Flash    */   
}
/*-----------------------------------------------------------------------------
 *      Uninitialise NAND flash driver
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR         - UnInit successful
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *      Flexible Static Memory Controler (FSMC) uninitialization
 *
 * cs = FSMC Chip Select (NOR/PSRAM, NAND, PC Card chip select)
 *
 *  Return: RTV_NOERR - UnInit successful
 *----------------------------------------------------------------------------*/
u32 FSMC_UnInit (u32 cs) {

	/* Uninit PG9 and PD6 */
	GPIOG->MODER   &= ~GPIO_MODER_MODER9;
	GPIOG->OTYPER  &= ~GPIO_OTYPER_OT_9;
	GPIOG->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR6;
	GPIOG->PUPDR   &= ~GPIO_PUPDR_PUPDR9;
	GPIOG->AFR[1]  &= ~0x000000F0;

	GPIOD->MODER   &= ~GPIO_MODER_MODER6;
	GPIOD->OTYPER  &= ~GPIO_OTYPER_OT_6;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR6;
	GPIOD->PUPDR   &= ~GPIO_PUPDR_PUPDR6;
	GPIOD->AFR[0]  &= ~0x0F000000;

	/* FSMC Registers to reset values */
	FMC_Bank3->PCR3  = 0x00000018;
	FMC_Bank3->PMEM3 = 0xFCFCFCFC;
	FMC_Bank3->PATT3 = 0xFCFCFCFC;

  return RTV_NOERR;
}


static u32 UnInit (NAND_DRV_CFG *cfg) {

	return FSMC_UnInit (FSMC_NCE3);
}
/*-----------------------------------------------------------------------------
        Set page address
 *----------------------------------------------------------------------------*/
static void NAND_SetPgAddr (u32 numCyc, u32 row, u32 pgSz) {  
  u32 i;

  NAND_ADR_W8 = 0;
  numCyc--;
  if (pgSz > 528) {    
    NAND_ADR_W8 = 0;
    numCyc--;
  }

  i = 0;
		row = (row >>11);

  while (numCyc--) {
    NAND_ADR_W8 = (u8)(row >> i);
	
    i += 8;
  }
//		NAND_ADR_W8 = 0;
//		i+=8;
//		NAND_ADR_W8 = 0;
//		i+=3;
//		NAND_ADR_W8 = (u8) (row>>i);
//		i+=8;
//		NAND_ADR_W8 = (u8) (row>>i);
//		i+=8;
//		NAND_ADR_W8 = (u8) (row>>i);
		
}
/*-----------------------------------------------------------------------------
        Set block address
 *----------------------------------------------------------------------------*/
__inline void NAND_SetBlAddr (u32 numCyc, u32 row, u32 pgSz) {
  u32 i;

  numCyc = (pgSz > 528) ? (numCyc - 2) : (numCyc - 1);

  i = 0;
	row = (row <<5) >> 16;
  while (numCyc--) {
    NAND_ADR_W8 = (u8)(row >> i);
    i += 8;
  }
//		NAND_ADR_W8 = (u8)0;

//		NAND_ADR_W8 = (u8) 0;
//		i+=11;
//		NAND_ADR_W8 = (u8) (row>>i);
//		i+=8;
//		NAND_ADR_W8 = (u8) (row>>i);
//		i+=8;
//		NAND_ADR_W8 = (u8) (row>>i);
			
}

/*-----------------------------------------------------------------------------
 *      Read data from NAND
 *  row  = Page address
 *  *buf = Pointer to data buffer
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR         - Page read successful
 *          ERR_NAND_HW_TOUT  - Hardware transfer timeout
 *          ERR_ECC_COR       - ECC corrected the data within page
 *          ERR_ECC_UNCOR     - ECC was not able to correct the data
 *----------------------------------------------------------------------------*/
static u32 FSMC_NAND_Read (u32 row, u8 *buf, NAND_DRV_CFG *cfg) {
  u32 i, sz;

  /* Write command 1 */
  NAND_WrCmd (NAND_CMD_READ1ST);

  /* Set address */
  NAND_SetPgAddr (cfg->AddrCycles, row, cfg->PageSize);

  /* Write command 2 */
  NAND_WrCmd (NAND_CMD_READ2ND);

  /* Wait until NAND ready */
  if (NAND_WaitReady() == __FALSE) {
    return ERR_NAND_HW_TOUT;
  }

  /* Switch back to read: Write command 1 */
  NAND_WrCmd (NAND_CMD_READ1ST);

  /* Read page from NAND chip */
  sz = cfg->PageSize;
  for (i = 0; i < sz; i += 8) {
    buf[i]   = NAND_DATA_W8;
    buf[i+1] = NAND_DATA_W8;
    buf[i+2] = NAND_DATA_W8;
    buf[i+3] = NAND_DATA_W8;
    buf[i+4] = NAND_DATA_W8;
    buf[i+5] = NAND_DATA_W8;
    buf[i+6] = NAND_DATA_W8;
    buf[i+7] = NAND_DATA_W8;
  }

  return RTV_NOERR;
}


static u32 FSMC_Random_Read (u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len)
{
		 u32 i, sz;

  /* Write command 1 */
  NAND_WrCmd (NAND_CMD_READ1ST);

  /* Set address */
  NAND_SetPgAddr (cfg->AddrCycles, row, cfg->PageSize);

  /* Write command 2 */
  NAND_WrCmd (NAND_CMD_READ2ND);

  /* Wait until NAND ready */
  if (NAND_WaitReady() == __FALSE) {
    return ERR_NAND_HW_TOUT;
  }

  /* Switch back to read: Write command 1 */
  NAND_WrCmd (NAND_CMD_RANDOM_READ1);
		NAND_ADR_W8 = (u8)(row&0xFF);
		NAND_ADR_W8 = (u8) (row>>8&0x07);
	NAND_WrCmd (NAND_CMD_RANDOM_READ2);
	/* Wait until NAND ready */
  if (NAND_WaitReady() == __FALSE) {
    return ERR_NAND_HW_TOUT;
  }
	NAND_WrCmd (NAND_CMD_READ1ST);
  /* Read page from NAND chip */
 // sz = cfg->PageSize - (u32)row&0xFFF;
  for (i = 0; i < len; i ++) {
    buf[i]   = NAND_DATA_W8;
  }

  return RTV_NOERR;
}
/*-----------------------------------------------------------------------------
        Check status register if specified flag is set

  flag = NAND status flag

  Return: NAND_FLAG_SET  - NAND flag is set
          NAND_FLAG_CLR  - NAND flag is cleared
 *----------------------------------------------------------------------------*/
__inline static u32 NAND_ChkStatus (u32 flag) {

  NAND_CMD_W8 = NAND_CMD_STATUS;  
  return (NAND_DATA_W8 & flag) ? NAND_FLAG_SET : NAND_FLAG_CLR; 
}

/*-----------------------------------------------------------------------------
 *      Write data to NAND
 *  row  = Page address
 *  *buf = Pointer to data to write
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR         - Buffer written successfuly
 *          ERR_NAND_PROG     - Programing failed
 *          ERR_NAND_HW_TOUT  - Hardware request timeout
 *----------------------------------------------------------------------------*/
static u32 FSMC_NAND_Write (u32 row, u8 *buf, NAND_DRV_CFG *cfg) {
  u32 i, sz;

  /* Write command 1 */
  NAND_WrCmd (NAND_CMD_PROG1ST);

  /* Set address */
  NAND_SetPgAddr (cfg->AddrCycles, row, cfg->PageSize);
  
  /* Write data to NAND chip */
  sz = cfg->PageSize;
  for (i = 0; i < sz; i += 8) {
    NAND_DATA_W8 = buf[i];
    NAND_DATA_W8 = buf[i+1];
    NAND_DATA_W8 = buf[i+2];
    NAND_DATA_W8 = buf[i+3];
    NAND_DATA_W8 = buf[i+4];
    NAND_DATA_W8 = buf[i+5];
    NAND_DATA_W8 = buf[i+6];
    NAND_DATA_W8 = buf[i+7];
  }

  /* Write command 2 */
  NAND_WrCmd (NAND_CMD_PROG2ND);

  /* Wait until NAND ready */
  if (NAND_WaitReady() == __FALSE) {
    return ERR_NAND_HW_TOUT;
  }

  /* Check status */
  switch (NAND_ChkStatus (NAND_STAT_FAIL)) {
    case NAND_FLAG_TOUT:
      return ERR_NAND_HW_TOUT;
    case NAND_FLAG_SET:
      return ERR_NAND_PROG;
    default:
      return RTV_NOERR;
  }
}


u32 FSMC_Random_Write(u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len)
{
		u32 i, sz;
	
		/* Write command 1 */
		NAND_WrCmd (NAND_CMD_PROG1ST);
		NAND_SetPgAddr (cfg->AddrCycles, row, cfg->PageSize);
		NAND_WrCmd (NAND_CMD_RANDOM_WRITE);
		NAND_ADR_W8 = (u8)(row&0xFF);
		NAND_ADR_W8 = (u8) (row>>8&0x07);
	
		/* Write data to NAND chip */
  //sz = cfg->PageSize - used_address ;
  for (i = 0; i < len; i ++) {
    NAND_DATA_W8 = buf[i];
  }

	/* Write command 2 */
  NAND_WrCmd (NAND_CMD_PROG2ND);

  /* Wait until NAND ready */
  if (NAND_WaitReady() == __FALSE) {
    return ERR_NAND_HW_TOUT;
  }	
	
	 /* Check status */
  switch (NAND_ChkStatus (NAND_STAT_FAIL)) {
    case NAND_FLAG_TOUT:
      return ERR_NAND_HW_TOUT;
    case NAND_FLAG_SET:
      return ERR_NAND_PROG;
    default:
      return RTV_NOERR;
  }
		
}
/*-----------------------------------------------------------------------------
 *      Erase block of memory on NAND
 *  row  = Block address
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR        - Erase operation successful
 *          ERR_NAND_ERASE   - Erase operation failed
 *          ERR_NAND_HW_TOUT - Hardware request timeout
 *----------------------------------------------------------------------------*/
static u32 FSMC_NAND_Erase (u32 row, NAND_DRV_CFG *cfg) {
  /* Write command 1 */
  NAND_WrCmd (NAND_CMD_ERASE1ST);
  
  /* Set address */
  NAND_SetBlAddr (cfg->AddrCycles, row, cfg->PageSize);
  
  /* Write command 2 */
  NAND_WrCmd (NAND_CMD_ERASE2ND);

  /* Wait until NAND ready */
  if (NAND_WaitReady() == __FALSE) {
    return ERR_NAND_HW_TOUT;
  }
  
  /* Check status */
  switch (NAND_ChkStatus (NAND_STAT_FAIL)) {
    case NAND_FLAG_TOUT:
      return ERR_NAND_HW_TOUT;
    case NAND_FLAG_SET:
      return ERR_NAND_ERASE;
    default:
      return RTV_NOERR;
  }
}

/*-----------------------------------------------------------------------------
 *      Read page
 *  row  = Page address
 *  *buf = Pointer to data buffer
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR         - Page read successful
 *          ERR_NAND_HW_TOUT  - Hardware transfer timeout
 *          ERR_ECC_COR       - ECC corrected the data within page
 *          ERR_ECC_UNCOR     - ECC was not able to correct the data
 *----------------------------------------------------------------------------*/
static u32 PageRead (u32 row, u8 *buf, NAND_DRV_CFG *cfg) {

	return FSMC_NAND_Read (row, buf, cfg);
}

/*-----------------------------------------------------------------------------
 *      Write page
 *  row  = Page address
 *  *buf = Pointer to data buffer
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR         - Page write successful
 *          ERR_NAND_PROG     - Page write failed
 *          ERR_NAND_HW_TOUT  - Hardware transfer timeout
 *----------------------------------------------------------------------------*/
static u32 PageWrite (u32 row, u8 *buf, NAND_DRV_CFG *cfg) {

	return FSMC_NAND_Write (row, buf, cfg);
}

/*-----------------------------------------------------------------------------
 *      Erase block
 *  row  = Block address
 *  *cfg = Pointer to configuration structure
 *
 *  Return: RTV_NOERR         - Block erase successful
 *          ERR_NAND_ERASE    - Block erase failed
 *          ERR_NAND_HW_TOUT  - Hardware transfer timeout
 *----------------------------------------------------------------------------*/
static u32 BlockErase (u32 row, NAND_DRV_CFG *cfg) {

	return FSMC_NAND_Erase (row, cfg);
}

static u32 RandomWrite(u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len)
{
		return FSMC_Random_Write(row, buf, cfg,len);
}

static u32 RandomRead(u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len)
{
		return FSMC_Random_Read(row, buf, cfg,len);
}

 void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
		 uint32_t tmpIndex = 0;
  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
		
  }
}


u8 NAND_test()
{
	
			/* Fill the buffer to write */
		Fill_Buffer(TxBuffer, NAND_PAGE_SIZE, 0);   
		nand0_drv.BlockErase((100*128*1024),&cfg);
		nand0_drv.PageWrite((100*128*1024),TxBuffer,&cfg);
		nand0_drv.PageRead((100*128*1024),RxBuffer,&cfg);
		for (Index = 0; (Index < NAND_PAGE_SIZE) && (WriteReadStatus == 0); Index++)
		{
			if (RxBuffer[Index] != TxBuffer[Index])
			{
				WriteReadStatus++;
			}
		}	
		if(WriteReadStatus) return 0;
		else return 1;
}

u8 NAND_Random_test(void)
{
		Fill_Buffer(TxBuffer,10,2);
		nand0_drv.BlockErase(10,&cfg);
		nand0_drv.RandomWrite(10,TxBuffer,&cfg,10);
		nand0_drv.RandomRead(10,RxBuffer,&cfg,10);
	//	nand0_drv.PageRead(10,RxBuffer,&cfg);
		for (Index = 0; (Index < 10) && (WriteReadStatus == 0); Index++)
		{
			if (RxBuffer[Index] != TxBuffer[Index])
			{
				WriteReadStatus++;
			}
		}	
		if(WriteReadStatus) return 0;
		else return 1;
		
}

