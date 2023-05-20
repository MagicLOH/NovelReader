#ifndef _SPI_H
#define _SPI_H
#include "stm32f10x.h"
#include "sys.h"
#define SPI_MODE 1 //1Ӳ��ʱ��0Ϊģ��ʱ��
#define SPI2_SCK PBout(13)
#define SPI2_MISO PBin(14)
#define SPI2_MOSI PBout(15)
void SPI2_Init(void);
u8 SPI2_WR_OneByte(u8 dat_w);
#endif

