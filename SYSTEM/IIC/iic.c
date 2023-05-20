#include "iic.h"
#if IIC_MODE
/***********IIC引脚初始化**********
**
**IIC_SCL  -- PB6 时钟线
**IIC_SDA  -- PB7数据线
**
**
***********************************/
void IIC_Init(void)
{
  RCC->APB2ENR|=1<<3;//PB
  GPIOB->CRL&=0x00FFFFFF;
  GPIOB->CRL|=0xff000000;//复用开漏输出模式
  /*配置IIC功能*/
  RCC->APB1ENR|=1<<21;//IIC1
  RCC->APB1RSTR|=1<<21;//IIC复位时钟
  RCC->APB1RSTR&=~(1<<21);//取消复位
  I2C1->CR1&=~(1<<1);//I2C模式
  I2C1->CR2|=30<<0;//I2C模块工作频率30MHZ
  I2C1->CCR|=1<<15;//快速模式
  /*IIC快速模式下工作频率为400KHZ,周期T=1/400KHZ=2.5us
  IIC模式工作频率:30MHZ,周期时间:T=1/30MHZ
  总周期:2.5us/(1/30MHZ)=75
  Tlow/Thigh = 2；
  CCR=75/3=25
  */
  I2C1->CCR|=25<<0;//设置时钟控制分频系数
  I2C1->CR1|=1<<0;//开启I2C1
}
/****************起始信号********************
**
**时钟为高电平时，数据线由高变低。
**
***********************************************/
void IIC_Send_Start(void)
{
  I2C1->CR1|=1<<8;//产生起始信号
  while(!(I2C1->SR1&1<<0));//等待起始信号产生成功
}
/**************停止信号************************
**
**时钟为高电平时，数据线由低变高。
**
************************************************/
void IIC_Send_Stop(void)
{
  I2C1->CR1|=1<<9;//产生停止信号 
}
/********************发送地址*********************
**
**读取SR1寄存器后，对SR2寄存器的读操作将清除该位
**
****************************************************/
void IIC_Send_Addr(u8 addr)
{
  u8 c;
  c=c;
  I2C1->DR=addr;
  while(!(I2C1->SR1&1<<1));//等待地址发送完成
  c=I2C1->SR2;
}
/******************发送一个字节数据****************
**
**写数据到DR寄存器可清除该位
**
*****************************************************/
inline void IIC_Send_OneByte(u8 data)
{
  I2C1->DR=data;
  while(!(I2C1->SR1&1<<7));//等待数据发送完成
}
/**************************读取一个字节数据***************
**
**对数据寄存器的读写操作清除该位
**
***********************************************************/
inline u8 IIC_Read_OneByte(void)
{
  u8 data;
  I2C1->CR1|=1<<10;//应答
  while(!(I2C1->SR1&1<<6));
  data=I2C1->DR;
  I2C1->CR1&=~(1<<10);//非应答
  return data;
}
#else
/***********IIC引脚初始化**********
**
**IIC_SCL  -- PB6 时钟线
**IIC_SDA  -- PB7数据线
**
**
***********************************/
void IIC_Init(void)
{
  RCC->APB2ENR|=1<<3;//PB
  GPIOB->CRL&=0x00FFFFFF;
  GPIOB->CRL|=0x77000000;
  GPIOB->ODR|=0x3<<6;//上拉
}
/****************起始信号********************
**
**时钟为高电平时，数据线由高变低。
**
***********************************************/
void IIC_Send_Start(void)
{
  IIC_SDA_OUT_MODE();//SDA为输出模式
  IIC_SDA_OUT=1;//时钟和数据同为高
  IIC_SCL_OUT=1;
  Delay_Us(1);
  IIC_SDA_OUT=0;
  
  IIC_SCL_OUT=0;//拉低时钟线，方便读写数据
}
/**************停止信号************************
**
**时钟为高电平时，数据线由低变高。
**
************************************************/
void IIC_Send_Stop(void)
{
  IIC_SDA_OUT_MODE();//SDA为输出模式
  IIC_SDA_OUT=0;
  IIC_SCL_OUT=0;
  Delay_Us(1);
  IIC_SCL_OUT=1;
  Delay_Us(1);
  IIC_SDA_OUT=1;
}
/*********获取应答信号***************
**
**单片器获取应答信号
**返回值:0表示应答，1表示非应答
***************************************/
u8 IIC_GetAck(void)
{
  u8 i=0;
  IIC_SDA_IN_MODE();//SDA为输入模式
  IIC_SCL_OUT=0;//告诉从机，主机需要读取数据
  Delay_Us(1);
  IIC_SCL_OUT=1;//开始读取数据
  while(IIC_SDA_IN)
  {
    i++;
    if(i>100)return 1;//非应答
  }
  Delay_Us(1);
  IIC_SCL_OUT=0;//方便下一次读写数据
  return 0;//应答
}
/****************发送应答*******************
**
**形参:u8 ack -- 0表示发送应答，1表示发送非应答
**
********************************************/
void IIC_SendAck(u8 ack)
{
  IIC_SDA_OUT_MODE();//SDA为输出模式
  IIC_SCL_OUT=0;//告诉从机，主机开始发送数据
  IIC_SDA_OUT=ack&0x1;
  IIC_SCL_OUT=1;//数据发送完,从机开始读取数据
  Delay_Us(1);//等待从机读取数据
  IIC_SCL_OUT=0;//方便下一次读写数据
}
/******************发送一个字节数据*****************/
void IIC_Send_OneByte(u8 data)
{
  u8 i=0;
  IIC_SDA_OUT_MODE();//SDA为输出模式
  for(i=0;i<8;i++)
  {
    IIC_SCL_OUT=0;//告诉从机，主机开始发送数据
    if(data&0x80)IIC_SDA_OUT=1;
    else IIC_SDA_OUT=0;
    IIC_SCL_OUT=1;//数据发送完成
    Delay_Us(1);//等待从机读完数据
    data<<=1;//继续发送下一位数据
  }
  IIC_SCL_OUT=0;//方便下一次读写数据
}
/**************************读取一个字节数据****************/
u8 IIC_Read_OneByte(void)
{
  u8 data=0;
  u8 i=0;
  IIC_SDA_IN_MODE();//SDA为输入模式
  for(i=0;i<8;i++)
  {
    IIC_SCL_OUT=0;//告诉从机，主机需要读取数据
    Delay_Us(1);//等待从机数据发送完成
    IIC_SCL_OUT=1;//开始读取数据
    data<<=1;
    if(IIC_SDA_IN)data|=0X01;
  }
  Delay_Us(1);
  IIC_SCL_OUT=0;//方便下一次读写数据
  return data;
}
#endif



