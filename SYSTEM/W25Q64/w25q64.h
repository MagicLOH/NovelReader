#ifndef _W25Q64_H
#define _W25Q64_H
#include "stm32f10x.h"
#include "sys.h"
#include "spi.h"
#define F_CS PBout(12)
void W25Q64_Init(void);//��ʼ��
u16 W25Q64_ReadID(void);//��ȡID
void W25Q64_ReadData(u32 addr,u8 *buff,u32 count);//��ȡ����
void W25Q64_SectorErase(u32 addr);//��������
void W25Q64_PageWrite_Erase(u32 addr,u8 *buff,u32 count);
void W25Q64_WriteData(u32 addr,const u8 *buff,u32 count);
#endif
