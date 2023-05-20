#ifndef _LED_H
#define _LED_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#define LED1 PBout(5)
#define LED2 PEout(5)
void Led_Init(void);
#endif
