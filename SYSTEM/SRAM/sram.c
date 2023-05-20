#include "sram.h"
/*****************SRAM********************************
地址线：
FSMC_A0~A5 --PF0~PF5
FSMC_A6~A9 --PF12~PF15
FSMC_A10~A15 -- PG0~PG5
FSMC_A16~A18 --PD11~PD13
数据线：
FSMC_D0~D1 --PD14~PD15
FSMC_D2~D3 -- PD0~PD1
FSMC_D4~D12 --PE7~PE15
FSMC_D13~D14 --PD8~PD9
FSMC_NBL0 -- PE0  数据宽度选择
FSMC_NBL1 --PE1
FSMC_NOE -- PD4  读使能
FSMC_NWE --PD5  写使能
FSMC_NE3 --PG10  片选(第一个块的第3个区域)
******************************************************/
void Sram_Init(void)
{
  RCC->APB2ENR|=1<<5;//PD
  RCC->APB2ENR|=1<<6;//PE
  RCC->APB2ENR|=1<<7;//PF
  RCC->APB2ENR|=1<<8;//PG
  GPIOF->CRL&=0xFF000000;
  GPIOF->CRL|=0x00BBBBBB;//A0~A5
  GPIOF->CRH&=0x0000FFFF;
  GPIOF->CRH|=0xBBBB0000;//A6~A9
  GPIOG->CRL&=0xFF000000;
  GPIOG->CRL|=0x00BBBBBB;//A10~A15
  GPIOD->CRH&=0xFF000FFF;
  GPIOD->CRH|=0x00BBB000;//A16~A18
  
  GPIOD->CRH&=0x00fff000;
  GPIOD->CRH|=0xBB000BBB;//D0~D1,D13~D15
  GPIOD->CRL&=0xFFFFFF00;
  GPIOD->CRL|=0x000000BB;//D2~D3
  GPIOE->CRL&=0x0FFFFFFF;
  GPIOE->CRL|=0xB0000000;//D4
  GPIOE->CRH&=0x00000000;
  GPIOE->CRH|=0xBBBBBBBB;//D5~D
  
  GPIOE->CRL&=0xFFFFFF00;//NBL0~L1
  GPIOE->CRL|=0x000000BB;
  GPIOD->CRL&=0xFF00FFFF;
  GPIOD->CRL|=0x00BB0000;//NOE、NWE
  GPIOG->CRH&=0xfffff0ff;
  GPIOG->CRH|=0x00000b00;//NE3
  
  /*fsmc配置*/
  RCC->AHBENR|=1<<8;//FSMC
  FSMC_Bank1->BTCR[4]=0;
  FSMC_Bank1->BTCR[5]=0;
  FSMC_Bank1->BTCR[4]|=1<<12;//写使能
  FSMC_Bank1->BTCR[4]|=1<<4;//16位数据宽度
  FSMC_Bank1->BTCR[5]|=3<<8;//数据保持时间，4时钟周期，4*(1/720)=52ns
  FSMC_Bank1->BTCR[5]&=~(0Xf<<4);//地址保持时间
  FSMC_Bank1->BTCR[5]&=~(0xf<<0);//地址建立时间
  FSMC_Bank1->BTCR[4]|=1<<0;//FSMC使能
}
/********************SRAM写数据******************
**
**形参:u32 addr -- 写入数据位置
**     u8 *buff --写入的数据
**     u32 len  -- 写入的字节数
**SRAM存储空间:1MB
*************************************************/
void Sram_WriteData(u32 addr,u8 *buff,u32 len)
{
  u32 i=0;
  for(i=0;i<len;i++)
  {
    *(SRAM_ADDR+addr+i)=buff[i];
  }
}
/*********************SRAM读数据*********************
**
**
****形参:u32 addr -- 读取数据位置
**     u8 *buff --读数据缓冲区
**     u32 len  -- 读取的字节数
**SRAM存储空间:1MB
**
******************************************************/
void Sram_ReadData(u32 addr,u8 *buff,u32 len)
{
  u32 i=0;
  for(i=0;i<len;i++)
  {
    buff[i]=*(SRAM_ADDR+addr+i);
  }
}
