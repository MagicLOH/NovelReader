#ifndef _IIC_H
#define _IIC_H
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#define IIC_MODE 0//1ΪӲ��ʱ��
#if IIC_MODE
void IIC_Init(void);//��ʼ��
void IIC_Send_Start(void);//��ʼ�ź�
void IIC_Send_Stop(void);//ֹͣ�ź�
void IIC_Send_Addr(u8 addr);//���͵�ַ
void IIC_Send_OneByte(u8 data);//��������                            
u8 IIC_Read_OneByte(void);//��ȡ����
#else
#define IIC_SDA_IN_MODE()    {GPIOB->CRL&=0x0FFFFFFF;\
                              GPIOB->CRL|=0x80000000;}//����ģʽ
#define IIC_SDA_OUT_MODE() {GPIOB->CRL&=0x0FFFFFFF;\
                            GPIOB->CRL|=0x70000000;}//��©���ģʽ
#define IIC_SDA_OUT PBout(7)
#define IIC_SDA_IN PBin(7)
#define IIC_SCL_OUT PBout(6)
void IIC_Init(void);//��ʼ��
void IIC_Send_Start(void);//��ʼ�ź�
void IIC_Send_Stop(void);//ֹͣ�ź�                           
u8 IIC_GetAck(void);//��ȡӦ��
void IIC_SendAck(u8 ack);//����Ӧ��
void IIC_Send_OneByte(u8 data);//��������                            
u8 IIC_Read_OneByte(void);//��ȡ����
#endif

#endif
