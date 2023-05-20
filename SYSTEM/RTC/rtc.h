#ifndef _RTC_H
#define _RTC_H
#include "sys.h"
//����ʱʱ��ṹ��
struct RTC_TIME
{
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec; 
  u8 week;
  u32 time_sec;//������
};
extern struct RTC_TIME RTC_Time;//���ýṹ�����
void RTC_Init(void);
void Sec_Conversion_Time(int Sec);//��תʱ��
void Time_Conversion_Sec(u16 year,u8 month,u8 day,u8 hour,u8 min,u8 sec);//ʱ��ת��
void Get_Week(u32 sec);//��ȡ����
#endif
