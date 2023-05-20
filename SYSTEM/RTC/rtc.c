#include "rtc.h"
#include <stdio.h>
#include "beep.h"
#include "delay.h"
#include "ff.h"
/*RTC����*/
struct RTC_TIME RTC_Time;//����ṹ�����RTC_Time
void RTC_Init(void)
{
  if(BKP->DR5!=0x55)
  {
    /*1.��������ʱ�Ӻ͵�Դ�ӿ�ʱ��*/
    RCC->APB1ENR|=1<<27;
    RCC->APB1ENR|=1<<28;
    /*2.����RTCд����*/
    PWR->CR|=1<<8;
    /*3.RTCʱ������*/
    RCC->BDCR&=~(0x3<<8);//���ʱ��ѡ��
    RCC->BDCR|=0x1<<8;//ѡ��LSE:32.768KHZʱ��Դ
    RCC->BDCR|=1<<0;//����LSEʱ��
    while(!(RCC->BDCR&1<<1));//�ȴ�LSE׼������
    RCC->BDCR|=1<<15;//����RTCʱ��
    
    /*4.����RTC���ļĴ���*/
    while(!(RTC->CRL&1<<5));//�ȴ��ϴ�д�������
    RTC->CRL|=1<<4;//��������ģʽ
    RTC->PRLH=0;//Ԥ��Ƶ��λ
    RTC->PRLL=0x7fff;//RTC����Ƶ��:32768HZ/(0x7fff+1)=1HZ,��CNT+1ʱ��Ϊ1s
    RTC->CNTH=0;
    RTC->CNTL=0;//������ֵ
    RTC->ALRH=0;
    RTC->ALRL=20;//����ֵ
    RTC->CRL&=~(1<<4);//�˳�����ģʽ
    while(!(RTC->CRL&1<<5));//�ȴ��ϴ�д�������
    BKP->DR5=0x55;//RTC���ñ�־λ
    PWR->CR&=~(1<<8);//��ֹ����д����
  }
  RTC->CRH|=1<<0;//�������ж�
  RTC->CRH|=1<<1;//���������ж�
  STM32_NVIC_SetPriority(RTC_IRQn,3,3);
}
/*RTC�жϷ�����*/
extern DWORD fatfs_time;
void RTC_IRQHandler(void)
{
  if(RTC->CRL&1<<0)//���жϱ�־
  {
    RTC_Time.time_sec=RTC->CNTH<<16|RTC->CNTL;
    Get_Week(RTC_Time.time_sec);
    Sec_Conversion_Time(RTC_Time.time_sec);
//    printf("%d/%d/%d -- %d:%d:%d ��:%d\r\n",RTC_Time.year,
//                                      RTC_Time.month,
//                                      RTC_Time.day,
//                                      RTC_Time.hour,
//                                      RTC_Time.min,
//                                      RTC_Time.sec,
//                                      RTC_Time.week
//                                      );
    RTC->CRL&=~(1<<0);//������־
  }
  if(RTC->CRL&1<<1)//�����жϱ�־
  {
    printf("����ʱ�䵽\r\n");
    RTC->CRL&=~(1<<1);//������ӱ�־
  }
}
/*�ж���ƽ��
���귵��1��ƽ�귵��0
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
	//��
  RTC_Time.year=1970;//��ʼ���
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
  RTC_Time.month=1;//��
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
  RTC_Time.day=1;//��
	while(1)
	{
		if(Sec>=24*60*60)
		{
			Sec-=24*60*60;
			RTC_Time.day++;
		}
		else break;
	}
	//ʱ
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
	//��
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

/*************ʱ��ת����******************/
void Time_Conversion_Sec(u16 year,u8 month,u8 day,u8 hour,u8 min,u8 sec)
{
  u16 i=0;
  u32 cnt=0;
  //��
  for(i=1970;i<year;i++)
  {
    if(Year_judge(i))
    {
      cnt+=366*24*60*60;
    }
    else cnt+=365*24*60*60;
  }
  //��
  month-=1;//�·��Ǵ�1�¿�ʼ
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
  cnt+=(day-1)*24*60*60;//��
  cnt+=hour*60*60;//ʱ 
  cnt+=min*60;//��
  cnt+=sec;//��
  //����ʱ��д��RTC_CNT��
  RCC->APB1ENR|=1<<27;
  RCC->APB1ENR|=1<<28;
  /*2.����RTCд����*/
  PWR->CR|=1<<8;
  while(!(RTC->CRL&1<<5));//�ȴ��ϴ�д�������
  RTC->CRL|=1<<4;//��������ģʽ 
  RTC->CNTH=cnt>>16;
  RTC->CNTL=cnt;
  RTC->CRL&=~(1<<4);//�˳�����ģʽ 
  while(!(RTC->CRL&1<<5));//�ȴ��ϴ�д�������
  PWR->CR&=~(1<<8);//�ر�RTCд��
}
/**********��ȡ����***************
**�ο�ʱ��:1970/1/1 ����4
**        
**
**********************************/
void Get_Week(u32 sec)
{
  u32 day=sec/(24*60*60);//������
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


