#include "beep.h"
/********蜂鸣器初始化************
**
**硬件接口:BEEP -- PB8 (高电平驱动)
**
************************************/
void Beep_Init(void)
{
  /*1.开时钟*/
  RCC->APB2ENR|=1<<3;//PB
  /*2.配置GPIO口*/
  GPIOB->CRH&=0xFFFFFFF0;
  GPIOB->CRH|=0x00000003;//通用推挽输出模式
  /*3.上拉/下拉*/
}
