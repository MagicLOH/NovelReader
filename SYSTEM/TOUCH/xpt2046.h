#ifndef _XPT2046_H
#define _XPT2046_H
#include "sys.h"
#include "stm32f10x.h"
#include "delay.h"

#define TOUCH_CALIBRATION_ADDR 800 //保存触摸屏校准参数
#define T_MISO PBin(2)
#define T_MOSI PFout(9)
#define T_PEN PFin(10)
#define T_CS PFout(11)
#define T_SCK PBout(1)
/*保存坐标值结构体*/
struct XPT2046
{
  //采集到的物理坐标值
  u16 x0;
  u16 y0;
  /*实际坐标值*/
  u16 x;
  u16 y;
  /*xy比例参数*/
  float xfac;//X轴方向比例因子	 
	float yfac;//Y轴方向比例因子	 
	float xoff; //X方向偏移量
	float yoff; //Y方向偏移量
  /*校准完成标志*/
  u8 flag;
};
extern struct XPT2046 xpt2046_info;
void XPT2046_Init(void);//初始化
u8 XPT2046_ReadXY(void);//获取xy坐标
void Touch_Calibration(void);
#endif
