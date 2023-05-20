#include "infrared.h"
#include "timer.h"
/*************�����߽��ճ�ʼ��************
**
**Ӳ���ӿڣ�REMOTE_IN -- PB9
**
******************************************/
void HS0038_Init(void)
{
  #if INFRARED_DECODE_MODE
    TIM4_CH4_InputCapture_Config(72,65535);//��ʱ�����벶��
  #else
    RCC->APB2ENR|=1<<3;//PB
    GPIOB->CRH&=0xFFFFFF0F;
    GPIOB->CRH|=0x00000080;
    /*�ⲿ�ж�����*/
    RCC->APB2ENR|=1<<0;//AFIO
    AFIO->EXTICR[2]&=~(0xf<<4);//���ԭ������
    AFIO->EXTICR[2]|=1<<4;//PB9��Ϊ�ж�����Դ
    EXTI->IMR|=1<<9;//�����ж���9
    EXTI->FTSR|=1<<9;//�½��ش���
    STM32_NVIC_SetPriority(EXTI9_5_IRQn,1,1);//�������ȼ�
    /*���ö�ʱ��*/
    TIMx_Init(TIM4,72,65535);//����Ƶ��:72MHZ/72=1MHZ,һ������ʱ��:65535*1/1=65535us
  #endif
}
u32 infrared_decode_data=0;
u8 infrared_flag=0;
u8 midea_buff[6];//�������Ŀյ�48λ����
#if INFRARED_DECODE_MODE //���벶��ʽ����
/**************TIM4�жϷ�����***************/
void TIM4_IRQHandler(void)
{
  u16 time;
  static u8 cnt=0;
  static u8 i=0;
  if(TIM4->SR&1<<4)//�����жϱ�־
  {
    TIM4->CNT=0;//���CNT��ֵ
    time=TIM4->CCR4;//����ʱ��
  #if Midea_DCODE
     if(REMOTE_IN) //��������������,�õ��͵�ƽʱ��
     {
        TIM4->CCER|=1<<13;//���������½���
        if(infrared_flag==0)
        {
          if(time>=3500 && time<=5500)infrared_flag=1;//�յ�4.5ms�͵�ƽ�ź�
          else infrared_flag=0;//����ʧ��
        }
        else if(infrared_flag==2)//��������ճɹ�
        {
          if(time<350 || time>700)infrared_flag=0;//���ʱ�����ʧ�ܣ����½���������
        }
     }
     else//���������½��أ��õ��ߵ�ƽʱ��
     {
        TIM4->CCER&=~(1<<13);//��������������
        if(infrared_flag==1)
        {
          if(time>=3500 && time<=5500)infrared_flag=2;//�յ�4.5ms�ߵ�ƽ�ź�
          else infrared_flag=0;//����ʧ��
        }
        else if(infrared_flag==2)//��ʼ����48λ����
        {
          if(time>=400 && time<=700)//����0:540us��+540us��
          {
            cnt++;
            midea_buff[i]<<=1;//����0
          }
          else if(time>=1500 && time<=1800)//����1:540us��+1600us��
          {
            cnt++;
            midea_buff[i]<<=1;//����0
            midea_buff[i]|=0x01;
          }
          else//����λ����ʧ��
          {
            cnt=0;
            i=0;
            infrared_flag=0;
          }
          if(cnt==48)//���ݽ������
          {
            cnt=0;
            i=0;
            infrared_flag=3;//���ݽ������
          }
          else if(cnt%8==0)i++;//������һ���ֽ�����
        }
     }
  #else 
    //printf("cnt=%d\r\n",cnt);

    if(REMOTE_IN)//��������������,�õ��͵�ƽʱ��
    {
      //printf("L:%d\r\n",time);
      TIM4->CCER|=1<<13;//���������½���
      if(infrared_flag==0)
      {
        if(time>=8000 && time<=10000)infrared_flag=1;//��ʾ�յ�9ms�͵�ƽ
        else infrared_flag=0;//9ms�͵�ƽ����ʧ��
      }
      else if(infrared_flag==2)//��������ճɹ�
      {
        if(time<400 || time>700)infrared_flag=0;//���ʱ�����ʧ�ܣ����¿�ʼ����
      }
    }
    else//���������½��أ��õ��ߵ�ƽʱ��
    {
     // printf("H:%d\r\n",time);
      TIM4->CCER&=~(1<<13);//��������������
      if(infrared_flag==1)
      {
        if(time>=3500 && time<=5500)infrared_flag=2;//4.5ms�ߵ�ƽ���ճɹ�
        else infrared_flag=0;//���������ʧ�����¿�ʼ����������
      }
      else if(infrared_flag==2)//��ʼ����32λ����,�Ƚӵ�λ
      {
        if(time>=400 && time<=700)//���յ�����0
        {
          cnt++;
          infrared_decode_data>>=1;
        }
        else if(time>=1550 && time<=1800)//�յ�����1
        {
          cnt++;
          infrared_decode_data>>=1;
          infrared_decode_data|=0x80000000;
        }
        else 
        {
          cnt=0;
          infrared_decode_data=0;
          infrared_flag=0;//���ݽ���ʧ�ܣ����¿�ʼ����
        }
      }
    }
    if(cnt==32)
    {
      cnt=0;
      infrared_flag=3;//���ݽ������
    }
     #endif
  }
  TIM4->SR=0;//�����־
	
}

#else
/****************��ȡ�ߵ�ƽʱ��***************/
u16 Infrared_GetH_Time(void)
{
  TIM4->CNT=0;//��ռ�����ֵ
  TIM4->CR1|=1<<0;//������ʱ��
  while(REMOTE_IN);//�ȴ���������
  TIM4->CR1&=~(1<<0);//�رն�ʱ��
  return TIM4->CNT;
}
/*******************��ȡ�͵�ƽʱ��****************/
u16 Infrared_GetL_Time(void)
{
  TIM4->CNT=0;//��ռ�����ֵ
  TIM4->CR1|=1<<0;//������ʱ��
  while(REMOTE_IN==0);//�ȴ���������
  TIM4->CR1&=~(1<<0);//�رն�ʱ��
  return TIM4->CNT; 
}
void EXTI9_5_IRQHandler(void)
{
  u16 time=0;
  u8 i=0;
  EXTI->PR|=0x1<<9;//�����־λ
  time=Infrared_GetL_Time();//��ȡ9ms�͵�ƽ
  if(time<8000 || time>10000)return ;//9ms�͵�ƽ��ȡʧ��
  time=Infrared_GetH_Time();//��ȡ4.5ms�ߵ�ƽ
  if(time<3500 || time>5500)return ;//4.5ms�ߵ�ƽ��ȡʧ��
 // printf("�������ȡ�ɹ�\r\n");
  for(i=0;i<32;i++)
  {
    time=Infrared_GetL_Time();//��ȡ560us�ĵ͵�ƽ(���ʱ��)
    if(time<400 || time>650)return ;
    time=Infrared_GetH_Time();
    if(time>=400 && time<=650)//����0��560us��+565us��
    {
      infrared_decode_data>>=1;//�ӵ�����0
    }
    else if(time>=1550 && time<=1800)//����1��560us��+1.69ms��
    {
      infrared_decode_data>>=1;//�ӵ�����0
      infrared_decode_data|=0x80000000;//��ȡ����1
    }
    else 
    {
      infrared_decode_data=0;
      return ;//���ݽ���ʧ��
    }
  }
  infrared_flag=1;//���ݽ������
}
#endif

/**************************************�����߱���****************************************
**
**VCC -=5V
**GND --GND
**DAT -- PE0
**
*****************************************************************************************/
void Infrared_Code_Init(void)
{
  #if PWM_38KHZ_MODE
    /*
    TIM3����Ƶ��:72mhz/1=72mhz,CNT+1��Ҫ��ʱ��:1/72MHZ=13.8ns
      ����38KHZ���壺T=1/38KHZ*1000=26.3us
      ��װ��ֵ:26.3us/0.0138=1895
    Ӳ���ӿ�:DATA -- PA6
    */
   TIM3_PWM_CH1_Config(1,1895,0,0);//TIM3_CH1��ʼ�� 
  #else
    RCC->APB2ENR|=1<<6;//PE
    GPIOE->CRL&=0xFFFFFFF0;
    GPIOE->CRL|=0x00000003;
  #endif
}
/*******************����38KHZ�ز��ź�***************
**
**�β�:u8 flag --1��ʾ����38khz�ز��ź�
**               0��ʾ���͵͵�ƽ�ź�
**
*****************************************************/
static void Infrared_Send38KHZ(u8 flag,u16 time)
{
#if PWM_38KHZ_MODE
  if(flag)
  {
    TIM3->CCR1=474;//ռ�ձȣ��͵�ƽ/�ߵ�ƽ=3
    Delay_Us(time);
    TIM3->CCR1=0;
  }
  else
  {
    TIM3->CCR1=0;//�������ڻָ�Ϊ�͵�ƽ
    Delay_Us(time);
  }
#else
  if(flag)//����38KHZ�ز��ź�
  {
    u16 i=0;
    for(i=0;i<time/13;i++)
    {
      INFRARED_CODE=!INFRARED_CODE;
      Delay_Us(12);
    }
  }
  else//���͵͵�ƽ
  {
    INFRARED_CODE=0;
    Delay_Us(time);
  }
#endif
}
/****************************NECЭ�鷢������**********************/
void Infrared_SendData(u8 user,u8 data)
{
  //���ݸ�ʽ:�û���+�û�����+������+���ݷ���
  //��λ�ȷ�
  u8 i=0;
  u32 infrared_data=(~data&0xff)<<24|(data<<16)|(~user&0xff)<<8|user;
  /*1.�����룺9ms��+4.5ms��*/
  Infrared_Send38KHZ(1,9000);//9ms�ߵ�ƽ
  Infrared_Send38KHZ(0,4500);//4.5ms�͵�ƽ
  /*����32λ����*/
  for(i=0;i<32;i++)
  {
    Infrared_Send38KHZ(1,560);//560us�ߵ�ƽ(���ʱ��)
    if(infrared_data&0x01)
    {
      Infrared_Send38KHZ(0,1690);//����1:560us��+1.69ms��
    }
    else
    {
      Infrared_Send38KHZ(0,565);//����0:560us��+565us��
    }
    infrared_data>>=1;
  }
  Infrared_Send38KHZ(1,560);//ֹͣλ:560us�ߵ�ƽ
}
/*****************************���Ŀյ�Э�����***************************/
void Infrared_MideaSendData(u8 *buff)
{
  u8 i,j;
  u8 data=0;
  u8 cnt=0;
Midea:
  /*1.������:4.5ms��+4.5ms��*/
  Infrared_Send38KHZ(1,4500);//4.5ms�ߵ�ƽ
  Infrared_Send38KHZ(0,4500);//4.5ms�͵�ƽ  
  /*2.����48λ����*/
  for(i=0;i<6;i++)
  {
    data=buff[i];
    for(j=0;j<8;j++)
    {
      Infrared_Send38KHZ(1,540);//540us�ߵ�ƽ(���ʱ��) 
      if(data&0x80)
      {
        Infrared_Send38KHZ(0,1600);//����1:504us��+1600us��
      }
      else 
      {
        Infrared_Send38KHZ(0,540);//����0:504us��+540us��
      }
      data<<=1;//����������һλ����
    }
  }
  cnt++;
  /*2.���ͷָ���:540us��+5.2ms��*/
  Infrared_Send38KHZ(1,540);//540us�ߵ�ƽ(���ʱ��) 
  if(cnt==1)
  {
    Infrared_Send38KHZ(0,5200);//540us�ߵ�ƽ(���ʱ��) 
    goto Midea;
  }
}






