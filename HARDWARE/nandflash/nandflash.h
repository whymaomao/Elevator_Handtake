#ifndef __NANDFLASH_H
#define __NANDFLASH_H

#include "sys.h"


typedef unsigned int   BOOL;

#ifndef __TRUE
 #define __TRUE         1
#endif
#ifndef __FALSE
 #define __FALSE        0
#endif

/* Timing configured for HCLK @ 120MHz */
#define NAND_CO_HIZ_TIME      (0x02 << 24) 
#define NAND_CO_HOLD_TIME     (0x02 << 16)
#define NAND_CO_WAIT_TIME     (0x02 <<  8)
#define NAND_CO_SETUP_TIME    (0x01 <<  0)

#define NAND_AT_HIZ_TIME      (0x02 << 24)
#define NAND_AT_HOLD_TIME     (0x02 << 16)
#define NAND_AT_WAIT_TIME     (0x02 <<  8)
#define NAND_AT_SETUP_TIME    (0x01 <<  0)


#define NAND_DATA_W8  FSMC_AHB_W8(FSMC_BANK3_BASE, 0)
#define NAND_ADR_W8   FSMC_AHB_W8(FSMC_BANK3_BASE, FSMC_BANK3_ADR)
#define NAND_CMD_W8   FSMC_AHB_W8(FSMC_BANK3_BASE, FSMC_BANK3_CMD)


#define NandFlash_BASE 0x70000000
#define NandFlash_CMD  0x010000
#define NandFlash_ADR  0x020000
#define NandFlash_DAT  0x000000
#define NAND_TIMEOUT 2000000
/* Bank Base Address */
#define FSMC_BANK1_BASE 0x60000000UL
#define FSMC_BANK2_BASE 0x70000000UL
#define FSMC_BANK3_BASE 0x80000000UL
#define FSMC_BANK4_BASE 0x90000000UL

#define FSMC_BANK3_CMD  0x010000UL
#define FSMC_BANK3_ADR  0x020000UL

/* AHB Access Macro */
#define FSMC_AHB_W8(base,  addr)  (*(volatile u8  *)((base) + (addr)))
#define FSMC_AHB_W16(base, addr)  (*(volatile u16 *)((base) + (addr)))
	
/* NAND Page Layout Configuration Structure */
typedef struct {
  u8  Pos_LSN;                          /* LSN position                      */
  u8  Pos_COR;                          /* Data in page corrupted marker     */
  u8  Pos_BBM;                          /* Bad Block marker position         */
  u8  Pos_ECC;                          /* First byte of ECC                 */
  u16 SectInc;                          /* Column increment till next sector */
  u16 SpareOfs;                         /* Spare area offset from begining   */
                                        /*  of the page                      */
  u16 SpareInc;                         /* Column increment till next spare  */  
} NAND_PG_LAY;


/* NAND Flash Geometry and Layout */
typedef struct _NAND_DRV_CFG {
  NAND_PG_LAY *PgLay;                   /* Page Layout Definitions           */
  u16 NumBlocks;                        /* Number of blocks per device       */
  u16 NumPages;                         /* Number of pages per block         */
  u16 PageSize;                         /* Page size                         */
  u16 SectorsPerBlock;                  /* Number of sectors per block       */
	u8  SectorsPerPage;                   /* Number of sectors per page        */
  u8  AddrCycles;                       /* Device address cycles             */
  u8  SwEccEn;                          /* Software ECC enabled              */
  u8  DrvInst;                          /* Driver Instance definition        */
} const NAND_DRV_CFG;


/* NAND Flash Device driver */
typedef struct {
  u32  (*Init)        (NAND_DRV_CFG *cfg);
  u32  (*UnInit)      (NAND_DRV_CFG *cfg);
  u32  (*PageRead)    (u32 row, u8 *buf, NAND_DRV_CFG *cfg);
  u32  (*PageWrite)   (u32 row, u8 *buf, NAND_DRV_CFG *cfg);
	u32  (*RandomRead)  (u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len);
	u32  (*RandomWrite)	(u32 row, u8 *buf, NAND_DRV_CFG *cfg,u8 len);
  u32  (*BlockErase)  (u32 row, NAND_DRV_CFG *cfg);
} const NAND_DRV;


/* NFTL Error Codes */
typedef enum {
  RTV_NOERR = 0,                        /* No error                          */
  ERR_INVALID_PARAM,                    /* Invalid parameter                 */

  ERR_FTL_ECC,                          /* ECC error on last page read       */
  ERR_FTL_WARN,                         /* Warning, data not reliable        */
  ERR_FTL_BB,                           /* Bad block                         */
  ERR_FTL_LBN,                          /* Wrong LBN                         */
  ERR_FTL_ALLOC,                        /* Block allocation error            */
  ERR_FTL_RANGE,                        /* BN to high                        */
  ERR_FTL_UNFOR,                        /* NAND is unformatted               */
  ERR_FTL_NOT_FOUND,                    /* Item not found                    */
  ERR_FTL_NOT_FOUND_EOB,                /* LSN not found, end of block       */
  ERR_FTL_FATAL,                        /* Fatal, undefined error            */

  ERR_ECC_COR,                          /* ECC corrected the data            */
  ERR_ECC_UNCOR,                        /* ECC could not correct the data    */

  ERR_NAND_PROG,                        /* Programming failed (flash status) */
  ERR_NAND_ERASE,                       /* Erase verify failed (flash status)*/
  ERR_NAND_HW_TOUT,                     /* NAND hardware timeout             */
  ERR_NAND_DMA_TOUT,                    /* DMA timeout                       */
  ERR_NAND_UNSUPPORTED,                 /* Functionality not supported       */
} NAND_FTL_ERROR;

/* NOR/PSRAM Chip Select */
#define FSMC_NE1      0                    /* Bank 1, Chip Select 1 (NE1)   */
#define FSMC_NE2      1                    /* Bank 1, Chip Select 2 (NE2)   */
#define FSMC_NE3      2                    /* Bank 1, Chip Select 3 (NE3)   */
#define FSMC_NE4      3                    /* Bank 1, Chip Select 4 (NE4)   */

/* NAND Chip Select */
#define FSMC_NCE2     4                    /* Bank 2 Chip Select (NCE2)     */
#define FSMC_NCE3     5                    /* Bank 3 Chip Select (NCE3)     */

/* PC Card Chip Select */
#define FSMC_NCE4_1   6                    /* Bank 4, Chip Select 1 (NCE4_1)*/
#define FSMC_NCE4_2   7                    /* Bank 4, Chip Select 2 (NCE4_2)*/

/* Flag Set/Cleared */
#define NAND_FLAG_CLR       0x00
#define NAND_FLAG_SET       0x01
#define NAND_FLAG_TOUT      0x02

/* Invalid Chip Select */
#define FSMC_CS_INV   8
//////////NAND Status/////////////////
/*
bit0:Fail  0<->Error in program/erase/read
					 1<->Successful in program/erase/read
bit1:Failc
bit2:Don't care
bit3:Rewrite recommended 0<->Normal or uncorrectable
												 1<->Rewrite recommended
bit4:Don't care
bit5:ARDY
bit6:RDY 0<->busy 1<->Ready
bit7:Write protect 0<->protected 1<->not protected
*/
#define NAND_STAT_FAIL        0x01      /* Last command failed               */
#define NAND_STAT_FAILC       0x02      /* Command prior last failed         */
#define NAND_STAT_CSP         0x08      /* Command specific                  */
#define NAND_STAT_VSP         0x10      /* Vendor specific                   */
#define NAND_STAT_ARDY        0x20      /* Array operation in progress       */
#define NAND_STAT_RDY         0x40      /* LUN ready for another command     */
#define NAND_STAT_WP          0x80      /* Write protected                   */

typedef struct
{
		uint8_t Maker_ID;
		uint8_t Device_ID;
		uint8_t Third_ID;
		uint8_t Fourth_ID;
		uint8_t Fifth_ID;
}NAND_IDTypedef;


#define NAND_PAGE_SIZE 0x0800
#define NAND_BLOCK_SIZE 0x0040
#define NAND_SPARE_AREA_SIZE 0x0040
#define NAND_DEVICE_SIZE 0x1000

#define NAND_CMD_READ1   ((uint8_t)0x00)
#define NAND_CMD_READID  ((uint8_t)0x90)
#define NAND_CMD_IDADDR  ((uint8_t)0x00)
#define NAND_CMD_RESET   ((uint8_t)0xFF)
#define NAND_CMD_STATUS  ((uint8_t)0x70)
#define NAND_CMD_READ1ST      0x00      /* Read 1st Cycle                    */
#define NAND_CMD_READ2ND      0x30      /* Read 2nd Cycle                    */
#define NAND_CMD_PROG1ST      0x80      /* Page Program 1st Cycle            */
#define NAND_CMD_PROG2ND      0x10      /* Page Program 2nd Cycle            */
#define NAND_CMD_ERASE1ST     0x60      /* Block erase 1st Cycle             */
#define NAND_CMD_ERASE2ND     0xD0      /* Block erase 2nd cycle             */
#define NAND_CMD_RANDOM_WRITE  0x85  
#define NAND_CMD_RANDOM_READ1   0x05   /* Random Read 1st Cycle*/
#define NAND_CMD_RANDOM_READ2   0xE0	 /* Random Read 2nd Cycle*/


/*FMC NAND memory data computation*/
#define DATA_1st_cycle(DATA) (uint8_t)((DATA)&0xFF)
#define DATA_2nd_cycle(DATA) (uint8_t)(((DATA)&0xFF00)>>8)
#define DATA_3rd_cycle(DATA) (uint8_t)(((DATA)&0xFF0000)>>16)
#define DATA_4th_cycle(DATA) (uint8_t)(((DATA)&0xFF000000)>>24)
//#define DATA_5th_cycle(DATA) (uint8_t)(((DATA)&0xFF00000000)>>32)
u8 NAND_test(void);
void NAND_Read_ID(void);
void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
u8 NAND_Random_test(void);
void NandFlash_PD14_Set(void);
#endif /*__NANDFLASH*/


