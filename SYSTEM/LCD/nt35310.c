#include "nt35310.h"
#include "w25q64.h"
#include "ff.h"
#include <stdlib.h>
/*******************LCDӲ����ʼ��*******************
**Ӳ���ӿ�:
**LCD_CS --PG12 Ƭѡ
**RS   -- PG0 ��������ѡ���
**WR -- PD5 дʹ��
**RD -- PD4 ��ʹ��
**LCD_BL -- PB0 �����
**D0~D1 --PD14~PD15
**D2~D3 -- PD0~PD1
**D4~D12 --PE7~PE15
**D13~D14 --PD8~PD9
**D15  -- PD10
**
*****************************************************/
static void LCD_GPIO_Init(void)
{
  RCC->APB2ENR|=1<<3;//PB
  RCC->APB2ENR|=1<<5;//PD
  RCC->APB2ENR|=1<<6;//PE
  RCC->APB2ENR|=1<<8;//PG
  GPIOG->CRH&=0xFFF0FFFF;
  GPIOG->CRH|=0x000B0000;//CS
  GPIOG->CRL&=0xfffffff0;
  GPIOG->CRL|=0x0000000B;//RS
  GPIOD->CRL&=0xFF00FFFF;
  GPIOD->CRL|=0x00BB0000;//WR��RD
  GPIOB->CRL&=~(0xf<<0);
  GPIOB->CRL|=0x3<<0;//BL
  
  GPIOD->CRH&=0x00fff000;
  GPIOD->CRH|=0xBB000BBB;//D0~D1,D13~D15
  GPIOD->CRL&=0xFFFFFF00;
  GPIOD->CRL|=0x000000BB;//D2~D3
  GPIOE->CRL&=0x0FFFFFFF;
  GPIOE->CRL|=0xB0000000;//D4
  GPIOE->CRH&=0x00000000;
  GPIOE->CRH|=0xBBBBBBBB;//D5~D12
  /*2.����FSMC*/
  RCC->AHBENR|=1<<8;//FSMC�ӿ�
  FSMC_Bank1->BTCR[6]=0;//BCR4
  FSMC_Bank1->BTCR[7]=0;//BTR4
  FSMC_Bank1->BTCR[6]&=~(1<<19);//�첽ͨ��
  FSMC_Bank1->BTCR[6]&=~(1<<14);//��дʹ����ͬʱ��
  FSMC_Bank1->BTCR[6]|=1<<12;//дʹ��
  FSMC_Bank1->BTCR[6]|=1<<4;//16Ϊ���ݿ��
  FSMC_Bank1->BTCR[7]|=1<<8;//���ݱ���ʱ��
  FSMC_Bank1->BTCR[7]&=~(0xf<<4);//��ַ����ʱ�䣬1��ʱ������
  FSMC_Bank1->BTCR[7]&=~(0xf<<0);//��ַ����ʱ��
  FSMC_Bank1->BTCR[6]|=1<<0;//ʹ��FSMC
}
/*****************д�Ĵ���*************/
static void LcdWriteReg(u16 reg)
{
  LCD_WR_REG=reg;
}
/**************д����***********************/
static void LcdWriteData(u16 dat)
{
  LCD_WR_DAT=dat;
}
#if !SRAM_GRAM
/****************������**************/
static u16 LcdReadData(void)
{
  u16 data;
  data=LCD_WR_DAT;
  return data;
}
#endif
#if SRAM_GRAM 
  static u16 lcd_gram[480][320];//������Ļ��С��������320*480*2
#endif
/********************���ù��*******************/
#if !SRAM_GRAM
static void LCD_SetCursor(u16 x,u16 y)
{
  LcdWriteReg(0x2A);//����x����
  LcdWriteData((x>>8)&0xff);//x�����8λ
  LcdWriteData(x&0xff);//x�����8λ
  LcdWriteReg(0x2B);//����Y����
  LcdWriteData((y>>8)&0xff);//Y�����8λ
  LcdWriteData(y&0xff);//Y�����8λ
}
#endif
/*******************��������********************/
void LCD_Clear(u16 c)
{
  #if !SRAM_GRAM
    u32 i=0;
    LCD_SetCursor(0,0);//���ù��λ��
    LcdWriteReg(0x2C);//д������
    for(i=0;i<320*480;i++)LcdWriteData(c);//д����ɫ����
  #else
    u32 i,j;
    for(i=0;i<480;i++)
    {
      for(j=0;j<320;j++)
      {
        lcd_gram[i][j]=c;
      }
    }
  #endif
  
}
/*******************���㺯��***********************/
void LCD_DrawPoint(u16 x,u16 y,u16 c)
{
  #if !SRAM_GRAM
    LCD_SetCursor(x,y);
    LcdWriteReg(0x2C);//д������
    LcdWriteData(c);
  #else
    lcd_gram[y][x]=c;//д�����ݵ�������
  #endif
}
/*******************���㺯��*************************/
u16 LCD_ReadPoint(u16 x,u16 y)
{
  #if !SRAM_GRAM
    u16 data;
    u8 r,b,g;
    u16 rgb=0;
    LCD_SetCursor(x,y);
    LcdWriteReg(0x2E);//����ɫֵ
    LcdReadData();//��һ��Ϊ��Ч����
    data=LcdReadData();
    r=data>>11;
    g=data>>2;
    data=LcdReadData();
    b=data>>11;
    rgb=(r<<11)|(g<<5)|(b&0x1f);
    return rgb;
  #else 
    u16 data;
    data=lcd_gram[y][x];
    return data;
  #endif
  
}
/********************��ʾͼƬ����*************************/
void LCD_DispalyBmp(u16 x,u16 y,u16 w,u16 h,const u8 *bmp)
{
 #if !SRAM_GRAM
  u32 i=0;
  u16 rgb;
  LcdWriteReg(0x2A);//����x����
  LcdWriteData((x>>8)&0xff);//x��ʼ�����8λ
  LcdWriteData(x&0xff);//x��ʼ�����8λ
  LcdWriteData(((x+w)>>8)&0xff);//x���������8λ
  LcdWriteData((x+w)&0xff);//x���������8λ

  LcdWriteReg(0x2B);//����Y����
  LcdWriteData((y>>8)&0xff);//Y�����8λ
  LcdWriteData(y&0xff);//Y�����8λ  
  LcdWriteData(((y+h)>>8)&0xff);//Y�����8λ
  LcdWriteData((y+h)&0xff);//Y�����8λ
  LcdWriteReg(0x2C);//д������  
  for(i=0;i<w*h*2;i+=2)
  {
    rgb=bmp[i]<<8|bmp[i+1];//��8λ��ɫ��ϳ�16λ
    LcdWriteData(rgb);//Y�����8λ
  }
  /*���½���������Ϊ������Ļ��С*/
  LcdWriteReg(0x2A);//����x����
  LcdWriteData(0);//x��ʼ�����8λ
  LcdWriteData(0);//x��ʼ�����8λ
  LcdWriteData((320>>8)&0xff);//x���������8λ
  LcdWriteData(320&0xff);//x���������8λ

  LcdWriteReg(0x2B);//����Y����
  LcdWriteData(0);//Y�����8λ
  LcdWriteData(0);//Y�����8λ  
  LcdWriteData((480>>8)&0xff);//Y�����8λ
  LcdWriteData(480&0xff);//Y�����8λ  
  #else
  u32 i,j;
  u16 data;
  for(i=0;i<h;i++)
  {
    for(j=0;j<w;j++)
    {
      data=bmp[i*w*2+2*j]<<8|bmp[i*w*2+2*j+1];
      lcd_gram[y+i][x+j]=data;
    }
  }
  #endif
}
#if SRAM_GRAM
/*********************������ʾ****************************/
void LCD_Refresh(void)
{
  u32 i,j;
  LcdWriteReg(0x2A);//����x����
  LcdWriteData(0);//x��ʼ�����8λ
  LcdWriteData(0);//x��ʼ�����8λ

  LcdWriteReg(0x2B);//����Y����
  LcdWriteData(0);//Y�����8λ
  LcdWriteData(0);//Y�����8λ  
  LcdWriteReg(0x2C);//д������ 
  for(i=0;i<480;i++)
  {
    for(j=0;j<320;j++)LcdWriteData(lcd_gram[i][j]);
  }
}
#endif
/*********************��ʾ����***************************
**
**�β�:u16 x,u16 y  --��ʾλ��
**     size --�����С
**     u8 *font  --Ҫ��ʾ�ĺ���
**     u16 c  --��ɫֵ
**����ʽ����λ��ǰ
**********************************************************/
void LCD_Display_Font(u16 x,u16 y,u8 size,const u8 *font,u16 c)
{
  u8 H,L;
  u32 addr;
  u16 csize=size*size/8;//���ֵ����С
  H=*font;
  L=*(font+1);
  if(L<0x7F)L-=0x40;
  else L-=0x41;
  H-=0x81;
  addr=(190*H+L)*csize;
  u8 *buff=NULL;
  buff=malloc(csize);
  if(buff==NULL)return ;
  if(size==16)W25Q64_ReadData(GBK_16_ADDR+addr,buff,csize);//��ȡ����
  else if(size==24)W25Q64_ReadData(GBK_24_ADDR+addr,buff,csize);//��ȡ����
	else if(size==32)W25Q64_ReadData(GBK_32_ADDR+addr,buff,csize);//��ȡ����
  u16 i=0,j=0;
  u8 data=0;
  u16 x0=x;
//  u16 rgb;
  for(i=0;i<size*size/8;i++)//�����ֽ���
  {
    data=buff[i];
    for(j=0;j<8;j++)
    {
      if(data&0x80)LCD_DrawPoint(x0,y,c);
//      else 
//      {
//        #if SRAM_GRAM
//          rgb=gImage_bmp_320_480[y*320*2+2*x0]<<8|gImage_bmp_320_480[y*320*2+2*x0+1];
//        #else
//          rgb=WHITE;
//        #endif
//        //LCD_DrawPoint(x0,y,rgb);
//      }
      data<<=1;//������ʾ��һλ����
      x0++;
    }
    if(x0-x==size)
    {
      x0=x;
      y++;
    }
  }
  free(buff);//�ͷſռ�
}
/******************************��ʾ�ַ�*************************
**
**�β�:u16 x,u16 y  --��ʾλ��
**     u8 w,u8 h --�ַ���Ⱥ͸߶�
**     cha  --Ҫ��ʾ���ַ�
**     u16 c  --��ɫֵ
**����ʽ����λ��ǰ
*****************************************************************/
static void LCD_DispalyCha(u16 x,u16 y,u8 w,u8 h,const u8 cha,u16 c,u16 background)
{
  u16 i=0;
  u8 j=0;
  u8 data;
  u16 y0=y;
//  u16 rgb;
  for(i=0;i<w*h/8;i++)//�ַ��������
  {
    if(h==16)data=ascii_16_8[cha-' '][i];
    else if(h==24)data=ascii_24_12[cha-' '][i];
		else if(h==32)data=ascii_32_16[cha-' '][i];
    for(j=0;j<8;j++)
    {
      if(data&0x80)LCD_DrawPoint(x,y0,c);
      else 
      {
//        #if SRAM_GRAM
//          rgb=gImage_bmp_320_480[y0*320*2+2*x]<<8|gImage_bmp_320_480[y0*320*2+2*x+1];
//        #else
//          rgb=WHITE;
//        #endif
        LCD_DrawPoint(x,y0,background);
      }
      data<<=1;
      y0++;
    }
    if(y0-y==h)
    {
      y0=y;
      x++;
    }
  }
}
/*************************��ʾ�ַ���****************************
**
**�β�:u16 x,u16 y  --��ʾλ��
**     size --�����С
**     u8 *font  --���ֵ�����Ϣ
**     u16 c  --��ɫֵ
**����ʽ����λ��ǰ
**�ֿ���뷽ʽ:GBK,����ռ�����ֽڣ�ASCIIռһ���ֽ�
*****************************************************************/
u16 LCD_Display_Str(u16 x,u16 y,u8 size,const u8 *str,u16 c)
{
		u16 font_number=0;
    u16 x0=x;
    while(*str)
    {
      if(*str>0x80)//����
      {
        if(LCD_WIDTH-x0<(size-2))//�жϵ�ǰʣ���Ƿ�����ʾ��
        {
          x0=0;
          y+=size;
					if(410-y<size || y>=410)//�ж��Ƿ�д����Ļ
					{				
						return font_number;
					}
        }
				if(*(str+1)=='\0')
				{
					str++;
					font_number++;
					font_number|=0x8000;//�����λ��1,��ʾ��ǰ����ֻ������λ�ֽ�
				}
				else
				{
					LCD_Display_Font(x0,y,size,str,c);
					x0+=size;
					str+=2;
					font_number+=2;
				}
      }
      else if(*str>=' ' && *str<='~')
      {
        if(LCD_WIDTH-x0<(size/2-1))//�жϵ�ǰʣ���Ƿ�����ʾ��
        {
          x0=0;
          y+=size;
					if(410-y<size || y>=410)//�ж��Ƿ�д����Ļ
					{				
						return font_number;
					}
        }
        LCD_DispalyCha(x0,y,size/2,size,*str,c,WHITE);
        x0+=size/2;
        str++;
				font_number++;
      }
			else if(*str=='\n')
			{
				str++;
				x0=0;
        y+=size;
				if(410-y<size || y>=410)//�ж��Ƿ�д����Ļ
				{				
					return font_number;
				}
				font_number++;
			}
			else 			
			{
				str++;
				font_number++;
			}
    }
		return font_number;
}
void LCD_Display_Str2(u16 x,u16 y,u8 size,const u8 *str,u16 c,u16 background)
{
    u16 x0=x;
    while(*str)
    {
      if(*str>0x80)//����
      {
        if(LCD_WIDTH-x0<(size-2))//�жϵ�ǰʣ���Ƿ�����ʾ��
        {
          x0=0;
          y+=size;
        }
        LCD_Display_Font(x0,y,size,str,c);
        x0+=size;
        str+=2;
      }
      else if(*str>=' ' && *str<='~')
      {
        if(LCD_WIDTH-x0<(size/2-1))//�жϵ�ǰʣ���Ƿ�����ʾ��
        {
          x0=0;
          y+=size;
        }
        LCD_DispalyCha(x0,y,size/2,size,*str,c,background);
        x0+=size/2;
        str++;
      }
			else if(*str=='\n')
			{
				str++;
				x0=0;
        y+=size;
			}
			else 			
			{
				str++;
			}
    }
}
/*
�������ܣ���ֱ��
��    ����
x1,y1:�������
x2,y2:�յ����� 
*/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 c)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint	(uRow,uCol,c);//����
	
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}  

/*
�������ܣ�����ǶȻ�ֱ�� 
��    ����
	x,y:����
	du :����
	len:�뾶
	w  :�߶εĳ���
	c  :��ɫֵ 0����1
����:OLED_DrawAngleLine(60,30,45,20,20,1);//�ǶȻ���
*/
#include <math.h>
void OLED_DrawAngleLine(u32 x,u32 y,float du,u32 len,u32 w,u16 c)
{
	int i;
	int x0,y0;
	float k=du*(3.1415926535/180);	
	for(i=len-w;i<len;i++)
	{
		x0=cos(k)*i;
		y0=sin(k)*i;
		LCD_DrawPoint(x+x0,y+y0,c);
		//gui_circle(x+x0, y+y0,c,2, 1);
	}	
}
/*
�������ܣ�������	
��    ����(x1,y1),(x2,y2):���εĶԽ�����
*/
void OLED_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 c)
{
	LCD_DrawLine(x1,y1,x2,y1,c);
	LCD_DrawLine(x1,y1,x1,y2,c);
	LCD_DrawLine(x1,y2,x2,y2,c);
	LCD_DrawLine(x2,y1,x2,y2,c);
}

/*****************************************************************************
 * @name       :void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
 * @function   :8�Գ�Բ�����㷨���ڲ����ã�
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								x:the x coordinate relative to the Circular center 
								y:the y coordinate relative to the Circular center 
								c:the color value of the circle
 * @retvalue   :None
******************************************************************************/  
static void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
{
	LCD_DrawPoint(xc + x, yc + y, c);

	LCD_DrawPoint(xc - x, yc + y, c);

	LCD_DrawPoint(xc + x, yc - y, c);

	LCD_DrawPoint(xc - x, yc - y, c);

	LCD_DrawPoint(xc + y, yc + x, c);

	LCD_DrawPoint(xc - y, yc + x, c);

	LCD_DrawPoint(xc + y, yc - x, c);

	LCD_DrawPoint(xc - y, yc - x, c);
}
/*****************************************************************************

**˵��:��ָ��λ�û���ָ����С��Բ
**������int xc, int yc --Բ������
**			u16 c --��ɫֵ
**			int r --�뾶
**			int fill  --1��䣬0�����
******************************************************************************/  
void gui_circle(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// �����䣨��ʵ��Բ��
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} else 
	{
		// �������䣨������Բ��
		while (x <= y) {
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}
/*
�������ܣ�������ɫ���
��    ����(sx,sy),(ex,ey):���ζԽ�����
					color:Ҫ������ɫ
*/
void LcdFill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{    
	
	u16 i,j; 
	for(i=sy;i<ey;i++)
	{ 
		for(j=sx;j<ex;j++)lcd_gram[i][j]=color;  
	}	 
	
	
} 
/********************************��������************************
**������u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2 --- ����������������
**			u16 c  ---��ɫֵ
**
******************************************************************/
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 c)
{
	LCD_DrawLine(x0,y0,x1,y1,c);
	LCD_DrawLine(x1,y1,x2,y2,c);
	LCD_DrawLine(x2,y2,x0,y0,c);
}
static void _swap(u16 *a, u16 *b)
{
	u16 tmp;
  tmp = *a;
	*a = *b;
	*b = tmp;
}
/**************************************���������*************************
**������u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2 --- ����������������
**			u16 c  ---��ɫֵ
**
*************************************************************************/
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 c)
{
	u16 a, b, y, last;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	long sa = 0;
	long sb = 0;
 	if (y0 > y1) 
	{
    _swap(&y0,&y1); 
		_swap(&x0,&x1);
 	}
 	if (y1 > y2) 
	{
    _swap(&y2,&y1); 
		_swap(&x2,&x1);
 	}
  if (y0 > y1) 
	{
    _swap(&y0,&y1); 
		_swap(&x0,&x1);
  }
	if(y0 == y2) 
	{ 
		a = b = x0;
		if(x1 < a)
    {
			a = x1;
    }
    else if(x1 > b)
    {
			b = x1;
    }
    if(x2 < a)
    {
			a = x2;
    }
		else if(x2 > b)
    {
			b = x2;
    }
		LcdFill(a,y0,b,y0,c);
    return;
	}
	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;
	
	if(y1 == y2)
	{
		last = y1; 
	}
  else
	{
		last = y1-1; 
	}
	for(y=y0; y<=last; y++) 
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
    sb += dx02;
    if(a > b)
    {
			_swap(&a,&b);
		}
		LcdFill(a,y,b,y,c);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) 
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(a > b)
		{
			_swap(&a,&b);
		}
		LcdFill(a,y,b,y,c);
	}
}
/*******************LCD��ʼ��*******************/
void LCD_Init(void)
{
  LCD_GPIO_Init();//GPIO��ʼ��
  Delay_Ms(100);
  LcdWriteReg(0xED);
  LcdWriteData(0x01);
  LcdWriteData(0xFE);
  LcdWriteReg(0xEE);
  LcdWriteData(0xDE);
  LcdWriteData(0x21);
  LcdWriteReg(0xF1);
  LcdWriteData(0x01);
  LcdWriteReg(0xDF);
  LcdWriteData(0x10);
  //VCOMvoltage//
  LcdWriteReg(0xC4);
  LcdWriteData(0x8F); //5f
  LcdWriteReg(0xC6);
  LcdWriteData(0x00);
  LcdWriteData(0xE2);
  LcdWriteData(0xE2);
  LcdWriteData(0xE2);
  LcdWriteReg(0xBF);
  LcdWriteData(0xAA);
  LcdWriteReg(0xB0);
  LcdWriteData(0x0D);
  LcdWriteData(0x00);
  LcdWriteData(0x0D);
  LcdWriteData(0x00);
  LcdWriteData(0x11);
  LcdWriteData(0x00);
  LcdWriteData(0x19);
  LcdWriteData(0x00);
  LcdWriteData(0x21);
  LcdWriteData(0x00);
  LcdWriteData(0x2D);
  LcdWriteData(0x00);
  LcdWriteData(0x3D);
  LcdWriteData(0x00);
  LcdWriteData(0x5D);
  LcdWriteData(0x00);
  LcdWriteData(0x5D);
  LcdWriteData(0x00);
  LcdWriteReg(0xB1);
  LcdWriteData(0x80);
  LcdWriteData(0x00);
  LcdWriteData(0x8B);
  LcdWriteData(0x00);
  LcdWriteData(0x96);
  LcdWriteData(0x00);
  LcdWriteReg(0xB2);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x02);
  LcdWriteData(0x00);
  LcdWriteData(0x03);
  LcdWriteData(0x00);
  LcdWriteReg(0xB3);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xB4);
  LcdWriteData(0x8B);
  LcdWriteData(0x00);
  LcdWriteData(0x96);
  LcdWriteData(0x00);
  LcdWriteData(0xA1);
  LcdWriteData(0x00);
  LcdWriteReg(0xB5);
  LcdWriteData(0x02);
  LcdWriteData(0x00);
  LcdWriteData(0x03);
  LcdWriteData(0x00);
  LcdWriteData(0x04);
  LcdWriteData(0x00);
  LcdWriteReg(0xB6);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xB7);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x3F);
  LcdWriteData(0x00);
  LcdWriteData(0x5E);
  LcdWriteData(0x00);
  LcdWriteData(0x64);
  LcdWriteData(0x00);
  LcdWriteData(0x8C);
  LcdWriteData(0x00);
  LcdWriteData(0xAC);
  LcdWriteData(0x00);
  LcdWriteData(0xDC);
  LcdWriteData(0x00);
  LcdWriteData(0x70);
  LcdWriteData(0x00);
  LcdWriteData(0x90);
  LcdWriteData(0x00);
  LcdWriteData(0xEB);
  LcdWriteData(0x00);
  LcdWriteData(0xDC);
  LcdWriteData(0x00);
  LcdWriteReg(0xB8);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xBA);
  LcdWriteData(0x24);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xC1);
  LcdWriteData(0x20);
  LcdWriteData(0x00);
  LcdWriteData(0x54);
  LcdWriteData(0x00);
  LcdWriteData(0xFF);
  LcdWriteData(0x00);
  LcdWriteReg(0xC2);
  LcdWriteData(0x0A);
  LcdWriteData(0x00);
  LcdWriteData(0x04);
  LcdWriteData(0x00);
  LcdWriteReg(0xC3);
  LcdWriteData(0x3C);
  LcdWriteData(0x00);
  LcdWriteData(0x3A);
  LcdWriteData(0x00);
  LcdWriteData(0x39);
  LcdWriteData(0x00);
  LcdWriteData(0x37);
  LcdWriteData(0x00);
  LcdWriteData(0x3C);
  LcdWriteData(0x00);
  LcdWriteData(0x36);
  LcdWriteData(0x00);
  LcdWriteData(0x32);
  LcdWriteData(0x00);
  LcdWriteData(0x2F);
  LcdWriteData(0x00);
  LcdWriteData(0x2C);
  LcdWriteData(0x00);
  LcdWriteData(0x29);
  LcdWriteData(0x00);
  LcdWriteData(0x26);
  LcdWriteData(0x00);
  LcdWriteData(0x24);
  LcdWriteData(0x00);
  LcdWriteData(0x24);
  LcdWriteData(0x00);
  LcdWriteData(0x23);
  LcdWriteData(0x00);
  LcdWriteData(0x3C);
  LcdWriteData(0x00);
  LcdWriteData(0x36);
  LcdWriteData(0x00);
  LcdWriteData(0x32);
  LcdWriteData(0x00);
  LcdWriteData(0x2F);
  LcdWriteData(0x00);
  LcdWriteData(0x2C);
  LcdWriteData(0x00);
  LcdWriteData(0x29);
  LcdWriteData(0x00);
  LcdWriteData(0x26);
  LcdWriteData(0x00);
  LcdWriteData(0x24);
  LcdWriteData(0x00);
  LcdWriteData(0x24);
  LcdWriteData(0x00);
  LcdWriteData(0x23);
  LcdWriteData(0x00);
  LcdWriteReg(0xC4);
  LcdWriteData(0x62);
  LcdWriteData(0x00);
  LcdWriteData(0x05);
  LcdWriteData(0x00);
  LcdWriteData(0x84);
  LcdWriteData(0x00);
  LcdWriteData(0xF0);
  LcdWriteData(0x00);
  LcdWriteData(0x18);
  LcdWriteData(0x00);
  LcdWriteData(0xA4);
  LcdWriteData(0x00);
  LcdWriteData(0x18);
  LcdWriteData(0x00);
  LcdWriteData(0x50);
  LcdWriteData(0x00);
  LcdWriteData(0x0C);
  LcdWriteData(0x00);
  LcdWriteData(0x17);
  LcdWriteData(0x00);
  LcdWriteData(0x95);
  LcdWriteData(0x00);
  LcdWriteData(0xF3);
  LcdWriteData(0x00);
  LcdWriteData(0xE6);
  LcdWriteData(0x00);
  LcdWriteReg(0xC5);
  LcdWriteData(0x32);
  LcdWriteData(0x00);
  LcdWriteData(0x44);
  LcdWriteData(0x00);
  LcdWriteData(0x65);
  LcdWriteData(0x00);
  LcdWriteData(0x76);
  LcdWriteData(0x00);
  LcdWriteData(0x88);
  LcdWriteData(0x00);
  LcdWriteReg(0xC6);
  LcdWriteData(0x20);
  LcdWriteData(0x00);
  LcdWriteData(0x17);
  LcdWriteData(0x00);
  LcdWriteData(0x01);
  LcdWriteData(0x00);
  LcdWriteReg(0xC7);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xC8);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xC9);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xE0);
  LcdWriteData(0x16);
  LcdWriteData(0x00);
  LcdWriteData(0x1C);
  LcdWriteData(0x00);
  LcdWriteData(0x21);
  LcdWriteData(0x00);
  LcdWriteData(0x36);
  LcdWriteData(0x00);
  LcdWriteData(0x46);
  LcdWriteData(0x00);
  LcdWriteData(0x52);
  LcdWriteData(0x00);
  LcdWriteData(0x64);
  LcdWriteData(0x00);
  LcdWriteData(0x7A);
  LcdWriteData(0x00);
  LcdWriteData(0x8B);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0xA8);
  LcdWriteData(0x00);
  LcdWriteData(0xB9);
  LcdWriteData(0x00);
  LcdWriteData(0xC4);
  LcdWriteData(0x00);
  LcdWriteData(0xCA);
  LcdWriteData(0x00);
  LcdWriteData(0xD2);
  LcdWriteData(0x00);
  LcdWriteData(0xD9);
  LcdWriteData(0x00);
  LcdWriteData(0xE0);
  LcdWriteData(0x00);
  LcdWriteData(0xF3);
  LcdWriteData(0x00);
  LcdWriteReg(0xE1);
  LcdWriteData(0x16);
  LcdWriteData(0x00);
  LcdWriteData(0x1C);
  LcdWriteData(0x00);
  LcdWriteData(0x22);
  LcdWriteData(0x00);
  LcdWriteData(0x36);
  LcdWriteData(0x00);
  LcdWriteData(0x45);
  LcdWriteData(0x00);
  LcdWriteData(0x52);
  LcdWriteData(0x00);
  LcdWriteData(0x64);
  LcdWriteData(0x00);
  LcdWriteData(0x7A);
  LcdWriteData(0x00);
  LcdWriteData(0x8B);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0xA8);
  LcdWriteData(0x00);
  LcdWriteData(0xB9);
  LcdWriteData(0x00);
  LcdWriteData(0xC4);
  LcdWriteData(0x00);
  LcdWriteData(0xCA);
  LcdWriteData(0x00);
  LcdWriteData(0xD2);
  LcdWriteData(0x00);
  LcdWriteData(0xD8);
  LcdWriteData(0x00);
  LcdWriteData(0xE0);
  LcdWriteData(0x00);
  LcdWriteData(0xF3);
  LcdWriteData(0x00);
  LcdWriteReg(0xE2);
  LcdWriteData(0x05);
  LcdWriteData(0x00);
  LcdWriteData(0x0B);
  LcdWriteData(0x00);
  LcdWriteData(0x1B);
  LcdWriteData(0x00);
  LcdWriteData(0x34);
  LcdWriteData(0x00);
  LcdWriteData(0x44);
  LcdWriteData(0x00);
  LcdWriteData(0x4F);
  LcdWriteData(0x00);
  LcdWriteData(0x61);
  LcdWriteData(0x00);
  LcdWriteData(0x79);
  LcdWriteData(0x00);
  LcdWriteData(0x88);
  LcdWriteData(0x00);
  LcdWriteData(0x97);
  LcdWriteData(0x00);
  LcdWriteData(0xA6);
  LcdWriteData(0x00);
  LcdWriteData(0xB7);
  LcdWriteData(0x00);
  LcdWriteData(0xC2);
  LcdWriteData(0x00);
  LcdWriteData(0xC7);
  LcdWriteData(0x00);
  LcdWriteData(0xD1);
  LcdWriteData(0x00);
  LcdWriteData(0xD6);
  LcdWriteData(0x00);
  LcdWriteData(0xDD);
  LcdWriteData(0x00);
  LcdWriteData(0xF3);
  LcdWriteData(0x00);
  LcdWriteReg(0xE3);
  LcdWriteData(0x05);
  LcdWriteData(0x00);
  LcdWriteData(0xA);
  LcdWriteData(0x00);
  LcdWriteData(0x1C);
  LcdWriteData(0x00);
  LcdWriteData(0x33);
  LcdWriteData(0x00);
  LcdWriteData(0x44);
  LcdWriteData(0x00);
  LcdWriteData(0x50);
  LcdWriteData(0x00);
  LcdWriteData(0x62);
  LcdWriteData(0x00);
  LcdWriteData(0x78);
  LcdWriteData(0x00);
  LcdWriteData(0x88);
  LcdWriteData(0x00);
  LcdWriteData(0x97);
  LcdWriteData(0x00);
  LcdWriteData(0xA6);
  LcdWriteData(0x00);
  LcdWriteData(0xB7);
  LcdWriteData(0x00);
  LcdWriteData(0xC2);
  LcdWriteData(0x00);
  LcdWriteData(0xC7);
  LcdWriteData(0x00);
  LcdWriteData(0xD1);
  LcdWriteData(0x00);
  LcdWriteData(0xD5);
  LcdWriteData(0x00);
  LcdWriteData(0xDD);
  LcdWriteData(0x00);
  LcdWriteData(0xF3);
  LcdWriteData(0x00);
  LcdWriteReg(0xE4);
  LcdWriteData(0x01);
  LcdWriteData(0x00);
  LcdWriteData(0x01);
  LcdWriteData(0x00);
  LcdWriteData(0x02);
  LcdWriteData(0x00);
  LcdWriteData(0x2A);
  LcdWriteData(0x00);
  LcdWriteData(0x3C);
  LcdWriteData(0x00);
  LcdWriteData(0x4B);
  LcdWriteData(0x00);
  LcdWriteData(0x5D);
  LcdWriteData(0x00);
  LcdWriteData(0x74);
  LcdWriteData(0x00);
  LcdWriteData(0x84);
  LcdWriteData(0x00);
  LcdWriteData(0x93);
  LcdWriteData(0x00);
  LcdWriteData(0xA2);
  LcdWriteData(0x00);
  LcdWriteData(0xB3);
  LcdWriteData(0x00);
  LcdWriteData(0xBE);
  LcdWriteData(0x00);
  LcdWriteData(0xC4);
  LcdWriteData(0x00);
  LcdWriteData(0xCD);
  LcdWriteData(0x00);
  LcdWriteData(0xD3);
  LcdWriteData(0x00);
  LcdWriteData(0xDD);
  LcdWriteData(0x00);
  LcdWriteData(0xF3);
  LcdWriteData(0x00);
  LcdWriteReg(0xE5);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x02);
  LcdWriteData(0x00);
  LcdWriteData(0x29);
  LcdWriteData(0x00);
  LcdWriteData(0x3C);
  LcdWriteData(0x00);
  LcdWriteData(0x4B);
  LcdWriteData(0x00);
  LcdWriteData(0x5D);
  LcdWriteData(0x00);
  LcdWriteData(0x74);
  LcdWriteData(0x00);
  LcdWriteData(0x84);
  LcdWriteData(0x00);
  LcdWriteData(0x93);
  LcdWriteData(0x00);
  LcdWriteData(0xA2);
  LcdWriteData(0x00);
  LcdWriteData(0xB3);
  LcdWriteData(0x00);
  LcdWriteData(0xBE);
  LcdWriteData(0x00);
  LcdWriteData(0xC4);
  LcdWriteData(0x00);
  LcdWriteData(0xCD);
  LcdWriteData(0x00);
  LcdWriteData(0xD3);
  LcdWriteData(0x00);
  LcdWriteData(0xDC);
  LcdWriteData(0x00);
  LcdWriteData(0xF3);
  LcdWriteData(0x00);
  LcdWriteReg(0xE6);
  LcdWriteData(0x11);
  LcdWriteData(0x00);
  LcdWriteData(0x34);
  LcdWriteData(0x00);
  LcdWriteData(0x56);
  LcdWriteData(0x00);
  LcdWriteData(0x76);
  LcdWriteData(0x00);
  LcdWriteData(0x77);
  LcdWriteData(0x00);
  LcdWriteData(0x66);
  LcdWriteData(0x00);
  LcdWriteData(0x88);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0xBB);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0x66);
  LcdWriteData(0x00);
  LcdWriteData(0x55);
  LcdWriteData(0x00);
  LcdWriteData(0x55);
  LcdWriteData(0x00);
  LcdWriteData(0x45);
  LcdWriteData(0x00);
  LcdWriteData(0x43);
  LcdWriteData(0x00);
  LcdWriteData(0x44);
  LcdWriteData(0x00);
  LcdWriteReg(0xE7);
  LcdWriteData(0x32);
  LcdWriteData(0x00);
  LcdWriteData(0x55);
  LcdWriteData(0x00);
  LcdWriteData(0x76);
  LcdWriteData(0x00);
  LcdWriteData(0x66);
  LcdWriteData(0x00);
  LcdWriteData(0x67);
  LcdWriteData(0x00);
  LcdWriteData(0x67);
  LcdWriteData(0x00);
  LcdWriteData(0x87);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0xBB);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0x77);
  LcdWriteData(0x00);
  LcdWriteData(0x44);
  LcdWriteData(0x00);
  LcdWriteData(0x56);
  LcdWriteData(0x00);
  LcdWriteData(0x23);
  LcdWriteData(0x00);
  LcdWriteData(0x33);
  LcdWriteData(0x00);
  LcdWriteData(0x45);
  LcdWriteData(0x00);
  LcdWriteReg(0xE8);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0x87);
  LcdWriteData(0x00);
  LcdWriteData(0x88);
  LcdWriteData(0x00);
  LcdWriteData(0x77);
  LcdWriteData(0x00);
  LcdWriteData(0x66);
  LcdWriteData(0x00);
  LcdWriteData(0x88);
  LcdWriteData(0x00);
  LcdWriteData(0xAA);
  LcdWriteData(0x00);
  LcdWriteData(0xBB);
  LcdWriteData(0x00);
  LcdWriteData(0x99);
  LcdWriteData(0x00);
  LcdWriteData(0x66);
  LcdWriteData(0x00);
  LcdWriteData(0x55);
  LcdWriteData(0x00);
  LcdWriteData(0x55);
  LcdWriteData(0x00);
  LcdWriteData(0x44);
  LcdWriteData(0x00);
  LcdWriteData(0x44);
  LcdWriteData(0x00);
  LcdWriteData(0x55);
  LcdWriteData(0x00);
  LcdWriteReg(0xE9);
  LcdWriteData(0xAA);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0x00);
  LcdWriteData(0xAA);
  LcdWriteReg(0xCF);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xF0);
  LcdWriteData(0x00);
  LcdWriteData(0x50);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteData(0x00);
  LcdWriteReg(0xF3);
  LcdWriteData(0x00);
  LcdWriteReg(0xF9);
  LcdWriteData(0x06);
  LcdWriteData(0x10);
  LcdWriteData(0x29);
  LcdWriteData(0x00);
  LcdWriteReg(0x3A);
  LcdWriteData(0x55);
  LcdWriteReg(0x11);
  Delay_Ms(100);
  LcdWriteReg(0x29);
  LcdWriteReg(0x35);
  LcdWriteData(0x00);
  LcdWriteReg(0x51);
  LcdWriteData(0xFF);
  LcdWriteReg(0x53);
  LcdWriteData(0x2C);
  LcdWriteReg(0x55);
  LcdWriteData(0x82);
  LcdWriteReg(0x2c); 
  LCD_Clear(WHITE); 
	LCD_Refresh();
  LCD_BL=1;//��������
}
