#include "adc.h"
/********************ADC规则通道***************
**
**ADC1_IN8 --PB0
**
***********************************************/
void ADC_Regular_Init(void)
{
    /*1.开时钟*/
    RCC->APB2ENR|=1<<9;//ADC1
    RCC->APB2RSTR|=1<<9;//开复位时钟
    RCC->APB2RSTR&=~(1<<9);//取消复位
    /*2.GPIO口配置*/
    RCC->APB2ENR|=1<<3;//PB
    GPIOB->CRL&=0xfffffff0;//模拟输入
    /*3.ADC时钟频率配置*/
    RCC->CFGR&=~(0x3<<14);
    RCC->CFGR|=0x2<<14;//ADC工作频率:72/6=12MHZ
    /*4.ADC核心寄存器配置*/
    ADC1->CR1&=~(1<<8);//关闭扫描模式
    ADC1->CR2|=1<<23;//采集内部温度数据
    ADC1->CR2|=1<<20;//外部事件启动转换
    ADC1->CR2|=0x7<<17;//软件方式触发
    ADC1->CR2&=~(1<<11);//右对齐
    ADC1->CR2&=~(1<<1);//单次转换
    ADC1->SMPR1|=0x7<<18;//内部温度数据
    ADC1->SMPR2|=0x7<<24;//通道8采样周期数据
    ADC1->SQR1&=~(0xf<<20);//一个转换
    ADC1->CR2|=1<<0;//开启ADC
    ADC1->CR2|=1<<3;//初始化校准
    while(ADC1->CR2&1<<3);//等待初始化完成
    ADC1->CR2|=1<<2;//开始校准
    while(ADC1->CR2&1<<2);//等待校准完成
      
}
/**********获取一次数据***************/
u16 ADC1_Regular_GetCHx(u8 chx)
{
   ADC1->SQR3&=~(0x1F<<0);//清空序列中的值
   ADC1->SQR3|= chx<<0;
   ADC1->CR2|=1<<22;//开启规则通道转换 
   while(!(ADC1->SR&1<<1));//等待转换完成
   return ADC1->DR;
}




