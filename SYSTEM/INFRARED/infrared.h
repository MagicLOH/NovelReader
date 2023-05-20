#ifndef _INFRARED_H
#define _INFRARED_H
#include "stm32f10x.h"
#include "sys.h"
#define INFRARED_DECODE_MODE 1//��ʱ�����벶��0�ⲿ�ж�+��ʱ��
#define PWM_38KHZ_MODE 1//1��ʾ��ʱ��PWM�����0��ʾģ�����
#define Midea_DCODE 1 //��ʾ���Ŀյ�����
#define REMOTE_IN PBin(9) //�����������
#define INFRARED_CODE PEout(0)//���ⷢ������
extern u8 midea_buff[6];//�������Ŀյ�48λ����
extern u32 infrared_decode_data;
extern u8 infrared_flag;
void HS0038_Init(void);
void Infrared_Code_Init(void);
void Infrared_SendData(u8 user,u8 data);
void Infrared_MideaSendData(u8 *buff);//�������Ŀյ�Э��
#endif
