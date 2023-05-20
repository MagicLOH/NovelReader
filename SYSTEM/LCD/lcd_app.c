#include "nt35310.h"
#include <string.h>
#include "xpt2046.h"
#include "led.h"
#include "beep.h"
const char *celen_buff[]={"һ","��","��","��","��","��","��"};
static const u8 month_r[]={31,29,31,30,31,30,31,31,30,31,30,31};
static const u8 month_p[]={31,28,31,30,31,30,31,31,30,31,30,31};
/*�ж���ƽ��
���귵��1��ƽ�귵��0
***/
static int Year_judge(int year)
{
	if((year%4==0 && year%100!=0) || (year%400==0))
	{
		return 1;
	}
	return 0;
}
/*************�ж�ĳһ�������ڼ�*************
**
**��2000��1��1����Ϊ�ж�����
**2000��1��1�� ����6
**********************************************/
static u8 Week_judge(u16 year,u8 month,u8 day)
{
  u16 i=0;
  u32 count=0;//����������
  //��
  for(i=2000;i<year;i++)
  {
    if(Year_judge(i))
    {
      count+=366;
    }
    else count+=365;
  } 
 //��
  month-=1;//�·��Ǵ�1�¿�ʼ
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
  /*��������*/
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
  u8 week=Week_judge(year,month,1);//���㵱ǰ�·ݵ�1�������ڼ�
  snprintf(buff,sizeof(buff),"%d��%d��%d��",year,month,day);
  LCD_Display_Str(20,20,16,(u8 *)buff,BLACK);//��ʾ�ַ���
  LCD_DrawLine(0, 50, 320,50,GRAY);//��ֱ��
  LCD_DrawLine(0, 51, 320,51,GRAY);//��ֱ��
  for(i=0;i<7;i++)
  {
    LCD_Display_Font(30+40*i,y0,16,(u8 *)celen_buff[i],BLACK);//��ʾ����
  }
  y0+=20;//������һ��
  if(Year_judge(year))//�ж���ƽ��
  {
    count=month_r[month-1];//�����·�����
  }
  else
  {
    count=month_p[month-1];//ƽ���·�����
  }
  for(i=1;i<=count;i++)
  {
    snprintf(buff,sizeof(buff),"%2d",i);
    LCD_Display_Str(22+40*(week-1),y0,16,(u8 *)buff,BLACK);//��ʾ�ַ���
    if(i==day)
    {
      OLED_DrawRectangle(18+40*(week-1),y0-4,18+40*(week-1)+22,y0-4+22,RED);//���ƾ���
    }
    week++;
    if(week>7)
    {
      week=1;
      y0+=20;
    }
  }
}
/****************���Ʊ���******************/
void LcdDrawClockDial(void)
{
  u16 i=0;
  gui_circle(LCD_WIDTH/2,LCD_HIGHT-140,BLACK,100, 0);//��Բ
  gui_circle(LCD_WIDTH/2,LCD_HIGHT-140,BLACK,101, 0);//��Բ
  for(i=0;i<360;i+=6)//��̶�
  {
    OLED_DrawAngleLine(LCD_WIDTH/2,LCD_HIGHT-140,i,101,10,DARKBLUE);//������Ƕ�ֱ��
  }
  for(i=0;i<360;i+=30)
  {
    OLED_DrawAngleLine(LCD_WIDTH/2,LCD_HIGHT-140,i,101,18,BRRED);//������Ƕ�ֱ��
  }
  LCD_Display_Str(LCD_WIDTH/2-8,LCD_HIGHT-140-80,16,(u8 *)"12",BLACK);//��ʾ�ַ���
  LCD_Display_Str(LCD_WIDTH/2-4,LCD_HIGHT-140+65,16,(u8 *)"6",BLACK);//��ʾ�ַ���
  LCD_Display_Str(LCD_WIDTH/2+75,LCD_HIGHT-140-8,16,(u8 *)"3",BLACK);//��ʾ�ַ���
  LCD_Display_Str(LCD_WIDTH/2-80,LCD_HIGHT-140-8,16,(u8 *)"9",BLACK);//��ʾ�ַ���
}
/************************���ư���*********************/
void LCD_DrawKey(void)
{
  /*���ư�ť1*/
  LcdFill(LCD_WIDTH/2-50,100,LCD_WIDTH/2+50,150,LIGHTBLUE);
  LCD_Display_Str(LCD_WIDTH/2-strlen("����1:LED1")*8/2,125-8,16,(u8 *)"����1:LED1",BLACK);//��ʾ�ַ���
  /*���ư�ť2*/
  LcdFill(LCD_WIDTH/2-50,200,LCD_WIDTH/2+50,250,LIGHTBLUE);
  LCD_Display_Str(LCD_WIDTH/2-strlen("����2:LED2")*8/2,200+50/2-8,16,(u8 *)"����2:LED2",BLACK);//��ʾ�ַ���
  /*���ư�ť3*/
  LcdFill(LCD_WIDTH/2-50,300,LCD_WIDTH/2+50,350,LIGHTBLUE);
  LCD_Display_Str(LCD_WIDTH/2-strlen("����3:BEEP")*8/2,300+50/2-8,16,(u8 *)"����3:BEEP",BLACK);//��ʾ�ַ���
}
void LCD_Check_Key(void)
{
  while(1)
  {
    if(!T_PEN)
    {
      if(XPT2046_ReadXY())//��ȡ����
      {
        if((xpt2046_info.x>=LCD_WIDTH/2-50 && xpt2046_info.x<=LCD_WIDTH/2+50) && (xpt2046_info.y>=100 && xpt2046_info.y<=150))
        {
          LED1=!LED1;
          LcdFill(LCD_WIDTH/2-50,100,LCD_WIDTH/2+50,150,GRAY);
          LCD_Display_Str(LCD_WIDTH/2-strlen("����1:LED1")*8/2,125-8,16,(u8 *)"����1:LED1",BLACK);//��ʾ�ַ���
        }
        else if((xpt2046_info.x>=LCD_WIDTH/2-50 && xpt2046_info.x<=LCD_WIDTH/2+50) && (xpt2046_info.y>=200 && xpt2046_info.y<=250))
        {
          LED2=!LED2;
          LcdFill(LCD_WIDTH/2-50,200,LCD_WIDTH/2+50,250,GRAY);
          LCD_Display_Str(LCD_WIDTH/2-strlen("����2:LED2")*8/2,200+50/2-8,16,(u8 *)"����2:LED2",BLACK);//��ʾ�ַ���
        }
        else if((xpt2046_info.x>=LCD_WIDTH/2-50 && xpt2046_info.x<=LCD_WIDTH/2+50) && (xpt2046_info.y>=300 && xpt2046_info.y<=350))
        {
            BEEP=!BEEP;
            LcdFill(LCD_WIDTH/2-50,300,LCD_WIDTH/2+50,350,GRAY);
            LCD_Display_Str(LCD_WIDTH/2-strlen("����3:BEEP")*8/2,300+50/2-8,16,(u8 *)"����3:BEEP",BLACK);//��ʾ�ַ���
        }
      }
      while(!T_PEN){}//�ȴ��ɿ�
      LCD_DrawKey(); //�ɿ����ͷŰ���
    }
  }
}

