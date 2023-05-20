#ifndef _ADC_H
#define _ADC_H
#include "stm32f10x.h"
void ADC_Regular_Init(void);//规则通道配置
u16 ADC1_Regular_GetCHx(u8 chx);//获取一次数据
#endif
