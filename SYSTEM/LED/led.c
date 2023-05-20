#include "led.h"
/*******************LED初始化****************
**硬件接口：LED0 --PB5
**					LED1 --PE5
**驱动方式：低电平驱动
**
**********************************************/
void Led_Init(void)
{
	/*1.开时钟*/
	RCC->APB2ENR|=1<<3;//PB时钟使能
	RCC->APB2ENR|=1<<6;//PE时钟使能
	/*2.配置GPIO口*/
	GPIOB->CRL&=0xFF0FFFFF;//清除原来寄存器中的值
	GPIOB->CRL|=0x00300000;//PB5配置为通用推挽输出
	GPIOE->CRL&=0xFF0FFFFF;
	GPIOE->CRL|=0x00300000;
	/*3.上下拉*/
	GPIOB->ODR|=0x1<<5;//上拉
	GPIOE->ODR|=0x1<<5;//上拉
}

