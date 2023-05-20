#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f10x.h"
void Delay_Ms(u32 time);
void Delay_Us(int time);
#define SYS_TICK 1
#endif
