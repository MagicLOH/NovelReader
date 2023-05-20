#ifndef _SRAM_H
#define _SRAM_H
#include "stm32f10x.h"
#include "sys.h"
#define SRAM_ADDR (volatile u32 *)0x68000000
void Sram_Init(void);
void Sram_WriteData(u32 addr,u8 *buff,u32 len);
void Sram_ReadData(u32 addr,u8 *buff,u32 len);
#endif
