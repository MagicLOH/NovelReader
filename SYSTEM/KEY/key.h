#ifndef _KEY_H
#define _KEY_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#define KEY1 PAin(0)
#define KEY2 PEin(4)
#define KEY3 PEin(3)
#define KEY4 PEin(2)
void Key_Init(void);
u8 Key_Getval(void);
#endif
