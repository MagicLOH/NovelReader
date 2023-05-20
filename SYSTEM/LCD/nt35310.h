#ifndef _NT35310_H
#define _NT35310_H
#include "sys.h"
#include "delay.h"
#define SRAM_GRAM 1 //1��ʾ��������
#define LCD_WR_DAT *((volatile u16 *)0x6c000800) //��д����
#define LCD_WR_REG  *((volatile u16 *)0x6c000000)//��д����

#define GBK_16_ADDR 1024*1024*1 //GBK_16����д��λ��
#define GBK_24_ADDR 1024*1024*2 //GBK_24����д��λ��
#define GBK_32_ADDR 1024*1024*4 //GBK_32����д��λ��
extern const unsigned char ascii_16_8[][8*16/8];
extern const unsigned char ascii_24_12[][12*24/8];
extern const unsigned char ascii_32_16[][16*32/8];
extern const unsigned char gImage_bmp_320_480[307200];
#define LCD_WIDTH 320
#define LCD_HIGHT 480
#define LCD_BL PBout(0)//�����


//16λ����ɫֵ
#define WHITE         	 0xFFFF //��ɫ
#define BLACK         	 0x0000	//��ɫ  
#define BLUE         	   0x001F //��ɫ 
#define RED           	 0xF800 //��ɫ
#define YELLOW        	 0xFFE0 //��ɫ
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ

void LCD_Clear(u16 c);//��������
void LCD_Init(void);//LCD��ʼ��
void LCD_DrawPoint(u16 x,u16 y,u16 c);//���㺯��
u16 LCD_ReadPoint(u16 x,u16 y);//���㺯��
void LCD_Display_Font(u16 x,u16 y,u8 size,const u8 *font,u16 c);//��ʾ����
void LCD_DispalyBmp(u16 x,u16 y,u16 w,u16 h,const u8 *bmp);//��ʾͼƬ
void LCD_Refresh(void);//������ʾ
u16 LCD_Display_Str(u16 x,u16 y,u8 size,const u8 *str,u16 c);//��ʾ�ַ���
void LCD_Display_Str2(u16 x,u16 y,u8 size,const u8 *str,u16 c,u16 background);
/*����ͼ����غ���*/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 c);//��ֱ��
void OLED_DrawAngleLine(u32 x,u32 y,float du,u32 len,u32 w,u16 c);//������Ƕ�ֱ��
void LcdFill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);//�������
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 c);//����������
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 c);//���������
void OLED_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 c);//���ƾ���
void gui_circle(int xc, int yc,u16 c,int r, int fill);//��Բ
#endif
