#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f10x.h"
#include "sys.h"
#include "beep.h"
#include "led.h"
#include "usart.h"
#include <stdio.h>
void TIMx_Init(TIM_TypeDef *TIMx,u16 psc,u16 arr);
void TIM3_PWM_CH1_Config(u16 psc,u16 arr,u8 flag,u16 ccr);//TIM3_CH1初始化
void TIM3_PWM_Config(u16 psc,u16 arr,u8 chx,u8 flag);
void TIM4_CH4_InputCapture_Config(u16 pas,u16 arr);//TIM4_CH4输入捕获配置
#endif
