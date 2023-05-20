#ifndef _RTC_H
#define _RTC_H
#include "sys.h"
//定义时时间结构体
struct RTC_TIME
{
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec; 
  u8 week;
  u32 time_sec;//总秒数
};
extern struct RTC_TIME RTC_Time;//引用结构体变量
void RTC_Init(void);
void Sec_Conversion_Time(int Sec);//秒转时间
void Time_Conversion_Sec(u16 year,u8 month,u8 day,u8 hour,u8 min,u8 sec);//时间转秒
void Get_Week(u32 sec);//获取星期
#endif
