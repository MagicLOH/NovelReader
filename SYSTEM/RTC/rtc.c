#include "rtc.h"
#include <stdio.h>
#include "beep.h"
#include "delay.h"
#include "ff.h"
/*RTC配置*/
struct RTC_TIME RTC_Time;//定义结构体变量RTC_Time
void RTC_Init(void)
{
  if(BKP->DR5!=0x55)
  {
    /*1.开启后备域时钟和电源接口时钟*/
    RCC->APB1ENR|=1<<27;
    RCC->APB1ENR|=1<<28;
    /*2.允许RTC写操作*/
    PWR->CR|=1<<8;
    /*3.RTC时钟配置*/
    RCC->BDCR&=~(0x3<<8);//清除时钟选择
    RCC->BDCR|=0x1<<8;//选择LSE:32.768KHZ时钟源
    RCC->BDCR|=1<<0;//开启LSE时钟
    while(!(RCC->BDCR&1<<1));//等待LSE准备就绪
    RCC->BDCR|=1<<15;//开启RTC时钟
    
    /*4.配置RTC核心寄存器*/
    while(!(RTC->CRL&1<<5));//等待上次写操作完成
    RTC->CRL|=1<<4;//进入配置模式
    RTC->PRLH=0;//预分频高位
    RTC->PRLL=0x7fff;//RTC工作频率:32768HZ/(0x7fff+1)=1HZ,即CNT+1时间为1s
    RTC->CNTH=0;
    RTC->CNTL=0;//计算器值
    RTC->ALRH=0;
    RTC->ALRL=20;//闹钟值
    RTC->CRL&=~(1<<4);//退出配置模式
    while(!(RTC->CRL&1<<5));//等待上次写操作完成
    BKP->DR5=0x55;//RTC配置标志位
    PWR->CR&=~(1<<8);//禁止后备域写操作
  }
  RTC->CRH|=1<<0;//允许秒中断
  RTC->CRH|=1<<1;//允许闹钟中断
  STM32_NVIC_SetPriority(RTC_IRQn,3,3);
}
/*RTC中断服务函数*/
extern DWORD fatfs_time;
void RTC_IRQHandler(void)
{
  if(RTC->CRL&1<<0)//秒中断标志
  {
    RTC_Time.time_sec=RTC->CNTH<<16|RTC->CNTL;
    Get_Week(RTC_Time.time_sec);
    Sec_Conversion_Time(RTC_Time.time_sec);
//    printf("%d/%d/%d -- %d:%d:%d 周:%d\r\n",RTC_Time.year,
//                                      RTC_Time.month,
//                                      RTC_Time.day,
//                                      RTC_Time.hour,
//                                      RTC_Time.min,
//                                      RTC_Time.sec,
//                                      RTC_Time.week
//                                      );
    RTC->CRL&=~(1<<0);//清除秒标志
  }
  if(RTC->CRL&1<<1)//闹钟中断标志
  {
    printf("闹钟时间到\r\n");
    RTC->CRL&=~(1<<1);//清除闹钟标志
  }
}
/*判读闰平年
闰年返回1，平年返回0
***/
const u8 month_r[]={31,29,31,30,31,30,31,31,30,31,30,31};
const u8 month_p[]={31,28,31,30,31,30,31,31,30,31,30,31};
static int Year_judge(int year)
{
	if((year%4==0 && year%100!=0) || (year%400==0))
	{
		return 1;
	}
	return 0;
}

void Sec_Conversion_Time(int Sec)
{
  int i;
	//年
  RTC_Time.year=1970;//起始年份
	while(1)
	{
		if(Year_judge(RTC_Time.year))
		{
			if(Sec>=366*24*60*60)
			{
				Sec-=366*24*60*60;
				RTC_Time.year++;
			}
			else break;
		}
		else
		{
			if(Sec>=365*24*60*60)
			{
				Sec-=365*24*60*60;
				RTC_Time.year++;
			}
			else break;
		}
	}
  RTC_Time.month=1;//月
	if(Year_judge(RTC_Time.year))
	{
		for(i=0;i<12;i++)
		{
			if(Sec>=month_r[i]*24*60*60)
			{
				Sec-=month_r[i]*24*60*60;
				RTC_Time.month++;
			}
			else break;
		}
	}
	else
	{
		for(i=0;i<12;i++)
		{
			if(Sec>=month_p[i]*24*60*60)
			{
				Sec-=month_p[i]*24*60*60;
				RTC_Time.month++;
			}
			else break;
		}		
	}
  RTC_Time.day=1;//日
	while(1)
	{
		if(Sec>=24*60*60)
		{
			Sec-=24*60*60;
			RTC_Time.day++;
		}
		else break;
	}
	//时
  RTC_Time.hour=0;
	while(1)
	{
		if(Sec>=60*60)
		{
			Sec-=60*60;
			RTC_Time.hour++;
		}
		else break;
	}
	//分
  RTC_Time.min=0;
	while(1)
	{
		if(Sec>=60)
		{
			Sec-=60;
			RTC_Time.min++;
		}
		else break;
	}
	RTC_Time.sec=Sec;
}

/*************时间转换秒******************/
void Time_Conversion_Sec(u16 year,u8 month,u8 day,u8 hour,u8 min,u8 sec)
{
  u16 i=0;
  u32 cnt=0;
  //年
  for(i=1970;i<year;i++)
  {
    if(Year_judge(i))
    {
      cnt+=366*24*60*60;
    }
    else cnt+=365*24*60*60;
  }
  //月
  month-=1;//月份是从1月开始
  if(Year_judge(year))
  {
    for(i=0;i<month;i++)
    {
      cnt+=month_r[i]*24*60*60;
    }
  }
  else
  {
    for(i=0;i<month;i++)
    {
      cnt+=month_p[i]*24*60*60;
    }
  }
  cnt+=(day-1)*24*60*60;//天
  cnt+=hour*60*60;//时 
  cnt+=min*60;//分
  cnt+=sec;//秒
  //将秒时间写入RTC_CNT中
  RCC->APB1ENR|=1<<27;
  RCC->APB1ENR|=1<<28;
  /*2.允许RTC写操作*/
  PWR->CR|=1<<8;
  while(!(RTC->CRL&1<<5));//等待上次写操作完成
  RTC->CRL|=1<<4;//进入配置模式 
  RTC->CNTH=cnt>>16;
  RTC->CNTL=cnt;
  RTC->CRL&=~(1<<4);//退出配置模式 
  while(!(RTC->CRL&1<<5));//等待上次写操作完成
  PWR->CR&=~(1<<8);//关闭RTC写操
}
/**********获取星期***************
**参考时间:1970/1/1 星期4
**        
**
**********************************/
void Get_Week(u32 sec)
{
  u32 day=sec/(24*60*60);//总天数
  switch(day%7)
  {
    case 0: 
      RTC_Time.week=4;
      break;
    case 1:
      RTC_Time.week=5;
      break;
    case 2:
      RTC_Time.week=6;
      break;
    case 3:
      RTC_Time.week=7;
      break;
    case 4:
      RTC_Time.week=1;
      break;
    case 5:
      RTC_Time.week=2;
      break;
    case 6:
      RTC_Time.week=3;
      break;    
  }
}


