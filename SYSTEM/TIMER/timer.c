#include "timer.h"
/****************TIM函数封装************************
**形参：TIM_TypeDef *TIMx -- 那个定时器
**			u16 psc -- 预分频系数（0~65535）
**			u16 arr -- 重装载值（0~16）
****************************************************/
void TIMx_Init(TIM_TypeDef *TIMx,u16 psc,u16 arr)
{
	if(TIM1 == TIMx)
	{	
			//1.开时钟
		RCC->APB2ENR|=1<<11;//TIM1
		RCC->APB2RSTR|=1<<11;//TIM1复位时钟
		RCC->APB2RSTR&=~(1<<11);//TIM1复位时钟
		#ifdef TIM1_IRQ
			TIM1->DIER|=1<<0;//更新请求源
			STM32_NVIC_SetPriority(TIM1_UP_IRQn,3,2);
		#endif	
	}
	else if(TIM2 == TIMx)
	{
		/*1.开时钟*/
		RCC->APB1ENR|=1<<0;//TIM2
		RCC->APB1RSTR|=1<<0;//TIM2复位
		RCC->APB1RSTR&=~(1<<0);//取消复位	
		#ifdef TIM2_IRQ
			TIM2->DIER|=1<<0;//开启TIM2更新中断
			STM32_NVIC_SetPriority(TIM2_IRQn,1,2);//设置优先级
		#endif
	}
	else if(TIM3 == TIMx)
	{
		//1.开时钟
		RCC->APB1ENR|=1<<1;//TIM3
		RCC->APB1RSTR|=1<<1;//TIM3复位时钟
		RCC->APB1RSTR&=~(1<<1);//取消复位	
		#ifdef TIM3_IRQ
			TIM3->DIER|=1<<0;//更新请求源
			STM32_NVIC_SetPriority(TIM3_IRQn,2,1);
		#endif
	}
	else if(TIM4 == TIMx)
	{
		RCC->APB1ENR|=1<<2;//TIM4
		RCC->APB1RSTR|=1<<2;//复位时钟
		RCC->APB1RSTR&=~(1<<2);//取消复位
		#ifdef TIM4_IRQ
			TIM4->DIER|=1<<0;//更新请求源
			STM32_NVIC_SetPriority(TIM4_IRQn,0,0);
		#endif			
	}
	else return ;
		//2.配置核心寄存器
	TIMx->CNT=0;//清空计数器
	TIMx->PSC=psc-1;//预分频系数
	TIMx->ARR=arr;//重装载值
	TIMx->CR1|=1<<7;//自动重装载预装载允许位
	TIMx->EGR|=1<<0;//软件方式产生更新事件
	TIMx->SR=0;//清除状态位
	TIMx->CR1&=~(1<<0);//关闭定时器
//	TIMx->CR1|=1<<0;//开启定时器	
}
void TIM2_IRQHandler(void)
{
	if(TIM2->SR&1<<0)//判断是否为更新中断
	{
		TIM2->CR1&=~(1<<0);//关闭定时器2
		usart1_flag=1;
	}
	TIM2->SR=0;//清除标志位
}
/**************TIM3中断服务函数***************/
void TIM3_IRQHandler(void)
{
	if(TIM3->SR&1<<0)
	{
		TIM3->CR1&=~(1<<0);//关闭定时器
		usart2_flag=1;//表示数据接收完成
	}
	TIM3->SR=0;//清除标志
}

void TIM1_UP_IRQHandler(void)
{
	if(TIM1->SR&1<<0)
	{
        
	}
	TIM1->SR=0;//清除标志	
}
/***************************************************定时器高级应用*********************************************/
/********************************TIM3 PWM输出*******************************
**
**硬件接口:             没有重映像         部分重映像      完全重映像
**          TIM3_CH1       PA6                PB4               PC6
**          TIM3_CH2       PA7                PB5               PC7
**          TIM3_CH3       PB0                PB0               PC8
**          TIM3_CH4       PB1                PB1               PC9
**形参:u16 psc --预分频系数
**      u16 arr --重装载值
**      u8 chx  -- 配置， 第0位表示通道1选择，
                            第1位表示通道2选择，
                            第2位表示通道3选择，
                            第3位表示通道4选择，
**      u8 flag --0没有重映像，1部分重映像、2完全重映像
***************************************************************************/
void TIM3_PWM_Config(u16 psc,u16 arr,u8 chx,u8 flag)
{
  /*1.开时钟*/
  RCC->APB1ENR|=1<<1;//TIM3
  RCC->APB1RSTR|=1<<1;//复位时钟
  RCC->APB1RSTR&=~(1<<1);
  /*3.配置定时器基本功能*/
  TIM3->CR1|=1<<7;//自动重装载预装载允许位
  TIM3->CNT=0;//清空计数器
  TIM3->PSC=psc-1;//预分频
  TIM3->ARR=arr;
  TIM3->EGR|=1<<0;//软件方式产生更新事件
  TIM3->SR=0;//清除状态位
  /*2.GPIO口配置*/
  if(flag==1)//部分重映像  
  {
    RCC->APB2ENR|=1<<3;//PB
    RCC->APB2ENR|=1<<0;//开AFIO时钟
    AFIO->MAPR&=~(0x7<<24);
    AFIO->MAPR|=0x1<<24;//完全SWJ(JTAG-DP + SW-DP)但没有NJTRST，PB4可用
    AFIO->MAPR&=~(0x3<<10);//清空原来寄存器中的值
    AFIO->MAPR|=0x2<<10;//部分重映像
    if(chx&1<<0)
    {
      GPIOB->CRL&=0xFFF0FFFF;
      GPIOB->CRL|=0x000B0000;//PB4
    }
    if(chx&1<<1)
    {
      GPIOB->CRL&=0xFF0FFFFF;
      GPIOB->CRL|=0x00B00000;//PB5
    }
    if(chx&1<<2)
    {
      GPIOB->CRL&=0xFFFFFFF0;
      GPIOB->CRL|=0x0000000B;//PB0
    }
    if(chx&1<<3)
    {
      GPIOB->CRL&=0xFFFFFF0F;
      GPIOB->CRL|=0x000000B0;//PB1
    }
  }
  else if(flag==2)//完全重映像
  {
    RCC->APB2ENR|=1<<4;//PC
    RCC->APB2ENR|=1<<0;//开AFIO时钟  
    AFIO->MAPR&=~(0x3<<10);//清空原来寄存器中的值
    AFIO->MAPR|=0x3<<10;//部分重映像
    if(chx&1<<0)
    {
      GPIOC->CRL&=0xF0FFFFFF;
      GPIOC->CRL|=0x0B000000;//PC6
    }
    if(chx&1<<1)
    {
      GPIOC->CRL&=0x0FFFFFFF;
      GPIOC->CRL|=0xB0000000;//PC7
    }
    if(chx&1<<2)
    {
      GPIOB->CRH&=0xFFFFFFF0;
      GPIOB->CRL|=0x0000000B;//PC8
    }
    if(chx&1<<3)
    {
      GPIOB->CRH&=0xFFFFFF0F;
      GPIOB->CRL|=0x000000B0;//PC9
    }
  }
  else
  {
    RCC->APB2ENR|=1<<2;//PA
    RCC->APB2ENR|=1<<3;//PB
    if(chx&1<<0)
    {
      GPIOA->CRL&=0xF0FFFFFF;
      GPIOA->CRL|=0x0B000000;
    }
    if(chx&1<<1)
    {
      GPIOA->CRL&=0x0FFFFFFF;
      GPIOA->CRL|=0xB0000000;
    }
    if(chx&1<<2)
    {
      GPIOB->CRL&=0xFFFFFFF0;
      GPIOB->CRL|=0x0000000B;//PB0
    }
    if(chx&1<<3)
    {
      GPIOB->CRL&=0xFFFFFF0F;
      GPIOB->CRL|=0x000000B0;//PB1
    }
  }
  if(chx&1<<0)
  {
    /*CH1*/
    TIM3->CCMR1&=~(0x3<<0);//输出模式
    TIM3->CCMR1|=1<<2;//输出比较快速使能
    TIM3->CCMR1|=1<<3;//输出比较预装载使能
    TIM3->CCMR1|=0x6<<4;//PWM模式1：CNT<CCR为有效电平
    TIM3->CCER&=~(1<<1);//1，有效电平为低电平；0有效电平为高电平
    TIM3->CCR1=0;//占空比
    TIM3->CCER|=1<<0;//开启通道1    
  }
  if(chx&1<<1)
  {
    /*CH2*/
    TIM3->CCMR1&=~(0x3<<8);//输出模式
    TIM3->CCMR1|=1<<10;//输出比较快速使能
    TIM3->CCMR1|=1<<11;//输出比较预装载使能
    TIM3->CCMR1|=0x6<<12;//PWM模式1：CNT<CCR为有效电平
    TIM3->CCER&=~(1<<5);//1，有效电平为低电平；0有效电平为高电平
    TIM3->CCR2=0;//占空比
    TIM3->CCER|=1<<4;//开启通道1      
  }
  if(chx&1<<2)
  {
    /*CH1*/
    TIM3->CCMR2&=~(0x3<<0);//输出模式
    TIM3->CCMR2|=1<<2;//输出比较快速使能
    TIM3->CCMR2|=1<<3;//输出比较预装载使能
    TIM3->CCMR2|=0x6<<4;//PWM模式1：CNT<CCR为有效电平
    TIM3->CCER&=~(1<<9);//1，有效电平为低电平；0有效电平为高电平
    TIM3->CCR3=0;//占空比
    TIM3->CCER|=1<<8;//开启通道1    
  }
  if(chx&1<<3)
  {
    /*CH2*/
    TIM3->CCMR2&=~(0x3<<8);//输出模式
    TIM3->CCMR2|=1<<10;//输出比较快速使能
    TIM3->CCMR2|=1<<11;//输出比较预装载使能
    TIM3->CCMR2|=0x6<<12;//PWM模式1：CNT<CCR为有效电平
    TIM3->CCER|=1<<13;//1，有效电平为低电平；0有效电平为高电平
    TIM3->CCR4=0;//占空比
    TIM3->CCER|=1<<12;//开启通道1      
  }
  TIM3->CR1|=1<<0;//TIM3使能
}
/**********************************TIM3_CH1输出PWM*****************************/
void TIM3_PWM_CH1_Config(u16 psc,u16 arr,u8 flag,u16 ccr)
{
  /*1.开时钟*/
  RCC->APB1ENR|=1<<1;//TIM3
  RCC->APB1RSTR|=1<<1;//复位时钟
  RCC->APB1RSTR&=~(1<<1);
  /*2.GPIO口配置*/
  if(flag==1)//部分重映像
  {
    RCC->APB2ENR|=1<<3;//PB
    RCC->APB2ENR|=1<<0;//开AFIO时钟
    AFIO->MAPR&=~(0x7<<24);
    AFIO->MAPR|=0x1<<24;//完全SWJ(JTAG-DP + SW-DP)但没有NJTRST，PB4可用
    AFIO->MAPR&=~(0x3<<10);//清空原来寄存器中的值
    AFIO->MAPR|=0x2<<10;//部分重映像
    GPIOB->CRL&=0xFFF0FFFF;
    GPIOB->CRL|=0x000B0000;//复用推挽输出   
  }
  else if(flag==2)//完全重映像
  {
    RCC->APB2ENR|=1<<4;//PC
    RCC->APB2ENR|=1<<0;//开AFIO时钟  
    AFIO->MAPR&=~(0x3<<10);//清空原来寄存器中的值
    AFIO->MAPR|=0x3<<10;//部分重映像
    GPIOC->CRL&=0xF0FFFFFF;
    GPIOC->CRL|=0x0B000000;
  }
  else//没有重映像
  {
    RCC->APB2ENR|=1<<2;//PA
    GPIOA->CRL&=0xF0FFFFFF;
    GPIOA->CRL|=0x0B000000;
  }
  /*3.配置定时器基本功能*/
  TIM3->CR1|=1<<7;//自动重装载预装载允许位
  TIM3->CNT=0;//清空计数器
  TIM3->PSC=psc-1;//预分频
  TIM3->ARR=arr;
  TIM3->EGR|=1<<0;//软件方式产生更新事件
  TIM3->SR=0;//清除状态位
  /*定时器PWM模式配置*/
  TIM3->CCMR1&=~(0x3<<0);//输出模式
  TIM3->CCMR1|=1<<2;//输出比较快速使能
  TIM3->CCMR1|=1<<3;//输出比较预装载使能
  TIM3->CCMR1|=0x6<<4;//PWM模式1：CNT<CCR为有效电平
  TIM3->CCER&=~(1<<1);//1，有效电平为低电平；0有效电平为高电平
  TIM3->CCR1=ccr;//占空比
  TIM3->CCER|=1<<0;//开启通道1
  TIM3->CR1|=1<<0;//TIM3使能
}
/*************************TIM4_CH4输入捕获配置**************************
**
**硬件接口:                  部分重映像      完全重映像
**          TIM4_CH1            PB6             PD12
**          TIM4_CH2            PB7             PD13
**          TIM4_CH3            PB8             PD14
**          TIM4_CH4            PB9             PD15
*************************************************************************/
void TIM4_CH4_InputCapture_Config(u16 pas,u16 arr)
{
  /*1.开时钟*/
  RCC->APB1ENR|=1<<2;
  RCC->APB1RSTR|=1<<2;
  RCC->APB1RSTR&=~(1<<2);
  /*GPIO口配置*/
  RCC->APB2ENR|=1<<3;
  GPIOB->CRH&=0xFFFFFF0F;
  GPIOB->CRH|=0x00000080;
  /*配置定时器基本功能*/
  TIM4->CR1|=1<<7;//自动重装载预装载寄存器
  TIM4->CNT=0;//清空计数器值
  TIM4->PSC=pas-1;//预分频系数
  TIM4->ARR=arr;//重装载值
  TIM4->EGR|=1<<0;//软件方式产生更新事件
  TIM4->SR=0;//清空所有状态位
  /*TIM4_CH4输入捕获通道配置*/
  TIM4->CCMR2|=1<<8;//输入模式，CH4的值保存在CCR4中
  TIM4->CCMR2&=~(0x3<<9);//无分频，每个边沿捕获一次
  TIM4->CCMR2&=~(0xf<<12);//无滤波器
  TIM4->DIER|=1<<4;//开启捕获中断
  STM32_NVIC_SetPriority(TIM4_IRQn,0,0);//设置优先级
  TIM4->CCER|=1<<13;//捕获发生在下降沿
  TIM4->CCER|=1<<12;//捕获使能
  TIM4->CR1|=1<<0;//开启定时器
}
