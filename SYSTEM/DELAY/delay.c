#include "delay.h"
/**************毫秒延时函数*********
**
**滴答定时器LOAD为24位寄存器：能填入的最大值：1864 -- 1.864s
**
************************************/
void Delay_Ms(u32 time)
{
    if(time==0)return ;
	#ifdef SYS_TICK
		u32 stat=0;
		SysTick->VAL=0;
		SysTick->LOAD=9000*time;//固定值9000为1ms延时
		SysTick->CTRL|=1<<0;//使能定时器
		do
		{
			stat=SysTick->CTRL;
		}while(!(stat&1<<16)&& (stat&1<<0));
		SysTick->CTRL=0;//关闭定时器
        SysTick->VAL=0;//计数器值清零
	#else
	u32 i,j,k;
	if(time<=0)return ;
	for(i=0;i<time;i++)
		for(j=0;j<100;j++)
			for(k=0;k<100;k++);
	#endif
}
/**************微妙延时函数*************/
void Delay_Us(int time)
{
    if(time==0)return ;
	#ifdef SYS_TICK
		u32 stat=0;
		SysTick->LOAD=9*time;//LOAD=9为固定1us延时
		SysTick->VAL=0;//计数器值清零
		SysTick->CTRL|=1<<0;//开启定时器
		do
		{
			stat=SysTick->CTRL;
		}while(!(stat&1<<16)&& (stat&1<<0));
		SysTick->CTRL=0;//关闭滴答定时器
        SysTick->VAL=0;//计数器值清零
	#else
	int i,j;
	for(i=0;i<time;i++)
		for(j=0;j<72;j++);
	#endif
}
