#include "sram.h"
/*****************SRAM********************************
��ַ�ߣ�
FSMC_A0~A5 --PF0~PF5
FSMC_A6~A9 --PF12~PF15
FSMC_A10~A15 -- PG0~PG5
FSMC_A16~A18 --PD11~PD13
�����ߣ�
FSMC_D0~D1 --PD14~PD15
FSMC_D2~D3 -- PD0~PD1
FSMC_D4~D12 --PE7~PE15
FSMC_D13~D14 --PD8~PD9
FSMC_NBL0 -- PE0  ���ݿ��ѡ��
FSMC_NBL1 --PE1
FSMC_NOE -- PD4  ��ʹ��
FSMC_NWE --PD5  дʹ��
FSMC_NE3 --PG10  Ƭѡ(��һ����ĵ�3������)
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
  GPIOD->CRL|=0x00BB0000;//NOE��NWE
  GPIOG->CRH&=0xfffff0ff;
  GPIOG->CRH|=0x00000b00;//NE3
  
  /*fsmc����*/
  RCC->AHBENR|=1<<8;//FSMC
  FSMC_Bank1->BTCR[4]=0;
  FSMC_Bank1->BTCR[5]=0;
  FSMC_Bank1->BTCR[4]|=1<<12;//дʹ��
  FSMC_Bank1->BTCR[4]|=1<<4;//16λ���ݿ��
  FSMC_Bank1->BTCR[5]|=3<<8;//���ݱ���ʱ�䣬4ʱ�����ڣ�4*(1/720)=52ns
  FSMC_Bank1->BTCR[5]&=~(0Xf<<4);//��ַ����ʱ��
  FSMC_Bank1->BTCR[5]&=~(0xf<<0);//��ַ����ʱ��
  FSMC_Bank1->BTCR[4]|=1<<0;//FSMCʹ��
}
/********************SRAMд����******************
**
**�β�:u32 addr -- д������λ��
**     u8 *buff --д�������
**     u32 len  -- д����ֽ���
**SRAM�洢�ռ�:1MB
*************************************************/
void Sram_WriteData(u32 addr,u8 *buff,u32 len)
{
  u32 i=0;
  for(i=0;i<len;i++)
  {
    *(SRAM_ADDR+addr+i)=buff[i];
  }
}
/*********************SRAM������*********************
**
**
****�β�:u32 addr -- ��ȡ����λ��
**     u8 *buff --�����ݻ�����
**     u32 len  -- ��ȡ���ֽ���
**SRAM�洢�ռ�:1MB
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
