#include "usart.h"
/*******************串口1初始化*************
**硬件接口：USART1_TX -- PA9(发送)
**					USART1-RX --PA10(接收)
**形参：u16 baud ---波特率
********************************************/
void Usart1_Init(u32 baud)
{
	/*1.开时钟*/
	RCC->APB2ENR|=1<<2;//PA时钟
	RCC->APB2ENR|=1<<14;//串口时钟
	RCC->APB2RSTR|=1<<14;//串口复位
	RCC->APB2RSTR&=~(1<<14);//取消复位
	/*2.配置GPIO口*/
	GPIOA->CRH&=0xFFFFF00F;
	GPIOA->CRH|=0x000008B0;//上下拉输入，复用推挽输出
	/*3.USART1核心寄存器配置*/
//	USART1->CR1&=~(1<<12);//一个起始位，8个数据位
//  USART1->CR2&=~(0X3<<12);//一个停止位
	USART1->BRR=72000000/baud;//设置波特率
	USART1->CR1|=1<<3;//使能发送
	USART1->CR1|=1<<2;//使能接收
#ifdef USART1_IQR
	USART1->CR1|=1<<5;//开启串口接收中断
	STM32_NVIC_SetPriority(USART1_IRQn,0,1);//设置优先级
#endif
	USART1->CR1|=1<<13;//使能串口1
}
/**********************串口2初始化*************************
**硬件接口：USART2_TX -- PA2(发送)
**					USART2-RX --PA3(接收)
**
**形参：u16 baud ---波特率
***********************************************************/
void Usart2_Init(u32 baud)
{
	/*1.开时钟*/
	RCC->APB2ENR|=1<<2;//PA时钟
	RCC->APB1ENR|=1<<17;//USART1时钟
	RCC->APB1RSTR|=1<<17;//开复位时钟
	RCC->APB1RSTR&=~(1<<17);//取消复位
	/*2.配置GPIO口*/
	GPIOA->CRL&=0xFFFF00FF;//清除原来寄存器中的值
	GPIOA->CRL|=0x00008B00;
	/*3.配置串口核心寄存器*/
	USART2->BRR=36000000/baud;//设置波特率
	USART2->CR1|=1<<2;//接收使能
	USART2->CR1|=1<<3;//发送使能
	#ifdef USART2_IRQ
		USART2->CR1|=1<<5;//串口2接收中断
		STM32_NVIC_SetPriority(USART2_IRQn,1,2);//设置优先级
	#endif
	USART2->CR1|=1<<13;//使能串口
}
/******************串口三初始化****************************
**硬件接口：USART3_TX -- PB10
**					USART3_RX -- PB11
**形参：u32 baud -- 波特率值
*************************************************************/
void Usart3_Init(u32 baud)
{
	/*1.开时钟*/
	RCC->APB2ENR|=1<<3;//PA时钟
	RCC->APB1ENR|=1<<18;//USART3时钟
	RCC->APB1RSTR|=1<<18;//开复位时钟
	RCC->APB1RSTR&=~(1<<18);//取消复位
	/*2.配置GPIO口*/
	GPIOB->CRH&=0xFFFF00FF;
	GPIOB->CRH|=0x00008B00;
	/*3.串口核心寄存器配置*/
	USART3->BRR=36000000/baud;//波特率设置
	USART3->CR1|=1<<2;//接收使能
	USART3->CR1|=1<<3;//发送使能
	#ifdef USART3_IRQ
		USART3->CR1|=1<<5;//开启接收中断
		STM32_NVIC_SetPriority(USART3_IRQn,0,0);//设置优先级
	#endif
	USART3->CR1|=1<<13;//使能串口3
}
/********************串口初始化函数封装*********************
****硬件接口：USART1_TX -- PA9(发送)
**						USART1-RX --PA10(接收)
**						USART2_TX -- PA2(发送)
**						USART2-RX --PA3(接收)
**						USART3_TX -- PB10(发送)
**						USART3_RX -- PB11(接收)
形参：USART_TypeDef *USARTx -- 要配置的哪个串口
**			u32 baud  --波特率
**			u32 sysclk --时钟频率（USART1 --72MHZ ,USAT2\USART3 --36MHZ）
**
***********************************************************/
void Usartx_Init(USART_TypeDef *USARTx,u32 baud,u32 sysclk)
{
	if(USART1 == USARTx)
	{
			/*1.开时钟*/
		RCC->APB2ENR|=1<<2;//PA时钟
		RCC->APB2ENR|=1<<14;//串口时钟
		RCC->APB2RSTR|=1<<14;//串口复位
		RCC->APB2RSTR&=~(1<<14);//取消复位
		/*2.配置GPIO口*/
		GPIOA->CRH&=0xFFFFF00F;
		GPIOA->CRH|=0x000008B0;//上下拉输入，复用推挽输出	
		#ifdef USART1_IQR
			USART1->CR1|=1<<5;//开启串口接收中断
			STM32_NVIC_SetPriority(USART1_IRQn,0,1);//设置优先级
		#endif
	}
	else if(USART2 == USARTx)
	{
		/*1.开时钟*/
		RCC->APB2ENR|=1<<2;//PA时钟
		RCC->APB1ENR|=1<<17;//USART2时钟
		RCC->APB1RSTR|=1<<17;//开复位时钟
		RCC->APB1RSTR&=~(1<<17);//取消复位
		/*2.配置GPIO口*/
		GPIOA->CRL&=0xFFFF00FF;//清除原来寄存器中的值
		GPIOA->CRL|=0x00008B00;		
		#ifdef USART2_IRQ
			USART2->CR1|=1<<5;//串口2接收中断
			STM32_NVIC_SetPriority(USART2_IRQn,1,2);//设置优先级
		#endif
	}
	else if(USART3 == USARTx)
	{
		/*1.开时钟*/
		RCC->APB2ENR|=1<<3;//PB时钟
		RCC->APB1ENR|=1<<18;//USART3时钟
		RCC->APB1RSTR|=1<<18;//开复位时钟
		RCC->APB1RSTR&=~(1<<18);//取消复位
		/*2.配置GPIO口*/
		GPIOB->CRH&=0xFFFF00FF;
		GPIOB->CRH|=0x00008B00;	
		#ifdef USART3_IRQ
			USART3->CR1|=1<<5;//开启接收中断
			STM32_NVIC_SetPriority(USART3_IRQn,0,0);//设置优先级
		#endif
	}
	else return;
	/*3.配置串口核心寄存器*/
	USARTx->BRR=sysclk*1000000/baud;//设置波特率
	USARTx->CR1|=1<<2;//接收使能
	USARTx->CR1|=1<<3;//发送使能
	USARTx->CR1|=1<<13;//使能串口3
}
/************************串口发送字符************************/
void Usartx_SendString(USART_TypeDef *USARTx,u8 *str)
{
	while(*str!='\0')
	{
		USARTx->DR=*str;
		while((USARTx->SR&1<<7)==0){}//等待数据发送完成
		str++;
	}
}
void Usartx_SendData(USART_TypeDef *USARTx,u8 *str,u32 len)
{
	u32 i=0;
	for(i=0;i<len;i++)
	{
		USARTx->DR=str[i];
		while((USARTx->SR&1<<7)==0){}//等待数据发送完成
	}
}
/****************串口发送字符串*****************************/
void Usart1_SendStr(u8 *str)
{
	while(*str!='\0')
	{
		USART1->DR=*str++;
		while(!(USART1->SR&1<<7));
	}
}
/****************串口2发送字符串*****************************/
void Usart2_SendStr(u8 *str)
{
	while(*str!='\0')
	{
		USART2->DR=*str++;
		while(!(USART2->SR&1<<7));
	}
}
/****************串口3发送字符串*****************************/
void Usart3_SendStr(u8 *str)
{
	while(*str!='\0')
	{
		USART3->DR=*str++;
		while(!(USART3->SR&1<<7));
	}
}
/***************printf重定向**************/
int fputc(int c,FILE *stream)
{
	USART1->DR=c;
	while(!(USART1->SR&1<<7)){}
	return c;
}
u8 usart1_rx_buff[1024];//串口1接收数据缓冲区
u16 usart1_cnt=0;//保存数组下班
u8 usart1_flag;//接收完成标志符
void USART1_IRQHandler(void)
{
	u8 c;
	if(USART1->SR&1<<5)
	{
		c=USART1->DR;
		if(usart1_flag==0)//判断上一次数据是否处理完成
		{
			if(usart1_cnt<1024)	
			{
				usart1_rx_buff[usart1_cnt++]=c;
				TIM2->CNT=0;//清空计数器值
				TIM2->CR1|=1<<0;//开启定时
			}
			else usart1_flag=1;
		}
		
	}
//	USART1->SR=0;//清除标志位
}
u8 usart2_rx_buff[1024];//串口1接收数据缓冲区
u16 usart2_cnt=0;//保存数组下班
u8 usart2_flag;//接收完成标志符
void USART2_IRQHandler(void)
{
	u8 c;
	if(USART2->SR&1<<5)//判断是否接收中断触发
	{
		c=USART2->DR;
//		USART1->DR=c;
		if(usart2_flag==0)//判断上一次数据是否处理完成
		{
			if(usart2_cnt<1024)	
			{
				usart2_rx_buff[usart2_cnt++]=c;
				TIM3->CNT=0;//清空计数器值
				TIM3->CR1|=1<<0;//开启定时
			}
			else usart2_flag=1;
		}
	}
}
u8 usart3_rx_buff[1024];//串口1接收数据缓冲区
u16 usart3_cnt=0;//保存数组下班
u8 usart3_flag;//接收完成标志符
void USART3_IRQHandler(void)
{
	u8 c;
	if(USART3->SR&1<<5)//判断是否接收中断触发
	{
		c=USART3->DR;
//		USART1->DR=c;
		if(usart3_flag==0)//判断上一次数据是否处理完成
		{
			if(usart3_cnt<1024)	
			{
				usart3_rx_buff[usart3_cnt++]=c;
				TIM4->CNT=0;//清空计数器值
				TIM4->CR1|=1<<0;//开启定时
			}
			else usart3_flag=1;
		}
	}
}
