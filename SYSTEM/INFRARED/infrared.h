#ifndef _INFRARED_H
#define _INFRARED_H
#include "stm32f10x.h"
#include "sys.h"
#define INFRARED_DECODE_MODE 1//定时器输入捕获，0外部中断+定时器
#define PWM_38KHZ_MODE 1//1表示定时器PWM输出，0表示模拟输出
#define Midea_DCODE 1 //表示美的空调解析
#define REMOTE_IN PBin(9) //红外接收引脚
#define INFRARED_CODE PEout(0)//红外发射引脚
extern u8 midea_buff[6];//保存美的空调48位数据
extern u32 infrared_decode_data;
extern u8 infrared_flag;
void HS0038_Init(void);
void Infrared_Code_Init(void);
void Infrared_SendData(u8 user,u8 data);
void Infrared_MideaSendData(u8 *buff);//编码美的空调协议
#endif
