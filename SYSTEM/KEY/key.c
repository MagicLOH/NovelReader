#include "key.h"
/****************按键初始化*********************
**硬件接口：KEY1 --PA0按下为高电平
**					KEY2 --PE4 按下为低电平
**					KEY3 --PE3 按下为低电平
**					KEY4 --PE2按下为低电平
************************************************/
void Key_Init(void)
{
	/*1.开时钟*/
	RCC->APB2ENR|=1<<2;//开启PA时钟
	RCC->APB2ENR|=1<<6;
	/*2.配置GPIO口*/
	GPIOA->CRL&=0xFFFFFFF0;//清除原来寄存器中的值
	GPIOA->CRL|=0x00000008;//配置为上下拉输入方式
	GPIOE->CRL&=0xFFF000FF;
	GPIOE->CRL|=0x00088800;
	/*3.上拉*/
	GPIOE->ODR|=0x7<<2;//PE2、3、4上拉
}
/***************检测按键函数*************
**硬件接口：KEY1 --PA0 按下为高电平
**					KEY2 --PE4 按下为低电平
**					KEY3 --PE3按下为低电平
**					KEY4 --PE2按下为低电平
**返回值：KEY1按下 --返回1
**				KEY2按下 --返回2
**				KEY3按下 --返回3
**				KEY4按下 --返回4
**没有按键按下：返回0
**注：该函数一次只能检测一个按键是否按下
******************************************/
u8  Key_Getval(void)
{
	static u8 stat=0;//按键按下标志位
	if((KEY1 || (KEY2==0) || (KEY3==0) || (KEY4==0)) && (stat==0))//是否有按键按下
	{
		Delay_Ms(25);//延时消抖
		stat=1;//表示有按键按下
		if(KEY1)return 1;
		else if(KEY2==0)return 2;
		else if(KEY3==0)return 3;
		else if(KEY4==0)return 4;
	}
	else
	{
		if((KEY1==0) && KEY2 && KEY3 && KEY4)stat=0;//是否所有按键都松开
	}
	return 0;//没有按键按下
}


