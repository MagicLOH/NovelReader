#include "wdg.h"
/**********�������Ź���ʼ��**********/
void IWDG_Init(void)
{
  IWDG->KR=0X5555;//����д
  IWDG->PR= 0x2;//��Ƶ����16��Ƶ
  IWDG->RLR=0xfff;//��װ��ֵ������ʱ��1638.4ms
  IWDG->KR=0xaaaa;
  IWDG->KR=0xcccc;//�������Ź�
}
