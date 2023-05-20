#ifndef _AT24C08_H
#define _AT24C08_H
#include "stm32f10x.h"
#include "iic.h"
#define IIC_ADDR_R 0xA1 //从器件地址读
#define IIC_ADDR_W 0XA0 //从器件地址写
void AT24C08_WriteData(u16 addr,u8 *buff,u16 count);
u8 AT24C08_ReadData(u16 addr,u8 *buff,u16 count);//连续读
#endif

