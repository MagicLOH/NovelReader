#include "sys.h"


//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
/*****************************设置优先级******************
**1.设置优先级分组
**2.获取优先级编码
**3.设置优先级
**4.使能中断线
**形参：IRQn_Type IRQn --- 中断线
**			uint32_t PreemptPriority -- 设置抢占优先级
**			uint32_t SubPriority  --设置子优先级
*********************************************************/
void STM32_NVIC_SetPriority(IRQn_Type IRQn,uint32_t PreemptPriority, uint32_t SubPriority)
{
	uint32_t priority;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);//设置优先级分组,抢占优先2位，子优先级2位
	priority=NVIC_EncodePriority (NVIC_PriorityGroup_2, PreemptPriority, SubPriority);//设置优先级编码
	NVIC_SetPriority(IRQn,priority);//设置优先级
	NVIC_EnableIRQ(IRQn);//使能中断线
}
/*****************滴答定时器初始化******************/
void SysTick_Init(void)
{
	SysTick->CTRL&=~(1<<2);//外部时钟源9MHZ
	SysTick->CTRL|=1<<1;//开中断
	SysTick->VAL=0;
	SysTick->LOAD=9000*500;//500MS
	SysTick->CTRL|=1<<0;//使能定时器
}
void SysTick_Handler(void)
{
	printf("时间到\r\n");
	BEEP=!BEEP;
}
