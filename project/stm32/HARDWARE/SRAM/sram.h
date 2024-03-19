#ifndef __SRAM_H
#define __SRAM_H															    
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部SRAM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define CSn PGout(10)	// DS0
#define RDn PDout(4)
#define WRn PDout(5)

#define O1_CS PFout(0)
#define O2_CS PFout(1)
#define I1_CS PFout(2)
#define I2_CS PFout(3)
#define I3_CS PFout(4)

#define D0 PDout(14)
#define D1 PDout(15)
#define D2 PDout(0)
#define D3 PDout(1)
#define D4 PEout(7)
#define D5 PEout(8)
#define D6 PEout(9)
#define D7 PEout(10)
#define D8 PEout(11)
#define D9 PEout(12)
#define D10 PEout(13)
#define D11 PEout(14)
#define D12 PEout(15)
#define D13 PDout(8)
#define D14 PDout(9)
#define D15 PDout(10)

void EXTIO_Init(void);
void ExtIO_Out1(uint16_t data);
	
//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM4_ADDR    ((u32)(0x6C000000))//SRAM
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))//扩展IO
#define Bank1_SRAM2_ADDR    ((u32)(0x64000000))//TC6014B
#define Bank1_SRAM1_ADDR    ((u32)(0x60000000))//TC6014A

void FSMC_SRAM_Init(void);
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 NumHalfwordToWrite);
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 NumHalfwordToRead);


void fsmc_sram_test_write(u32 addr,u8 data);
u8 fsmc_sram_test_read(u32 addr);

extern u32 testsram[250000];
extern u16 testio_o1;
extern u16 testio_o2;
void fsmc_sram_test(void);

#endif

