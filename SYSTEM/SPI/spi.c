#include "spi.h"
/*******SPI���ų�ʼ��***********
**
**SPI2_SCK --PB13ʱ��
**SPI2_MISO --PB14��������
**SPI2_MOSI --PB15������� 
**
*********************************/
void SPI2_Init(void)
{
  #if SPI_MODE
    RCC->APB2ENR|=1<<3;//PB
    GPIOB->CRH&=0x000FFFFF;
    GPIOB->CRH|=0xB8B00000;
    RCC->APB1ENR|=1<<14;//SPI2
    RCC->APB1RSTR|=1<<14;//������λʱ��
    RCC->APB1RSTR&=~(1<<14);//ȡ����λ
    SPI2->CR1|=1<<9;//�������Ƭѡ
    SPI2->CR1|=1<<8;//Ƭѡ���е�ƽΪ�ߵ�ƽ
//    SPI2->CR1&=~(0x7<<3);//����Ƶ��36/2=18MHZ
    SPI2->CR1|=1<<2;//��ģʽ
//    SPI2->CR1&=~(1<<1);//���е�ƽΪ�͵�ƽ
//    SPI2->CR1&=~(1<<0);//�����ز�������
    SPI2->CR1|=1<<6;//����SPI2
  
  #else
    RCC->APB2ENR|=1<<3;//PB
    GPIOB->CRH&=0x000FFFFF;
    GPIOB->CRH|=0x38300000;
    GPIOB->ODR|=1<<13;//ʱ���߿��е�ƽΪ�ߵ�ƽ
  #endif
}
/*************SPI2��дһ���ֽ�************/
u8 SPI2_WR_OneByte(u8 dat_w)
{
  #if SPI_MODE 
     SPI2->DR=dat_w;
     while(!(SPI2->SR&1<<1));//�ȴ����ݷ������
     while(!(SPI2->SR&1<<0));//�ȴ����ݵ���
     return SPI2->DR;
  #else
    u8 i=0;
    u8 dat_r=0;
    for(i=0;i<8;i++)
    {
      SPI2_SCK=0;//ʱ�������ͣ����ߴӻ���������ʼ��������
      SPI2_MOSI=dat_w&0x80?1:0;
  //    if(dat_w&0x80)SPI2_MOSI=1;
  //    else SPI2_MOSI=0;
      SPI2_SCK=1;//���ݷ������
      dat_w<<=1;//����������һλ����
      //��ȡ����
      dat_r<<=1;
      if(SPI2_MISO)dat_r|=0x1;//��������1
    }
    SPI2_SCK=1;//ʱ���߻ָ�Ϊ���е�ƽ
  return dat_r;
  #endif
}
