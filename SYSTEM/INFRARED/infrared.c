#include "infrared.h"
#include "timer.h"
/*************红外线接收初始化************
**
**硬件接口：REMOTE_IN -- PB9
**
******************************************/
void HS0038_Init(void)
{
  #if INFRARED_DECODE_MODE
    TIM4_CH4_InputCapture_Config(72,65535);//定时器输入捕获
  #else
    RCC->APB2ENR|=1<<3;//PB
    GPIOB->CRH&=0xFFFFFF0F;
    GPIOB->CRH|=0x00000080;
    /*外部中断配置*/
    RCC->APB2ENR|=1<<0;//AFIO
    AFIO->EXTICR[2]&=~(0xf<<4);//清空原来配置
    AFIO->EXTICR[2]|=1<<4;//PB9作为中断输入源
    EXTI->IMR|=1<<9;//开放中断线9
    EXTI->FTSR|=1<<9;//下降沿触发
    STM32_NVIC_SetPriority(EXTI9_5_IRQn,1,1);//设置优先级
    /*配置定时器*/
    TIMx_Init(TIM4,72,65535);//工作频率:72MHZ/72=1MHZ,一个周期时间:65535*1/1=65535us
  #endif
}
u32 infrared_decode_data=0;
u8 infrared_flag=0;
u8 midea_buff[6];//保存美的空调48位数据
#if INFRARED_DECODE_MODE //输入捕获方式解析
/**************TIM4中断服务函数***************/
void TIM4_IRQHandler(void)
{
  u16 time;
  static u8 cnt=0;
  static u8 i=0;
  if(TIM4->SR&1<<4)//捕获中断标志
  {
    TIM4->CNT=0;//清空CNT的值
    time=TIM4->CCR4;//捕获时间
  #if Midea_DCODE
     if(REMOTE_IN) //捕获发生在上升沿,得到低电平时间
     {
        TIM4->CCER|=1<<13;//捕获发生在下降沿
        if(infrared_flag==0)
        {
          if(time>=3500 && time<=5500)infrared_flag=1;//收到4.5ms低电平信号
          else infrared_flag=0;//接收失败
        }
        else if(infrared_flag==2)//引导码接收成功
        {
          if(time<350 || time>700)infrared_flag=0;//间隔时间接收失败，重新接收引导码
        }
     }
     else//捕获发生在下降沿，得到高电平时间
     {
        TIM4->CCER&=~(1<<13);//捕获发生在上升沿
        if(infrared_flag==1)
        {
          if(time>=3500 && time<=5500)infrared_flag=2;//收到4.5ms高电平信号
          else infrared_flag=0;//接收失败
        }
        else if(infrared_flag==2)//开始接收48位数据
        {
          if(time>=400 && time<=700)//数据0:540us低+540us高
          {
            cnt++;
            midea_buff[i]<<=1;//数据0
          }
          else if(time>=1500 && time<=1800)//数据1:540us低+1600us高
          {
            cnt++;
            midea_buff[i]<<=1;//数据0
            midea_buff[i]|=0x01;
          }
          else//数据位接收失败
          {
            cnt=0;
            i=0;
            infrared_flag=0;
          }
          if(cnt==48)//数据接收完成
          {
            cnt=0;
            i=0;
            infrared_flag=3;//数据接收完成
          }
          else if(cnt%8==0)i++;//接收下一个字节数据
        }
     }
  #else 
    //printf("cnt=%d\r\n",cnt);

    if(REMOTE_IN)//捕获发生在上升沿,得到低电平时间
    {
      //printf("L:%d\r\n",time);
      TIM4->CCER|=1<<13;//捕获发生在下降沿
      if(infrared_flag==0)
      {
        if(time>=8000 && time<=10000)infrared_flag=1;//表示收到9ms低电平
        else infrared_flag=0;//9ms低电平接收失败
      }
      else if(infrared_flag==2)//引导码接收成功
      {
        if(time<400 || time>700)infrared_flag=0;//间隔时间接收失败，重新开始接收
      }
    }
    else//捕获发生在下降沿，得到高电平时间
    {
     // printf("H:%d\r\n",time);
      TIM4->CCER&=~(1<<13);//捕获发生在上升沿
      if(infrared_flag==1)
      {
        if(time>=3500 && time<=5500)infrared_flag=2;//4.5ms高电平接收成功
        else infrared_flag=0;//引导码接收失败重新开始接收引导码
      }
      else if(infrared_flag==2)//开始接收32位数据,先接低位
      {
        if(time>=400 && time<=700)//接收到数据0
        {
          cnt++;
          infrared_decode_data>>=1;
        }
        else if(time>=1550 && time<=1800)//收到数据1
        {
          cnt++;
          infrared_decode_data>>=1;
          infrared_decode_data|=0x80000000;
        }
        else 
        {
          cnt=0;
          infrared_decode_data=0;
          infrared_flag=0;//数据接收失败，重新开始接收
        }
      }
    }
    if(cnt==32)
    {
      cnt=0;
      infrared_flag=3;//数据接收完成
    }
     #endif
  }
  TIM4->SR=0;//清除标志
	
}

#else
/****************获取高电平时间***************/
u16 Infrared_GetH_Time(void)
{
  TIM4->CNT=0;//清空计数器值
  TIM4->CR1|=1<<0;//开启定时器
  while(REMOTE_IN);//等待引脚拉低
  TIM4->CR1&=~(1<<0);//关闭定时器
  return TIM4->CNT;
}
/*******************获取低电平时间****************/
u16 Infrared_GetL_Time(void)
{
  TIM4->CNT=0;//清空计数器值
  TIM4->CR1|=1<<0;//开启定时器
  while(REMOTE_IN==0);//等待引脚拉高
  TIM4->CR1&=~(1<<0);//关闭定时器
  return TIM4->CNT; 
}
void EXTI9_5_IRQHandler(void)
{
  u16 time=0;
  u8 i=0;
  EXTI->PR|=0x1<<9;//清除标志位
  time=Infrared_GetL_Time();//获取9ms低电平
  if(time<8000 || time>10000)return ;//9ms低电平获取失败
  time=Infrared_GetH_Time();//获取4.5ms高电平
  if(time<3500 || time>5500)return ;//4.5ms高电平获取失败
 // printf("引导码获取成功\r\n");
  for(i=0;i<32;i++)
  {
    time=Infrared_GetL_Time();//获取560us的低电平(间隔时间)
    if(time<400 || time>650)return ;
    time=Infrared_GetH_Time();
    if(time>=400 && time<=650)//数据0：560us低+565us高
    {
      infrared_decode_data>>=1;//接到数据0
    }
    else if(time>=1550 && time<=1800)//数据1：560us低+1.69ms高
    {
      infrared_decode_data>>=1;//接到数据0
      infrared_decode_data|=0x80000000;//获取数据1
    }
    else 
    {
      infrared_decode_data=0;
      return ;//数据接收失败
    }
  }
  infrared_flag=1;//数据接收完成
}
#endif

/**************************************红外线编码****************************************
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
    TIM3工作频率:72mhz/1=72mhz,CNT+1需要的时间:1/72MHZ=13.8ns
      产生38KHZ脉冲：T=1/38KHZ*1000=26.3us
      重装载值:26.3us/0.0138=1895
    硬件接口:DATA -- PA6
    */
   TIM3_PWM_CH1_Config(1,1895,0,0);//TIM3_CH1初始化 
  #else
    RCC->APB2ENR|=1<<6;//PE
    GPIOE->CRL&=0xFFFFFFF0;
    GPIOE->CRL|=0x00000003;
  #endif
}
/*******************发送38KHZ载波信号***************
**
**形参:u8 flag --1表示发送38khz载波信号
**               0表示发送低电平信号
**
*****************************************************/
static void Infrared_Send38KHZ(u8 flag,u16 time)
{
#if PWM_38KHZ_MODE
  if(flag)
  {
    TIM3->CCR1=474;//占空比，低电平/高电平=3
    Delay_Us(time);
    TIM3->CCR1=0;
  }
  else
  {
    TIM3->CCR1=0;//整个周期恢复为低电平
    Delay_Us(time);
  }
#else
  if(flag)//发送38KHZ载波信号
  {
    u16 i=0;
    for(i=0;i<time/13;i++)
    {
      INFRARED_CODE=!INFRARED_CODE;
      Delay_Us(12);
    }
  }
  else//发送低电平
  {
    INFRARED_CODE=0;
    Delay_Us(time);
  }
#endif
}
/****************************NEC协议发送数据**********************/
void Infrared_SendData(u8 user,u8 data)
{
  //数据格式:用户码+用户反码+数据码+数据反码
  //低位先发
  u8 i=0;
  u32 infrared_data=(~data&0xff)<<24|(data<<16)|(~user&0xff)<<8|user;
  /*1.引导码：9ms高+4.5ms低*/
  Infrared_Send38KHZ(1,9000);//9ms高电平
  Infrared_Send38KHZ(0,4500);//4.5ms低电平
  /*发送32位数据*/
  for(i=0;i<32;i++)
  {
    Infrared_Send38KHZ(1,560);//560us高电平(间隔时间)
    if(infrared_data&0x01)
    {
      Infrared_Send38KHZ(0,1690);//数据1:560us高+1.69ms低
    }
    else
    {
      Infrared_Send38KHZ(0,565);//数据0:560us高+565us低
    }
    infrared_data>>=1;
  }
  Infrared_Send38KHZ(1,560);//停止位:560us高电平
}
/*****************************美的空调协议编码***************************/
void Infrared_MideaSendData(u8 *buff)
{
  u8 i,j;
  u8 data=0;
  u8 cnt=0;
Midea:
  /*1.引导码:4.5ms高+4.5ms低*/
  Infrared_Send38KHZ(1,4500);//4.5ms高电平
  Infrared_Send38KHZ(0,4500);//4.5ms低电平  
  /*2.发送48位数据*/
  for(i=0;i<6;i++)
  {
    data=buff[i];
    for(j=0;j<8;j++)
    {
      Infrared_Send38KHZ(1,540);//540us高电平(间隔时间) 
      if(data&0x80)
      {
        Infrared_Send38KHZ(0,1600);//数据1:504us高+1600us低
      }
      else 
      {
        Infrared_Send38KHZ(0,540);//数据0:504us高+540us低
      }
      data<<=1;//继续发送下一位数据
    }
  }
  cnt++;
  /*2.发送分隔码:540us高+5.2ms低*/
  Infrared_Send38KHZ(1,540);//540us高电平(间隔时间) 
  if(cnt==1)
  {
    Infrared_Send38KHZ(0,5200);//540us高电平(间隔时间) 
    goto Midea;
  }
}






