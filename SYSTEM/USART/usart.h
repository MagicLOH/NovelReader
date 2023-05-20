#ifndef _USART_H
#define _USART_H
#include "stm32f10x.h"
#include "sys.h"
#include "beep.h"
#include <stdio.h>
extern u8 usart1_rx_buff[1024];//����1�������ݻ�����
extern u16 usart1_cnt;//���������°�
extern u8 usart1_flag;

extern u8 usart2_rx_buff[1024];//����1�������ݻ�����
extern u16 usart2_cnt;//���������°�
extern u8 usart2_flag;//������ɱ�־��

extern u8 usart3_rx_buff[1024];//����1�������ݻ�����
extern u16 usart3_cnt;//���������°�
extern u8 usart3_flag;//������ɱ�־��
void Usart1_Init(u32 baud);
void Usart1_SendStr(u8 *str);
void Usart2_Init(u32 baud);
void Usart2_SendStr(u8 *str);
void Usart3_Init(u32 baud);
void Usart3_SendStr(u8 *str);
void Usartx_Init(USART_TypeDef *USARTx,u32 baud,u32 sysclk);
void Usartx_SendString(USART_TypeDef *USARTx,u8 *str);
void Usartx_SendData(USART_TypeDef *USARTx,u8 *str,u32 len);
#endif
