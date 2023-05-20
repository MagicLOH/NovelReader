#ifndef _NT35310_H
#define _NT35310_H
#include "sys.h"
#include "delay.h"
#define SRAM_GRAM 1 //1表示开启缓存
#define LCD_WR_DAT *((volatile u16 *)0x6c000800) //读写数据
#define LCD_WR_REG  *((volatile u16 *)0x6c000000)//读写命令

#define GBK_16_ADDR 1024*1024*1 //GBK_16字体写入位置
#define GBK_24_ADDR 1024*1024*2 //GBK_24字体写入位置
#define GBK_32_ADDR 1024*1024*4 //GBK_32字体写入位置
extern const unsigned char ascii_16_8[][8*16/8];
extern const unsigned char ascii_24_12[][12*24/8];
extern const unsigned char ascii_32_16[][16*32/8];
extern const unsigned char gImage_bmp_320_480[307200];
#define LCD_WIDTH 320
#define LCD_HIGHT 480
#define LCD_BL PBout(0)//背光脚


//16位的颜色值
#define WHITE         	 0xFFFF //白色
#define BLACK         	 0x0000	//黑色  
#define BLUE         	   0x001F //蓝色 
#define RED           	 0xF800 //红色
#define YELLOW        	 0xFFE0 //黄色
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色

void LCD_Clear(u16 c);//清屏函数
void LCD_Init(void);//LCD初始化
void LCD_DrawPoint(u16 x,u16 y,u16 c);//画点函数
u16 LCD_ReadPoint(u16 x,u16 y);//读点函数
void LCD_Display_Font(u16 x,u16 y,u8 size,const u8 *font,u16 c);//显示汉字
void LCD_DispalyBmp(u16 x,u16 y,u16 w,u16 h,const u8 *bmp);//显示图片
void LCD_Refresh(void);//更新显示
u16 LCD_Display_Str(u16 x,u16 y,u8 size,const u8 *str,u16 c);//显示字符串
void LCD_Display_Str2(u16 x,u16 y,u8 size,const u8 *str,u16 c,u16 background);
/*绘制图形相关函数*/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 c);//画直线
void OLED_DrawAngleLine(u32 x,u32 y,float du,u32 len,u32 w,u16 c);//画任意角度直线
void LcdFill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);//矩形填充
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 c);//绘制三角形
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 c);//三角形填充
void OLED_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 c);//绘制矩形
void gui_circle(int xc, int yc,u16 c,int r, int fill);//画圆
#endif
