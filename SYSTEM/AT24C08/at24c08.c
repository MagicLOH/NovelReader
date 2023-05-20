#include "at24c08.h"
#if IIC_MODE 
/**********************AT24C08页写（实现跨阵列）***********
**AT24C08空间大小:1024字节
**由于地址写入为1个字节，所以需要a9a8选择存储阵列。
**a9a8=00 --操作地址为0~255
**a9a8=01 --操作地址为256~511
**a9a8=10 --操作地址为512~767
**a9a8=11 --操作地址为768~1023

**形参:u8 addr -- 写入位置
**     u8 *buff --要写入的数据首地址
**     u8 count --写入的字节数
**返回值:0 --成功，其他值--失败
**AT24C08一页大小:16字节
*********************************************************/
static u8 AT24C08_PagePragram(u16 addr,u8 *buff,u8 count)
{
  u8 i=0;
  u8 cnt=addr/256;//计算地址在哪个存储阵列中
  u8 addr_w=addr%256;//计算写入地址在存储阵列哪个位置
  IIC_Send_Start();//起始信号
  IIC_Send_Addr(IIC_ADDR_W|(cnt<<1));//发送从设备地址
  IIC_Send_OneByte(addr_w);//发送要写入的位置
  for(i=0;i<count;i++)
  {
     IIC_Send_OneByte(buff[i]);//发送要写入的位置

  }
  IIC_Send_Stop();//停止信号
  Delay_Ms(10);//等待数据写入成功
  return 0;  
}
/*****************连续读(跨阵列)*********************
**
**AT24C08空间大小:1024字节
**由于地址写入为1个字节，所以需要a9a8选择存储阵列。
**a9a8=00 --操作地址为0~255
**a9a8=01 --操作地址为256~511
**a9a8=10 --操作地址为512~767
**a9a8=11 --操作地址为768~1023

**形参:u8 addr -- 读取位置
**     u8 *buff --保存读取数据缓冲区
**     u8 count --读取的字节数
**返回值:0 --成功，其他值--失败
**
*******************************************************/
u8 AT24C08_ReadData(u16 addr,u8 *buff,u16 count)
{
  u16 i=0;
  u8 cnt=addr/256;//计算地址在哪个存储阵列中
  u8 addr_r=addr%256;//计算写入地址在存储阵列哪个位置
  IIC_Send_Start();//起始信号
  IIC_Send_Addr(IIC_ADDR_W|(cnt<<1));//发送从设备地址
  IIC_Send_OneByte(addr_r);//发送要读取的位置
  IIC_Send_Start();//起始信号
  IIC_Send_Addr(IIC_ADDR_R|(cnt<<1));//发送从设备地址
  for(i=0;i<count;i++)
  {
    buff[i]=IIC_Read_OneByte();//读一字节数据

  }
  IIC_Send_Stop();//停止信号
  return 0;
}
#else 
/**********************AT24C08页写（实现跨阵列）***********
**AT24C08空间大小:1024字节
**由于地址写入为1个字节，所以需要a9a8选择存储阵列。
**a9a8=00 --操作地址为0~255
**a9a8=01 --操作地址为256~511
**a9a8=10 --操作地址为512~767
**a9a8=11 --操作地址为768~1023

**形参:u8 addr -- 写入位置
**     u8 *buff --要写入的数据首地址
**     u8 count --写入的字节数
**返回值:0 --成功，其他值--失败
**AT24C08一页大小:16字节
*********************************************************/
static u8 AT24C08_PagePragram(u16 addr,u8 *buff,u8 count)
{
  u8 i=0;
  u8 cnt=addr/256;//计算地址在哪个存储阵列中
  u8 addr_w=addr%256;//计算写入地址在存储阵列哪个位置
  IIC_Send_Start();//起始信号
  IIC_Send_OneByte(IIC_ADDR_W|(cnt<<1));//发送从设备地址
  if(IIC_GetAck())return 1;//获取应答
  IIC_Send_OneByte(addr_w);//发送要写入的位置
  if(IIC_GetAck())return 2;//获取应答
  for(i=0;i<count;i++)
  {
     IIC_Send_OneByte(buff[i]);//发送要写入的位置
     if(IIC_GetAck())return 3;//获取应答
  }
  IIC_Send_Stop();//停止信号
  Delay_Ms(10);//等待数据写入成功
  return 0;  
}
/*****************连续读(跨阵列)*********************
**
**AT24C08空间大小:1024字节
**由于地址写入为1个字节，所以需要a9a8选择存储阵列。
**a9a8=00 --操作地址为0~255
**a9a8=01 --操作地址为256~511
**a9a8=10 --操作地址为512~767
**a9a8=11 --操作地址为768~1023

**形参:u8 addr -- 读取位置
**     u8 *buff --保存读取数据缓冲区
**     u8 count --读取的字节数
**返回值:0 --成功，其他值--失败
**
*******************************************************/
u8 AT24C08_ReadData(u16 addr,u8 *buff,u16 count)
{
  u16 i=0;
  u8 cnt=addr/256;//计算地址在哪个存储阵列中
  u8 addr_r=addr%256;//计算写入地址在存储阵列哪个位置
  IIC_Send_Start();//起始信号
  IIC_Send_OneByte(IIC_ADDR_W|(cnt<<1));//发送从设备地址
  if(IIC_GetAck())return 1;//获取应答
  IIC_Send_OneByte(addr_r);//发送要读取的位置
  if(IIC_GetAck())return 2;//获取应答
  IIC_Send_Start();//起始信号
  IIC_Send_OneByte(IIC_ADDR_R|(cnt<<1));//发送从设备地址
  if(IIC_GetAck())return 3;//获取应答
  for(i=0;i<count-1;i++)
  {
    buff[i]=IIC_Read_OneByte();//读一字节数据
    IIC_SendAck(0);//发送应答
  }
  buff[i]=IIC_Read_OneByte();//读一字节数据
  IIC_SendAck(1);//发送应答
  IIC_Send_Stop();//停止信号
  return 0;
}
#endif
/************************任意位置写入任意长度数据******************/
void AT24C08_WriteData(u16 addr,u8 *buff,u16 count)
{
  u8 page_remain=16-addr%16;//计算当前页剩余空间
  if(page_remain>=count)page_remain=count;
  while(1)
  {
    AT24C08_PagePragram(addr,buff,page_remain);
    if(count == page_remain)break;
    addr+=page_remain;
    buff+=page_remain;
    count-=page_remain;
    if(count>=16)page_remain=16;
    else page_remain=count;
  }
}
