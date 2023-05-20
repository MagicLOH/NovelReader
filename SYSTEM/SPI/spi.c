#include "spi.h"
/*******SPI引脚初始化***********
**
**SPI2_SCK --PB13时钟
**SPI2_MISO --PB14主机输入
**SPI2_MOSI --PB15主机输出 
**
*********************************/
void SPI2_Init(void)
{
  #if SPI_MODE
    RCC->APB2ENR|=1<<3;//PB
    GPIOB->CRH&=0x000FFFFF;
    GPIOB->CRH|=0xB8B00000;
    RCC->APB1ENR|=1<<14;//SPI2
    RCC->APB1RSTR|=1<<14;//开启复位时钟
    RCC->APB1RSTR&=~(1<<14);//取消复位
    SPI2->CR1|=1<<9;//软件管理片选
    SPI2->CR1|=1<<8;//片选空闲电平为高电平
//    SPI2->CR1&=~(0x7<<3);//工作频率36/2=18MHZ
    SPI2->CR1|=1<<2;//主模式
//    SPI2->CR1&=~(1<<1);//空闲电平为低电平
//    SPI2->CR1&=~(1<<0);//上升沿采样数据
    SPI2->CR1|=1<<6;//开启SPI2
  
  #else
    RCC->APB2ENR|=1<<3;//PB
    GPIOB->CRH&=0x000FFFFF;
    GPIOB->CRH|=0x38300000;
    GPIOB->ODR|=1<<13;//时钟线空闲电平为高电平
  #endif
}
/*************SPI2读写一个字节************/
u8 SPI2_WR_OneByte(u8 dat_w)
{
  #if SPI_MODE 
     SPI2->DR=dat_w;
     while(!(SPI2->SR&1<<1));//等待数据发送完成
     while(!(SPI2->SR&1<<0));//等待数据到来
     return SPI2->DR;
  #else
    u8 i=0;
    u8 dat_r=0;
    for(i=0;i<8;i++)
    {
      SPI2_SCK=0;//时钟线拉低，告诉从机，主机开始发送数据
      SPI2_MOSI=dat_w&0x80?1:0;
  //    if(dat_w&0x80)SPI2_MOSI=1;
  //    else SPI2_MOSI=0;
      SPI2_SCK=1;//数据发送完成
      dat_w<<=1;//继续发送下一位数据
      //读取数据
      dat_r<<=1;
      if(SPI2_MISO)dat_r|=0x1;//读到数据1
    }
    SPI2_SCK=1;//时钟线恢复为空闲电平
  return dat_r;
  #endif
}
