#include "delay.h"
/**************������ʱ����*********
**
**�δ�ʱ��LOADΪ24λ�Ĵ���������������ֵ��1864 -- 1.864s
**
************************************/
void Delay_Ms(u32 time)
{
    if(time==0)return ;
	#ifdef SYS_TICK
		u32 stat=0;
		SysTick->VAL=0;
		SysTick->LOAD=9000*time;//�̶�ֵ9000Ϊ1ms��ʱ
		SysTick->CTRL|=1<<0;//ʹ�ܶ�ʱ��
		do
		{
			stat=SysTick->CTRL;
		}while(!(stat&1<<16)&& (stat&1<<0));
		SysTick->CTRL=0;//�رն�ʱ��
        SysTick->VAL=0;//������ֵ����
	#else
	u32 i,j,k;
	if(time<=0)return ;
	for(i=0;i<time;i++)
		for(j=0;j<100;j++)
			for(k=0;k<100;k++);
	#endif
}
/**************΢����ʱ����*************/
void Delay_Us(int time)
{
    if(time==0)return ;
	#ifdef SYS_TICK
		u32 stat=0;
		SysTick->LOAD=9*time;//LOAD=9Ϊ�̶�1us��ʱ
		SysTick->VAL=0;//������ֵ����
		SysTick->CTRL|=1<<0;//������ʱ��
		do
		{
			stat=SysTick->CTRL;
		}while(!(stat&1<<16)&& (stat&1<<0));
		SysTick->CTRL=0;//�رյδ�ʱ��
        SysTick->VAL=0;//������ֵ����
	#else
	int i,j;
	for(i=0;i<time;i++)
		for(j=0;j<72;j++);
	#endif
}
