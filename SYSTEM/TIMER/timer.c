#include "timer.h"
/****************TIM������װ************************
**�βΣ�TIM_TypeDef *TIMx -- �Ǹ���ʱ��
**			u16 psc -- Ԥ��Ƶϵ����0~65535��
**			u16 arr -- ��װ��ֵ��0~16��
****************************************************/
void TIMx_Init(TIM_TypeDef *TIMx,u16 psc,u16 arr)
{
	if(TIM1 == TIMx)
	{	
			//1.��ʱ��
		RCC->APB2ENR|=1<<11;//TIM1
		RCC->APB2RSTR|=1<<11;//TIM1��λʱ��
		RCC->APB2RSTR&=~(1<<11);//TIM1��λʱ��
		#ifdef TIM1_IRQ
			TIM1->DIER|=1<<0;//��������Դ
			STM32_NVIC_SetPriority(TIM1_UP_IRQn,3,2);
		#endif	
	}
	else if(TIM2 == TIMx)
	{
		/*1.��ʱ��*/
		RCC->APB1ENR|=1<<0;//TIM2
		RCC->APB1RSTR|=1<<0;//TIM2��λ
		RCC->APB1RSTR&=~(1<<0);//ȡ����λ	
		#ifdef TIM2_IRQ
			TIM2->DIER|=1<<0;//����TIM2�����ж�
			STM32_NVIC_SetPriority(TIM2_IRQn,1,2);//�������ȼ�
		#endif
	}
	else if(TIM3 == TIMx)
	{
		//1.��ʱ��
		RCC->APB1ENR|=1<<1;//TIM3
		RCC->APB1RSTR|=1<<1;//TIM3��λʱ��
		RCC->APB1RSTR&=~(1<<1);//ȡ����λ	
		#ifdef TIM3_IRQ
			TIM3->DIER|=1<<0;//��������Դ
			STM32_NVIC_SetPriority(TIM3_IRQn,2,1);
		#endif
	}
	else if(TIM4 == TIMx)
	{
		RCC->APB1ENR|=1<<2;//TIM4
		RCC->APB1RSTR|=1<<2;//��λʱ��
		RCC->APB1RSTR&=~(1<<2);//ȡ����λ
		#ifdef TIM4_IRQ
			TIM4->DIER|=1<<0;//��������Դ
			STM32_NVIC_SetPriority(TIM4_IRQn,0,0);
		#endif			
	}
	else return ;
		//2.���ú��ļĴ���
	TIMx->CNT=0;//��ռ�����
	TIMx->PSC=psc-1;//Ԥ��Ƶϵ��
	TIMx->ARR=arr;//��װ��ֵ
	TIMx->CR1|=1<<7;//�Զ���װ��Ԥװ������λ
	TIMx->EGR|=1<<0;//�����ʽ���������¼�
	TIMx->SR=0;//���״̬λ
	TIMx->CR1&=~(1<<0);//�رն�ʱ��
//	TIMx->CR1|=1<<0;//������ʱ��	
}
void TIM2_IRQHandler(void)
{
	if(TIM2->SR&1<<0)//�ж��Ƿ�Ϊ�����ж�
	{
		TIM2->CR1&=~(1<<0);//�رն�ʱ��2
		usart1_flag=1;
	}
	TIM2->SR=0;//�����־λ
}
/**************TIM3�жϷ�����***************/
void TIM3_IRQHandler(void)
{
	if(TIM3->SR&1<<0)
	{
		TIM3->CR1&=~(1<<0);//�رն�ʱ��
		usart2_flag=1;//��ʾ���ݽ������
	}
	TIM3->SR=0;//�����־
}

void TIM1_UP_IRQHandler(void)
{
	if(TIM1->SR&1<<0)
	{
        
	}
	TIM1->SR=0;//�����־	
}
/***************************************************��ʱ���߼�Ӧ��*********************************************/
/********************************TIM3 PWM���*******************************
**
**Ӳ���ӿ�:             û����ӳ��         ������ӳ��      ��ȫ��ӳ��
**          TIM3_CH1       PA6                PB4               PC6
**          TIM3_CH2       PA7                PB5               PC7
**          TIM3_CH3       PB0                PB0               PC8
**          TIM3_CH4       PB1                PB1               PC9
**�β�:u16 psc --Ԥ��Ƶϵ��
**      u16 arr --��װ��ֵ
**      u8 chx  -- ���ã� ��0λ��ʾͨ��1ѡ��
                            ��1λ��ʾͨ��2ѡ��
                            ��2λ��ʾͨ��3ѡ��
                            ��3λ��ʾͨ��4ѡ��
**      u8 flag --0û����ӳ��1������ӳ��2��ȫ��ӳ��
***************************************************************************/
void TIM3_PWM_Config(u16 psc,u16 arr,u8 chx,u8 flag)
{
  /*1.��ʱ��*/
  RCC->APB1ENR|=1<<1;//TIM3
  RCC->APB1RSTR|=1<<1;//��λʱ��
  RCC->APB1RSTR&=~(1<<1);
  /*3.���ö�ʱ����������*/
  TIM3->CR1|=1<<7;//�Զ���װ��Ԥװ������λ
  TIM3->CNT=0;//��ռ�����
  TIM3->PSC=psc-1;//Ԥ��Ƶ
  TIM3->ARR=arr;
  TIM3->EGR|=1<<0;//�����ʽ���������¼�
  TIM3->SR=0;//���״̬λ
  /*2.GPIO������*/
  if(flag==1)//������ӳ��  
  {
    RCC->APB2ENR|=1<<3;//PB
    RCC->APB2ENR|=1<<0;//��AFIOʱ��
    AFIO->MAPR&=~(0x7<<24);
    AFIO->MAPR|=0x1<<24;//��ȫSWJ(JTAG-DP + SW-DP)��û��NJTRST��PB4����
    AFIO->MAPR&=~(0x3<<10);//���ԭ���Ĵ����е�ֵ
    AFIO->MAPR|=0x2<<10;//������ӳ��
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
  else if(flag==2)//��ȫ��ӳ��
  {
    RCC->APB2ENR|=1<<4;//PC
    RCC->APB2ENR|=1<<0;//��AFIOʱ��  
    AFIO->MAPR&=~(0x3<<10);//���ԭ���Ĵ����е�ֵ
    AFIO->MAPR|=0x3<<10;//������ӳ��
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
    TIM3->CCMR1&=~(0x3<<0);//���ģʽ
    TIM3->CCMR1|=1<<2;//����ȽϿ���ʹ��
    TIM3->CCMR1|=1<<3;//����Ƚ�Ԥװ��ʹ��
    TIM3->CCMR1|=0x6<<4;//PWMģʽ1��CNT<CCRΪ��Ч��ƽ
    TIM3->CCER&=~(1<<1);//1����Ч��ƽΪ�͵�ƽ��0��Ч��ƽΪ�ߵ�ƽ
    TIM3->CCR1=0;//ռ�ձ�
    TIM3->CCER|=1<<0;//����ͨ��1    
  }
  if(chx&1<<1)
  {
    /*CH2*/
    TIM3->CCMR1&=~(0x3<<8);//���ģʽ
    TIM3->CCMR1|=1<<10;//����ȽϿ���ʹ��
    TIM3->CCMR1|=1<<11;//����Ƚ�Ԥװ��ʹ��
    TIM3->CCMR1|=0x6<<12;//PWMģʽ1��CNT<CCRΪ��Ч��ƽ
    TIM3->CCER&=~(1<<5);//1����Ч��ƽΪ�͵�ƽ��0��Ч��ƽΪ�ߵ�ƽ
    TIM3->CCR2=0;//ռ�ձ�
    TIM3->CCER|=1<<4;//����ͨ��1      
  }
  if(chx&1<<2)
  {
    /*CH1*/
    TIM3->CCMR2&=~(0x3<<0);//���ģʽ
    TIM3->CCMR2|=1<<2;//����ȽϿ���ʹ��
    TIM3->CCMR2|=1<<3;//����Ƚ�Ԥװ��ʹ��
    TIM3->CCMR2|=0x6<<4;//PWMģʽ1��CNT<CCRΪ��Ч��ƽ
    TIM3->CCER&=~(1<<9);//1����Ч��ƽΪ�͵�ƽ��0��Ч��ƽΪ�ߵ�ƽ
    TIM3->CCR3=0;//ռ�ձ�
    TIM3->CCER|=1<<8;//����ͨ��1    
  }
  if(chx&1<<3)
  {
    /*CH2*/
    TIM3->CCMR2&=~(0x3<<8);//���ģʽ
    TIM3->CCMR2|=1<<10;//����ȽϿ���ʹ��
    TIM3->CCMR2|=1<<11;//����Ƚ�Ԥװ��ʹ��
    TIM3->CCMR2|=0x6<<12;//PWMģʽ1��CNT<CCRΪ��Ч��ƽ
    TIM3->CCER|=1<<13;//1����Ч��ƽΪ�͵�ƽ��0��Ч��ƽΪ�ߵ�ƽ
    TIM3->CCR4=0;//ռ�ձ�
    TIM3->CCER|=1<<12;//����ͨ��1      
  }
  TIM3->CR1|=1<<0;//TIM3ʹ��
}
/**********************************TIM3_CH1���PWM*****************************/
void TIM3_PWM_CH1_Config(u16 psc,u16 arr,u8 flag,u16 ccr)
{
  /*1.��ʱ��*/
  RCC->APB1ENR|=1<<1;//TIM3
  RCC->APB1RSTR|=1<<1;//��λʱ��
  RCC->APB1RSTR&=~(1<<1);
  /*2.GPIO������*/
  if(flag==1)//������ӳ��
  {
    RCC->APB2ENR|=1<<3;//PB
    RCC->APB2ENR|=1<<0;//��AFIOʱ��
    AFIO->MAPR&=~(0x7<<24);
    AFIO->MAPR|=0x1<<24;//��ȫSWJ(JTAG-DP + SW-DP)��û��NJTRST��PB4����
    AFIO->MAPR&=~(0x3<<10);//���ԭ���Ĵ����е�ֵ
    AFIO->MAPR|=0x2<<10;//������ӳ��
    GPIOB->CRL&=0xFFF0FFFF;
    GPIOB->CRL|=0x000B0000;//�����������   
  }
  else if(flag==2)//��ȫ��ӳ��
  {
    RCC->APB2ENR|=1<<4;//PC
    RCC->APB2ENR|=1<<0;//��AFIOʱ��  
    AFIO->MAPR&=~(0x3<<10);//���ԭ���Ĵ����е�ֵ
    AFIO->MAPR|=0x3<<10;//������ӳ��
    GPIOC->CRL&=0xF0FFFFFF;
    GPIOC->CRL|=0x0B000000;
  }
  else//û����ӳ��
  {
    RCC->APB2ENR|=1<<2;//PA
    GPIOA->CRL&=0xF0FFFFFF;
    GPIOA->CRL|=0x0B000000;
  }
  /*3.���ö�ʱ����������*/
  TIM3->CR1|=1<<7;//�Զ���װ��Ԥװ������λ
  TIM3->CNT=0;//��ռ�����
  TIM3->PSC=psc-1;//Ԥ��Ƶ
  TIM3->ARR=arr;
  TIM3->EGR|=1<<0;//�����ʽ���������¼�
  TIM3->SR=0;//���״̬λ
  /*��ʱ��PWMģʽ����*/
  TIM3->CCMR1&=~(0x3<<0);//���ģʽ
  TIM3->CCMR1|=1<<2;//����ȽϿ���ʹ��
  TIM3->CCMR1|=1<<3;//����Ƚ�Ԥװ��ʹ��
  TIM3->CCMR1|=0x6<<4;//PWMģʽ1��CNT<CCRΪ��Ч��ƽ
  TIM3->CCER&=~(1<<1);//1����Ч��ƽΪ�͵�ƽ��0��Ч��ƽΪ�ߵ�ƽ
  TIM3->CCR1=ccr;//ռ�ձ�
  TIM3->CCER|=1<<0;//����ͨ��1
  TIM3->CR1|=1<<0;//TIM3ʹ��
}
/*************************TIM4_CH4���벶������**************************
**
**Ӳ���ӿ�:                  ������ӳ��      ��ȫ��ӳ��
**          TIM4_CH1            PB6             PD12
**          TIM4_CH2            PB7             PD13
**          TIM4_CH3            PB8             PD14
**          TIM4_CH4            PB9             PD15
*************************************************************************/
void TIM4_CH4_InputCapture_Config(u16 pas,u16 arr)
{
  /*1.��ʱ��*/
  RCC->APB1ENR|=1<<2;
  RCC->APB1RSTR|=1<<2;
  RCC->APB1RSTR&=~(1<<2);
  /*GPIO������*/
  RCC->APB2ENR|=1<<3;
  GPIOB->CRH&=0xFFFFFF0F;
  GPIOB->CRH|=0x00000080;
  /*���ö�ʱ����������*/
  TIM4->CR1|=1<<7;//�Զ���װ��Ԥװ�ؼĴ���
  TIM4->CNT=0;//��ռ�����ֵ
  TIM4->PSC=pas-1;//Ԥ��Ƶϵ��
  TIM4->ARR=arr;//��װ��ֵ
  TIM4->EGR|=1<<0;//�����ʽ���������¼�
  TIM4->SR=0;//�������״̬λ
  /*TIM4_CH4���벶��ͨ������*/
  TIM4->CCMR2|=1<<8;//����ģʽ��CH4��ֵ������CCR4��
  TIM4->CCMR2&=~(0x3<<9);//�޷�Ƶ��ÿ�����ز���һ��
  TIM4->CCMR2&=~(0xf<<12);//���˲���
  TIM4->DIER|=1<<4;//���������ж�
  STM32_NVIC_SetPriority(TIM4_IRQn,0,0);//�������ȼ�
  TIM4->CCER|=1<<13;//���������½���
  TIM4->CCER|=1<<12;//����ʹ��
  TIM4->CR1|=1<<0;//������ʱ��
}
