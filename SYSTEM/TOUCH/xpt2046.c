#include "xpt2046.h"
#include "nt35310.h"
#include "at24c08.h"
#include "key.h"
#include "led.h"
#include <string.h>
struct XPT2046 xpt2046_info;
/*******************��������ʼ��************
T_MISO --  PB2 ��������
T_MOSI --  PF9 �������
T_PEN --  PF10 ���жϽ�(�͵�ƽ����)
T_CS -- PF11 Ƭѡ
T_SCK -- PB1 ʱ����
************************************************/
void XPT2046_Init(void)
{
  RCC->APB2ENR|=1<<3;
  RCC->APB2ENR|=1<<7;
  GPIOB->CRL&=0xFFFFF00F;
  GPIOB->CRL|=0x00000830;
  GPIOF->CRH&=0xFFFF000F;
  GPIOF->CRH|=0x00003830;
  GPIOF->ODR|=1<<10;//���жϽ�����
  T_CS=1;//Ƭѡ����
}
/**************XTP2046��д����*****************/
static u16 XPT2046_WRData(u8 cmd)
{
  u16 data=0;
  u8 i=0;
  T_CS=0;//ѡ��XPT2046
  //��������
  for(i=0;i<8;i++)
  {
    T_SCK=0;//���ߴӻ���������ʼ��������
    if(cmd&0x80)T_MOSI=1;
    else T_MOSI=0;
    T_SCK=1;//���ݷ������
    cmd<<=1;//����������һλ����
  }
  //ȥ��æ״̬
  T_SCK=0;
  Delay_Us(8);
  T_SCK=1;
  //��ʼ��ȡ16λ����
  for(i=0;i<16;i++)
  {
    T_SCK=0;//���ߴӻ���������ʼ��ȡ����
    T_SCK=1;//��ʼ��ȡ����
    data<<=1;
    if(T_MISO)data|=0x01;
  }
  data>>=4;//������12λ����
  T_CS=1;//ȡ��ѡ��
  return data;
}
/*******************��β�����ƽ��ֵ*********************/
static u16 XPT2046_GetAvgXY(u8 cmd)
{
  u16 buff[10];
  u8 i,j;
  u32 temp;
  for(i=0;i<10;i++)
  {
    buff[i]=XPT2046_WRData(cmd);
  }
  /*ð������*/
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
  /*ȥ�����ֵ����Сֵ,��ƽ��ֵ*/
  temp=0;
  for(i=2;i<8;i++)
  {
    temp+=buff[i];
  }
  temp/=6;
  return temp;
}
/*************************��ȡxy����**********************
**
**���������ʽ:
** S  --��ʼλ������Ϊ1
**A2A1A0�� 001 --�ɼ�y����ֵ
**         101 --�ɼ�x����ֵ
**MODE ��0��ʾ12λ��1��ʾ8λ
**SER/DFR ��1Ϊ����ģʽ��0Ϊ���ģʽ
**PD1 PD0 ��00�����ڱ任֮�䴦�ڵ͹���ģʽ
**
**�ɼ�x����:1101 0000 --0xD0
**�ɼ�y����:1001 0000 --0x90
*********************************************************/
u8 XPT2046_ReadXY(void)
{
  if(T_PEN==0)//�ж��Ƿ���
  {
    xpt2046_info.x0=XPT2046_GetAvgXY(0xD0);//x����
    xpt2046_info.y0=XPT2046_GetAvgXY(0x90);//y����
    xpt2046_info.x=xpt2046_info.x0*xpt2046_info.xfac+xpt2046_info.xoff;  
    xpt2046_info.y=xpt2046_info.y0*xpt2046_info.yfac+xpt2046_info.yoff;
    return 1;
  }
  return 0;
}
/******************���������������ʵ������ƥ��(2��У׼)************
**
**���Ͻǣ�(x0,y0):3607,3851   ---ʵ������(0,0)
**���½�: (x0,y0):188,182     ---ʵ������(320,480)
**xʵ������:0~320  --- (3607-188)~(188-188)
**x�����б��:(3607-188)/320=10.684375
**xʵ������ת��:320-(�ɼ���ģ��ֵ-188)/10.684375
**
**yʵ������:0~480  --- (3851-182)~(182-182)
**y�����б��:(3851-182)/480=7.64375
**yʵ������ת��:480-(�ɼ���ģ��ֵ-182)/7.64375
**
************************************************************************/
/***************************4��У׼*************************************/
void Touch_Calibration(void)
{
  u8 cnt=0;
  u8 time=0;
  u16 count=0;
  u16 x1,y1,x2,y3;
  AT24C08_ReadData(TOUCH_CALIBRATION_ADDR,(u8 *)&xpt2046_info,sizeof(xpt2046_info));
  if(KEY1)//���°������봥����У׼
  {
    xpt2046_info.flag=0;
  }
  if(xpt2046_info.flag==0xab)
  {
    printf("У׼���\r\n");
    return ;
  }
  LCD_Clear(WHITE);
	LCD_Display_Str2(LCD_WIDTH/2-80,240,16,(u8 *)"�밴��ʾ���д���У׼��",RED,WHITE);
  //���Ƶ�һ����
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
          x1=XPT2046_GetAvgXY(0xD0);//x����
          y1=XPT2046_GetAvgXY(0x90);//y����
          while(!T_PEN){}//�ȴ��ɿ�
          LCD_DrawLine(10,20,10+20,20,WHITE);
          LCD_DrawLine(20,10,20,10+20,WHITE);  
          /*���Ƶڶ�����*/
          LCD_DrawLine(LCD_WIDTH-30,20,LCD_WIDTH-10,20,RED); 
          LCD_DrawLine(LCD_WIDTH-20,10,LCD_WIDTH-20,10+20,RED);
					LCD_Refresh();
          break;
        case 2:
          x2=XPT2046_GetAvgXY(0xD0);//x����
          while(!T_PEN){}//�ȴ��ɿ�
          LCD_DrawLine(LCD_WIDTH-30,20,LCD_WIDTH-10,20,WHITE); 
          LCD_DrawLine(LCD_WIDTH-20,10,LCD_WIDTH-20,10+20,WHITE);
          /*���Ƶ�������*/
          LCD_DrawLine(10,LCD_HIGHT-20,10+20,LCD_HIGHT-20,RED);
          LCD_DrawLine(20,LCD_HIGHT-30,20,LCD_HIGHT-10,RED); 
					LCD_Refresh();
          break;
        case 3:
          y3=XPT2046_GetAvgXY(0x90);//y����
          while(!T_PEN){}//�ȴ��ɿ�
          LCD_DrawLine(10,LCD_HIGHT-20,10+20,LCD_HIGHT-20,WHITE);
          LCD_DrawLine(20,LCD_HIGHT-30,20,LCD_HIGHT-10,WHITE); 
          /*���Ƶ�4����*/
          LCD_DrawLine(LCD_WIDTH-30,LCD_HIGHT-20,LCD_WIDTH-10,LCD_HIGHT-20,RED);
          LCD_DrawLine(LCD_WIDTH-20,LCD_HIGHT-30,LCD_WIDTH-20,LCD_HIGHT-10,RED);   
					LCD_Refresh();
          break;
        case 4:
          while(!T_PEN){}//�ȴ��ɿ�
          LCD_DrawLine(LCD_WIDTH-30,LCD_HIGHT-20,LCD_WIDTH-10,LCD_HIGHT-20,WHITE);
          LCD_DrawLine(LCD_WIDTH-20,LCD_HIGHT-30,LCD_WIDTH-20,LCD_HIGHT-10,WHITE);
					LCD_Display_Str2(LCD_WIDTH/2-80,240,16,(u8 *)"                        ",RED,WHITE);
					LCD_Display_Str2(LCD_WIDTH/2-32,230,16,(u8 *)"У׼���!",RED,WHITE);
					LCD_Display_Str2(LCD_WIDTH/2-48,250,16,(u8 *)"���ڱ�������",RED,WHITE);
					LCD_Refresh();
          /*����xy��б�ʺ�ƫ����*/
          xpt2046_info.xfac=(LCD_WIDTH-40)*1.0/(x2-x1);           //�õ�xfac  X�᷽���������	 
          xpt2046_info.xoff=(LCD_WIDTH-xpt2046_info.xfac*(x2+x1))/2.0; //�õ�xoff  X����ƫ����
          xpt2046_info.yfac=(LCD_HIGHT-40)*1.0/(y3-y1);           //�õ�yfac   Y�᷽���������
          xpt2046_info.yoff=(LCD_HIGHT-xpt2046_info.yfac*(y3+y1))/2.0; //�õ�yoff   Y����ƫ����
          printf("X:%f,%f\r\n",xpt2046_info.xfac,xpt2046_info.xoff);
          printf("Y:%f,%f\r\n",xpt2046_info.yfac,xpt2046_info.yoff);
					Delay_Ms(1000);
          xpt2046_info.flag=0xab;//У׼���
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
				return ;//5sû��û�в������˳�У׼ģʽ
			}
    }
  }
}
