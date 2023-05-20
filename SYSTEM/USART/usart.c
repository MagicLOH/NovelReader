#include "usart.h"
/*******************����1��ʼ��*************
**Ӳ���ӿڣ�USART1_TX -- PA9(����)
**					USART1-RX --PA10(����)
**�βΣ�u16 baud ---������
********************************************/
void Usart1_Init(u32 baud)
{
	/*1.��ʱ��*/
	RCC->APB2ENR|=1<<2;//PAʱ��
	RCC->APB2ENR|=1<<14;//����ʱ��
	RCC->APB2RSTR|=1<<14;//���ڸ�λ
	RCC->APB2RSTR&=~(1<<14);//ȡ����λ
	/*2.����GPIO��*/
	GPIOA->CRH&=0xFFFFF00F;
	GPIOA->CRH|=0x000008B0;//���������룬�����������
	/*3.USART1���ļĴ�������*/
//	USART1->CR1&=~(1<<12);//һ����ʼλ��8������λ
//  USART1->CR2&=~(0X3<<12);//һ��ֹͣλ
	USART1->BRR=72000000/baud;//���ò�����
	USART1->CR1|=1<<3;//ʹ�ܷ���
	USART1->CR1|=1<<2;//ʹ�ܽ���
#ifdef USART1_IQR
	USART1->CR1|=1<<5;//�������ڽ����ж�
	STM32_NVIC_SetPriority(USART1_IRQn,0,1);//�������ȼ�
#endif
	USART1->CR1|=1<<13;//ʹ�ܴ���1
}
/**********************����2��ʼ��*************************
**Ӳ���ӿڣ�USART2_TX -- PA2(����)
**					USART2-RX --PA3(����)
**
**�βΣ�u16 baud ---������
***********************************************************/
void Usart2_Init(u32 baud)
{
	/*1.��ʱ��*/
	RCC->APB2ENR|=1<<2;//PAʱ��
	RCC->APB1ENR|=1<<17;//USART1ʱ��
	RCC->APB1RSTR|=1<<17;//����λʱ��
	RCC->APB1RSTR&=~(1<<17);//ȡ����λ
	/*2.����GPIO��*/
	GPIOA->CRL&=0xFFFF00FF;//���ԭ���Ĵ����е�ֵ
	GPIOA->CRL|=0x00008B00;
	/*3.���ô��ں��ļĴ���*/
	USART2->BRR=36000000/baud;//���ò�����
	USART2->CR1|=1<<2;//����ʹ��
	USART2->CR1|=1<<3;//����ʹ��
	#ifdef USART2_IRQ
		USART2->CR1|=1<<5;//����2�����ж�
		STM32_NVIC_SetPriority(USART2_IRQn,1,2);//�������ȼ�
	#endif
	USART2->CR1|=1<<13;//ʹ�ܴ���
}
/******************��������ʼ��****************************
**Ӳ���ӿڣ�USART3_TX -- PB10
**					USART3_RX -- PB11
**�βΣ�u32 baud -- ������ֵ
*************************************************************/
void Usart3_Init(u32 baud)
{
	/*1.��ʱ��*/
	RCC->APB2ENR|=1<<3;//PAʱ��
	RCC->APB1ENR|=1<<18;//USART3ʱ��
	RCC->APB1RSTR|=1<<18;//����λʱ��
	RCC->APB1RSTR&=~(1<<18);//ȡ����λ
	/*2.����GPIO��*/
	GPIOB->CRH&=0xFFFF00FF;
	GPIOB->CRH|=0x00008B00;
	/*3.���ں��ļĴ�������*/
	USART3->BRR=36000000/baud;//����������
	USART3->CR1|=1<<2;//����ʹ��
	USART3->CR1|=1<<3;//����ʹ��
	#ifdef USART3_IRQ
		USART3->CR1|=1<<5;//���������ж�
		STM32_NVIC_SetPriority(USART3_IRQn,0,0);//�������ȼ�
	#endif
	USART3->CR1|=1<<13;//ʹ�ܴ���3
}
/********************���ڳ�ʼ��������װ*********************
****Ӳ���ӿڣ�USART1_TX -- PA9(����)
**						USART1-RX --PA10(����)
**						USART2_TX -- PA2(����)
**						USART2-RX --PA3(����)
**						USART3_TX -- PB10(����)
**						USART3_RX -- PB11(����)
�βΣ�USART_TypeDef *USARTx -- Ҫ���õ��ĸ�����
**			u32 baud  --������
**			u32 sysclk --ʱ��Ƶ�ʣ�USART1 --72MHZ ,USAT2\USART3 --36MHZ��
**
***********************************************************/
void Usartx_Init(USART_TypeDef *USARTx,u32 baud,u32 sysclk)
{
	if(USART1 == USARTx)
	{
			/*1.��ʱ��*/
		RCC->APB2ENR|=1<<2;//PAʱ��
		RCC->APB2ENR|=1<<14;//����ʱ��
		RCC->APB2RSTR|=1<<14;//���ڸ�λ
		RCC->APB2RSTR&=~(1<<14);//ȡ����λ
		/*2.����GPIO��*/
		GPIOA->CRH&=0xFFFFF00F;
		GPIOA->CRH|=0x000008B0;//���������룬�����������	
		#ifdef USART1_IQR
			USART1->CR1|=1<<5;//�������ڽ����ж�
			STM32_NVIC_SetPriority(USART1_IRQn,0,1);//�������ȼ�
		#endif
	}
	else if(USART2 == USARTx)
	{
		/*1.��ʱ��*/
		RCC->APB2ENR|=1<<2;//PAʱ��
		RCC->APB1ENR|=1<<17;//USART2ʱ��
		RCC->APB1RSTR|=1<<17;//����λʱ��
		RCC->APB1RSTR&=~(1<<17);//ȡ����λ
		/*2.����GPIO��*/
		GPIOA->CRL&=0xFFFF00FF;//���ԭ���Ĵ����е�ֵ
		GPIOA->CRL|=0x00008B00;		
		#ifdef USART2_IRQ
			USART2->CR1|=1<<5;//����2�����ж�
			STM32_NVIC_SetPriority(USART2_IRQn,1,2);//�������ȼ�
		#endif
	}
	else if(USART3 == USARTx)
	{
		/*1.��ʱ��*/
		RCC->APB2ENR|=1<<3;//PBʱ��
		RCC->APB1ENR|=1<<18;//USART3ʱ��
		RCC->APB1RSTR|=1<<18;//����λʱ��
		RCC->APB1RSTR&=~(1<<18);//ȡ����λ
		/*2.����GPIO��*/
		GPIOB->CRH&=0xFFFF00FF;
		GPIOB->CRH|=0x00008B00;	
		#ifdef USART3_IRQ
			USART3->CR1|=1<<5;//���������ж�
			STM32_NVIC_SetPriority(USART3_IRQn,0,0);//�������ȼ�
		#endif
	}
	else return;
	/*3.���ô��ں��ļĴ���*/
	USARTx->BRR=sysclk*1000000/baud;//���ò�����
	USARTx->CR1|=1<<2;//����ʹ��
	USARTx->CR1|=1<<3;//����ʹ��
	USARTx->CR1|=1<<13;//ʹ�ܴ���3
}
/************************���ڷ����ַ�************************/
void Usartx_SendString(USART_TypeDef *USARTx,u8 *str)
{
	while(*str!='\0')
	{
		USARTx->DR=*str;
		while((USARTx->SR&1<<7)==0){}//�ȴ����ݷ������
		str++;
	}
}
void Usartx_SendData(USART_TypeDef *USARTx,u8 *str,u32 len)
{
	u32 i=0;
	for(i=0;i<len;i++)
	{
		USARTx->DR=str[i];
		while((USARTx->SR&1<<7)==0){}//�ȴ����ݷ������
	}
}
/****************���ڷ����ַ���*****************************/
void Usart1_SendStr(u8 *str)
{
	while(*str!='\0')
	{
		USART1->DR=*str++;
		while(!(USART1->SR&1<<7));
	}
}
/****************����2�����ַ���*****************************/
void Usart2_SendStr(u8 *str)
{
	while(*str!='\0')
	{
		USART2->DR=*str++;
		while(!(USART2->SR&1<<7));
	}
}
/****************����3�����ַ���*****************************/
void Usart3_SendStr(u8 *str)
{
	while(*str!='\0')
	{
		USART3->DR=*str++;
		while(!(USART3->SR&1<<7));
	}
}
/***************printf�ض���**************/
int fputc(int c,FILE *stream)
{
	USART1->DR=c;
	while(!(USART1->SR&1<<7)){}
	return c;
}
u8 usart1_rx_buff[1024];//����1�������ݻ�����
u16 usart1_cnt=0;//���������°�
u8 usart1_flag;//������ɱ�־��
void USART1_IRQHandler(void)
{
	u8 c;
	if(USART1->SR&1<<5)
	{
		c=USART1->DR;
		if(usart1_flag==0)//�ж���һ�������Ƿ������
		{
			if(usart1_cnt<1024)	
			{
				usart1_rx_buff[usart1_cnt++]=c;
				TIM2->CNT=0;//��ռ�����ֵ
				TIM2->CR1|=1<<0;//������ʱ
			}
			else usart1_flag=1;
		}
		
	}
//	USART1->SR=0;//�����־λ
}
u8 usart2_rx_buff[1024];//����1�������ݻ�����
u16 usart2_cnt=0;//���������°�
u8 usart2_flag;//������ɱ�־��
void USART2_IRQHandler(void)
{
	u8 c;
	if(USART2->SR&1<<5)//�ж��Ƿ�����жϴ���
	{
		c=USART2->DR;
//		USART1->DR=c;
		if(usart2_flag==0)//�ж���һ�������Ƿ������
		{
			if(usart2_cnt<1024)	
			{
				usart2_rx_buff[usart2_cnt++]=c;
				TIM3->CNT=0;//��ռ�����ֵ
				TIM3->CR1|=1<<0;//������ʱ
			}
			else usart2_flag=1;
		}
	}
}
u8 usart3_rx_buff[1024];//����1�������ݻ�����
u16 usart3_cnt=0;//���������°�
u8 usart3_flag;//������ɱ�־��
void USART3_IRQHandler(void)
{
	u8 c;
	if(USART3->SR&1<<5)//�ж��Ƿ�����жϴ���
	{
		c=USART3->DR;
//		USART1->DR=c;
		if(usart3_flag==0)//�ж���һ�������Ƿ������
		{
			if(usart3_cnt<1024)	
			{
				usart3_rx_buff[usart3_cnt++]=c;
				TIM4->CNT=0;//��ռ�����ֵ
				TIM4->CR1|=1<<0;//������ʱ
			}
			else usart3_flag=1;
		}
	}
}
