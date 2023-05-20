#include "wdg.h"
/**********独立看门狗初始化**********/
void IWDG_Init(void)
{
  IWDG->KR=0X5555;//允许写
  IWDG->PR= 0x2;//分频因子16分频
  IWDG->RLR=0xfff;//重装载值，周期时间1638.4ms
  IWDG->KR=0xaaaa;
  IWDG->KR=0xcccc;//开启看门狗
}
