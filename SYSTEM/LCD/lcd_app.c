#include "nt35310.h"
#include <string.h>
#include "xpt2046.h"
#include "led.h"
#include "beep.h"
const char *celen_buff[]={"一","二","三","四","五","六","日"};
static const u8 month_r[]={31,29,31,30,31,30,31,31,30,31,30,31};
static const u8 month_p[]={31,28,31,30,31,30,31,31,30,31,30,31};
/*判读闰平年
闰年返回1，平年返回0
***/
static int Year_judge(int year)
{
	if((year%4==0 && year%100!=0) || (year%400==0))
	{
		return 1;
	}
	return 0;
}
/*************判断某一天是星期几*************
**
**以2000年1月1号作为判断依据
**2000年1月1号 星期6
**********************************************/
static u8 Week_judge(u16 year,u8 month,u8 day)
{
  u16 i=0;
  u32 count=0;//保存总天数
  //年
  for(i=2000;i<year;i++)
  {
    if(Year_judge(i))
    {
      count+=366;
    }
    else count+=365;
  } 
 //月
  month-=1;//月份是从1月开始
  if(Year_judge(year))
  {
    for(i=0;i<month;i++)
    {
      count+=month_r[i];
    }
  }
  else
  {
    for(i=0;i<month;i++)
    {
      count+=month_p[i];
    }
  } 
  count+=day;
  /*计算星期*/
  switch(count%7)
  {
    case 0:return 5;
    case 1:return 6;
    case 2:return 7;
    case 3:return 1;
    case 4:return 2;
    case 5:return 3;
    case 6:return 4;
  }
  return 0;
}
void LcdDrawcalen(u16 year,u8 month,u8 day)
{
  u8 i=0;
  char buff[20];
  u16 y0=60;
  u8 count;
  u8 week=Week_judge(year,month,1);//计算当前月份的1号是星期几
  snprintf(buff,sizeof(buff),"%d年%d月%d日",year,month,day);
  LCD_Display_Str(20,20,16,(u8 *)buff,BLACK);//显示字符串
  LCD_DrawLine(0, 50, 320,50,GRAY);//画直线
  LCD_DrawLine(0, 51, 320,51,GRAY);//画直线
  for(i=0;i<7;i++)
  {
    LCD_Display_Font(30+40*i,y0,16,(u8 *)celen_buff[i],BLACK);//显示汉字
  }
  y0+=20;//绘制下一行
  if(Year_judge(year))//判断闰平年
  {
    count=month_r[month-1];//闰年月份天数
  }
  else
  {
    count=month_p[month-1];//平年月份天数
  }
  for(i=1;i<=count;i++)
  {
    snprintf(buff,sizeof(buff),"%2d",i);
    LCD_Display_Str(22+40*(week-1),y0,16,(u8 *)buff,BLACK);//显示字符串
    if(i==day)
    {
      OLED_DrawRectangle(18+40*(week-1),y0-4,18+40*(week-1)+22,y0-4+22,RED);//绘制矩形
    }
    week++;
    if(week>7)
    {
      week=1;
      y0+=20;
    }
  }
}
/****************绘制表盘******************/
void LcdDrawClockDial(void)
{
  u16 i=0;
  gui_circle(LCD_WIDTH/2,LCD_HIGHT-140,BLACK,100, 0);//画圆
  gui_circle(LCD_WIDTH/2,LCD_HIGHT-140,BLACK,101, 0);//画圆
  for(i=0;i<360;i+=6)//秒刻度
  {
    OLED_DrawAngleLine(LCD_WIDTH/2,LCD_HIGHT-140,i,101,10,DARKBLUE);//画任意角度直线
  }
  for(i=0;i<360;i+=30)
  {
    OLED_DrawAngleLine(LCD_WIDTH/2,LCD_HIGHT-140,i,101,18,BRRED);//画任意角度直线
  }
  LCD_Display_Str(LCD_WIDTH/2-8,LCD_HIGHT-140-80,16,(u8 *)"12",BLACK);//显示字符串
  LCD_Display_Str(LCD_WIDTH/2-4,LCD_HIGHT-140+65,16,(u8 *)"6",BLACK);//显示字符串
  LCD_Display_Str(LCD_WIDTH/2+75,LCD_HIGHT-140-8,16,(u8 *)"3",BLACK);//显示字符串
  LCD_Display_Str(LCD_WIDTH/2-80,LCD_HIGHT-140-8,16,(u8 *)"9",BLACK);//显示字符串
}
/************************绘制按键*********************/
void LCD_DrawKey(void)
{
  /*绘制按钮1*/
  LcdFill(LCD_WIDTH/2-50,100,LCD_WIDTH/2+50,150,LIGHTBLUE);
  LCD_Display_Str(LCD_WIDTH/2-strlen("按键1:LED1")*8/2,125-8,16,(u8 *)"按键1:LED1",BLACK);//显示字符串
  /*绘制按钮2*/
  LcdFill(LCD_WIDTH/2-50,200,LCD_WIDTH/2+50,250,LIGHTBLUE);
  LCD_Display_Str(LCD_WIDTH/2-strlen("按键2:LED2")*8/2,200+50/2-8,16,(u8 *)"按键2:LED2",BLACK);//显示字符串
  /*绘制按钮3*/
  LcdFill(LCD_WIDTH/2-50,300,LCD_WIDTH/2+50,350,LIGHTBLUE);
  LCD_Display_Str(LCD_WIDTH/2-strlen("按键3:BEEP")*8/2,300+50/2-8,16,(u8 *)"按键3:BEEP",BLACK);//显示字符串
}
void LCD_Check_Key(void)
{
  while(1)
  {
    if(!T_PEN)
    {
      if(XPT2046_ReadXY())//获取坐标
      {
        if((xpt2046_info.x>=LCD_WIDTH/2-50 && xpt2046_info.x<=LCD_WIDTH/2+50) && (xpt2046_info.y>=100 && xpt2046_info.y<=150))
        {
          LED1=!LED1;
          LcdFill(LCD_WIDTH/2-50,100,LCD_WIDTH/2+50,150,GRAY);
          LCD_Display_Str(LCD_WIDTH/2-strlen("按键1:LED1")*8/2,125-8,16,(u8 *)"按键1:LED1",BLACK);//显示字符串
        }
        else if((xpt2046_info.x>=LCD_WIDTH/2-50 && xpt2046_info.x<=LCD_WIDTH/2+50) && (xpt2046_info.y>=200 && xpt2046_info.y<=250))
        {
          LED2=!LED2;
          LcdFill(LCD_WIDTH/2-50,200,LCD_WIDTH/2+50,250,GRAY);
          LCD_Display_Str(LCD_WIDTH/2-strlen("按键2:LED2")*8/2,200+50/2-8,16,(u8 *)"按键2:LED2",BLACK);//显示字符串
        }
        else if((xpt2046_info.x>=LCD_WIDTH/2-50 && xpt2046_info.x<=LCD_WIDTH/2+50) && (xpt2046_info.y>=300 && xpt2046_info.y<=350))
        {
            BEEP=!BEEP;
            LcdFill(LCD_WIDTH/2-50,300,LCD_WIDTH/2+50,350,GRAY);
            LCD_Display_Str(LCD_WIDTH/2-strlen("按键3:BEEP")*8/2,300+50/2-8,16,(u8 *)"按键3:BEEP",BLACK);//显示字符串
        }
      }
      while(!T_PEN){}//等待松开
      LCD_DrawKey(); //松开后释放按键
    }
  }
}

