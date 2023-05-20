#ifndef _IIC_H
#define _IIC_H
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#define IIC_MODE 0//1为硬件时序
#if IIC_MODE
void IIC_Init(void);//初始化
void IIC_Send_Start(void);//起始信号
void IIC_Send_Stop(void);//停止信号
void IIC_Send_Addr(u8 addr);//发送地址
void IIC_Send_OneByte(u8 data);//发送数据                            
u8 IIC_Read_OneByte(void);//读取数据
#else
#define IIC_SDA_IN_MODE()    {GPIOB->CRL&=0x0FFFFFFF;\
                              GPIOB->CRL|=0x80000000;}//输入模式
#define IIC_SDA_OUT_MODE() {GPIOB->CRL&=0x0FFFFFFF;\
                            GPIOB->CRL|=0x70000000;}//开漏输出模式
#define IIC_SDA_OUT PBout(7)
#define IIC_SDA_IN PBin(7)
#define IIC_SCL_OUT PBout(6)
void IIC_Init(void);//初始化
void IIC_Send_Start(void);//起始信号
void IIC_Send_Stop(void);//停止信号                           
u8 IIC_GetAck(void);//获取应答
void IIC_SendAck(u8 ack);//发送应答
void IIC_Send_OneByte(u8 data);//发送数据                            
u8 IIC_Read_OneByte(void);//读取数据
#endif

#endif
