#include "sys.h"


//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
/*****************************�������ȼ�******************
**1.�������ȼ�����
**2.��ȡ���ȼ�����
**3.�������ȼ�
**4.ʹ���ж���
**�βΣ�IRQn_Type IRQn --- �ж���
**			uint32_t PreemptPriority -- ������ռ���ȼ�
**			uint32_t SubPriority  --���������ȼ�
*********************************************************/
void STM32_NVIC_SetPriority(IRQn_Type IRQn,uint32_t PreemptPriority, uint32_t SubPriority)
{
	uint32_t priority;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);//�������ȼ�����,��ռ����2λ�������ȼ�2λ
	priority=NVIC_EncodePriority (NVIC_PriorityGroup_2, PreemptPriority, SubPriority);//�������ȼ�����
	NVIC_SetPriority(IRQn,priority);//�������ȼ�
	NVIC_EnableIRQ(IRQn);//ʹ���ж���
}
/*****************�δ�ʱ����ʼ��******************/
void SysTick_Init(void)
{
	SysTick->CTRL&=~(1<<2);//�ⲿʱ��Դ9MHZ
	SysTick->CTRL|=1<<1;//���ж�
	SysTick->VAL=0;
	SysTick->LOAD=9000*500;//500MS
	SysTick->CTRL|=1<<0;//ʹ�ܶ�ʱ��
}
void SysTick_Handler(void)
{
	printf("ʱ�䵽\r\n");
	BEEP=!BEEP;
}
