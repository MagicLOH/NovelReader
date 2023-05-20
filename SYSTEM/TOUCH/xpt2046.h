#ifndef _XPT2046_H
#define _XPT2046_H
#include "sys.h"
#include "stm32f10x.h"
#include "delay.h"

#define TOUCH_CALIBRATION_ADDR 800 //���津����У׼����
#define T_MISO PBin(2)
#define T_MOSI PFout(9)
#define T_PEN PFin(10)
#define T_CS PFout(11)
#define T_SCK PBout(1)
/*��������ֵ�ṹ��*/
struct XPT2046
{
  //�ɼ�������������ֵ
  u16 x0;
  u16 y0;
  /*ʵ������ֵ*/
  u16 x;
  u16 y;
  /*xy��������*/
  float xfac;//X�᷽���������	 
	float yfac;//Y�᷽���������	 
	float xoff; //X����ƫ����
	float yoff; //Y����ƫ����
  /*У׼��ɱ�־*/
  u8 flag;
};
extern struct XPT2046 xpt2046_info;
void XPT2046_Init(void);//��ʼ��
u8 XPT2046_ReadXY(void);//��ȡxy����
void Touch_Calibration(void);
#endif
