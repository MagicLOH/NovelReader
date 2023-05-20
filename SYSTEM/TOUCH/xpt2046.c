#include "xpt2046.h"
#include "nt35310.h"
#include "at24c08.h"
#include "key.h"
#include "led.h"
#include <string.h>
struct XPT2046 xpt2046_info;
/*******************触摸屏初始化************
T_MISO --  PB2 主机输入
T_MOSI --  PF9 主机输出
T_PEN --  PF10 笔中断脚(低电平按下)
T_CS -- PF11 片选
T_SCK -- PB1 时钟线
************************************************/
void XPT2046_Init(void)
{
  RCC->APB2ENR|=1<<3;
  RCC->APB2ENR|=1<<7;
  GPIOB->CRL&=0xFFFFF00F;
  GPIOB->CRL|=0x00000830;
  GPIOF->CRH&=0xFFFF000F;
  GPIOF->CRH|=0x00003830;
  GPIOF->ODR|=1<<10;//笔中断脚上拉
  T_CS=1;//片选拉高
}
/**************XTP2046读写数据*****************/
static u16 XPT2046_WRData(u8 cmd)
{
  u16 data=0;
  u8 i=0;
  T_CS=0;//选中XPT2046
  //发送命令
  for(i=0;i<8;i++)
  {
    T_SCK=0;//告诉从机，主机开始发送数据
    if(cmd&0x80)T_MOSI=1;
    else T_MOSI=0;
    T_SCK=1;//数据发送完成
    cmd<<=1;//继续发送下一位数据
  }
  //去除忙状态
  T_SCK=0;
  Delay_Us(8);
  T_SCK=1;
  //开始读取16位数据
  for(i=0;i<16;i++)
  {
    T_SCK=0;//告诉从机，主机开始读取数据
    T_SCK=1;//开始读取数据
    data<<=1;
    if(T_MISO)data|=0x01;
  }
  data>>=4;//保留高12位数据
  T_CS=1;//取消选中
  return data;
}
/*******************多次测量求平均值*********************/
static u16 XPT2046_GetAvgXY(u8 cmd)
{
  u16 buff[10];
  u8 i,j;
  u32 temp;
  for(i=0;i<10;i++)
  {
    buff[i]=XPT2046_WRData(cmd);
  }
  /*冒泡排序*/
  for(i=0;i<10-1;i++)
  {
    for(j=0;j<10-i-1;j++)
    {
      if(buff[j]>buff[j+1])
      {
        temp=buff[j];
        buff[j]=buff[j+1];
        buff[j+1]=temp;
      }
    }
  }
  /*去除最大值和最小值,求平均值*/
  temp=0;
  for(i=2;i<8;i++)
  {
    temp+=buff[i];
  }
  temp/=6;
  return temp;
}
/*************************读取xy坐标**********************
**
**发送命令格式:
** S  --起始位，必须为1
**A2A1A0： 001 --采集y坐标值
**         101 --采集x坐标值
**MODE ：0表示12位，1表示8位
**SER/DFR ：1为单端模式，0为差分模式
**PD1 PD0 ：00器件在变换之间处于低功耗模式
**
**采集x坐标:1101 0000 --0xD0
**采集y坐标:1001 0000 --0x90
*********************************************************/
u8 XPT2046_ReadXY(void)
{
  if(T_PEN==0)//判断是否按下
  {
    xpt2046_info.x0=XPT2046_GetAvgXY(0xD0);//x坐标
    xpt2046_info.y0=XPT2046_GetAvgXY(0x90);//y坐标
    xpt2046_info.x=xpt2046_info.x0*xpt2046_info.xfac+xpt2046_info.xoff;  
    xpt2046_info.y=xpt2046_info.y0*xpt2046_info.yfac+xpt2046_info.yoff;
    return 1;
  }
  return 0;
}
/******************触摸屏物理坐标和实际坐标匹配(2点校准)************
**
**左上角：(x0,y0):3607,3851   ---实际坐标(0,0)
**右下角: (x0,y0):188,182     ---实际坐标(320,480)
**x实际坐标:0~320  --- (3607-188)~(188-188)
**x方向的斜率:(3607-188)/320=10.684375
**x实际坐标转换:320-(采集的模拟值-188)/10.684375
**
**y实际坐标:0~480  --- (3851-182)~(182-182)
**y方向的斜率:(3851-182)/480=7.64375
**y实际坐标转换:480-(采集的模拟值-182)/7.64375
**
************************************************************************/
/***************************4点校准*************************************/
void Touch_Calibration(void)
{
  u8 cnt=0;
  u8 time=0;
  u16 count=0;
  u16 x1,y1,x2,y3;
  AT24C08_ReadData(TOUCH_CALIBRATION_ADDR,(u8 *)&xpt2046_info,sizeof(xpt2046_info));
  if(KEY1)//按下按键进入触摸屏校准
  {
    xpt2046_info.flag=0;
  }
  if(xpt2046_info.flag==0xab)
  {
    printf("校准完成\r\n");
    return ;
  }
  LCD_Clear(WHITE);
	LCD_Display_Str2(LCD_WIDTH/2-80,240,16,(u8 *)"请按提示进行触碰校准！",RED,WHITE);
  //绘制第一个点
  LCD_DrawLine(10,20,10+20,20,RED);
  LCD_DrawLine(20,10,20,10+20,RED);
	LCD_Refresh();
  while(1)
  {
    if(!T_PEN)
    {
      cnt++;
      count=0;
      switch(cnt)
      {
        case 1:
          x1=XPT2046_GetAvgXY(0xD0);//x坐标
          y1=XPT2046_GetAvgXY(0x90);//y坐标
          while(!T_PEN){}//等待松开
          LCD_DrawLine(10,20,10+20,20,WHITE);
          LCD_DrawLine(20,10,20,10+20,WHITE);  
          /*绘制第二个点*/
          LCD_DrawLine(LCD_WIDTH-30,20,LCD_WIDTH-10,20,RED); 
          LCD_DrawLine(LCD_WIDTH-20,10,LCD_WIDTH-20,10+20,RED);
					LCD_Refresh();
          break;
        case 2:
          x2=XPT2046_GetAvgXY(0xD0);//x坐标
          while(!T_PEN){}//等待松开
          LCD_DrawLine(LCD_WIDTH-30,20,LCD_WIDTH-10,20,WHITE); 
          LCD_DrawLine(LCD_WIDTH-20,10,LCD_WIDTH-20,10+20,WHITE);
          /*绘制第三个点*/
          LCD_DrawLine(10,LCD_HIGHT-20,10+20,LCD_HIGHT-20,RED);
          LCD_DrawLine(20,LCD_HIGHT-30,20,LCD_HIGHT-10,RED); 
					LCD_Refresh();
          break;
        case 3:
          y3=XPT2046_GetAvgXY(0x90);//y坐标
          while(!T_PEN){}//等待松开
          LCD_DrawLine(10,LCD_HIGHT-20,10+20,LCD_HIGHT-20,WHITE);
          LCD_DrawLine(20,LCD_HIGHT-30,20,LCD_HIGHT-10,WHITE); 
          /*绘制第4个点*/
          LCD_DrawLine(LCD_WIDTH-30,LCD_HIGHT-20,LCD_WIDTH-10,LCD_HIGHT-20,RED);
          LCD_DrawLine(LCD_WIDTH-20,LCD_HIGHT-30,LCD_WIDTH-20,LCD_HIGHT-10,RED);   
					LCD_Refresh();
          break;
        case 4:
          while(!T_PEN){}//等待松开
          LCD_DrawLine(LCD_WIDTH-30,LCD_HIGHT-20,LCD_WIDTH-10,LCD_HIGHT-20,WHITE);
          LCD_DrawLine(LCD_WIDTH-20,LCD_HIGHT-30,LCD_WIDTH-20,LCD_HIGHT-10,WHITE);
					LCD_Display_Str2(LCD_WIDTH/2-80,240,16,(u8 *)"                        ",RED,WHITE);
					LCD_Display_Str2(LCD_WIDTH/2-32,230,16,(u8 *)"校准完成!",RED,WHITE);
					LCD_Display_Str2(LCD_WIDTH/2-48,250,16,(u8 *)"正在保存数据",RED,WHITE);
					LCD_Refresh();
          /*计算xy的斜率和偏移量*/
          xpt2046_info.xfac=(LCD_WIDTH-40)*1.0/(x2-x1);           //得到xfac  X轴方向比例因子	 
          xpt2046_info.xoff=(LCD_WIDTH-xpt2046_info.xfac*(x2+x1))/2.0; //得到xoff  X方向偏移量
          xpt2046_info.yfac=(LCD_HIGHT-40)*1.0/(y3-y1);           //得到yfac   Y轴方向比例因子
          xpt2046_info.yoff=(LCD_HIGHT-xpt2046_info.yfac*(y3+y1))/2.0; //得到yoff   Y方向偏移量
          printf("X:%f,%f\r\n",xpt2046_info.xfac,xpt2046_info.xoff);
          printf("Y:%f,%f\r\n",xpt2046_info.yfac,xpt2046_info.yoff);
					Delay_Ms(1000);
          xpt2046_info.flag=0xab;//校准完成
          AT24C08_WriteData(TOUCH_CALIBRATION_ADDR,(u8 *)&xpt2046_info,sizeof(xpt2046_info));
					LED1=1;
          return;  
      }
      Delay_Ms(10);
      time++;
      count++;
      if(time>=10)
      {
        LED1=!LED1;
        time=0;
      }
      if(count>=500)
			{
				LED1=1;
				return ;//5s没有没有操作则退出校准模式
			}
    }
  }
}
