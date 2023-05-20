#include "w25q64.h"
/**********W25Q64��ʼ��**********
**
**F_CS -- PB12 Ƭѡ(�͵�ƽѡ�У��ߵ�ƽȡ��ѡ��)
**
*********************************/
void W25Q64_Init(void)
{
  RCC->APB2ENR|=1<<3;//PB
  GPIOB->CRH&=~(0xF<<4*4);//���PB12����
  GPIOB->CRH|=0x3<<4*4;
  GPIOB->ODR|=1<<12;//����
  SPI2_Init();
}
/***************��ȡ�豸ID**************/
u16 W25Q64_ReadID(void)
{
  u16 id=0;
  F_CS=0;//Ƭѡ���ͣ�ѡ��W25Q64
  SPI2_WR_OneByte(0x90);//����0x90
  //����24λ��ַ
  SPI2_WR_OneByte(0);
  SPI2_WR_OneByte(0);
  SPI2_WR_OneByte(0);
  /*��ȡ������ID���豸ID*/
  id=SPI2_WR_OneByte(0xff);//����ID
  id<<=8;
  id|=SPI2_WR_OneByte(0xff);//�豸ID
  F_CS=1;//����Ƭѡ���ָ�Ϊ����״̬
  return id; 
}
/*************��ȡоƬ״̬(0x5)*************/
static void W25Q64_WaitBusy(void)
{
  u8 stat=0xff;
  while(stat&0x01)
  {
    F_CS=0;//Ƭѡ���ͣ�ѡ��W25Q64
    SPI2_WR_OneByte(0x05);//��ȡ״̬
    stat=SPI2_WR_OneByte(0xff);//��ȡ״̬
    F_CS=1;//����Ƭѡ���ָ�Ϊ����״̬
  }
}
/************************������******************
**
**�β�:u32 addr --��ȡ���ݵ�λ��
**     u8 *buff  --�����ݻ�����
**     u32 count  --��ȡ���ֽ���
**
**************************************************/
void W25Q64_ReadData(u32 addr,u8 *buff,u32 count)
{
  u32 i=0;
  W25Q64_WaitBusy();//�ȴ�W25Q64��������
  F_CS=0;//Ƭѡ���ͣ�ѡ��W25Q64
  SPI2_WR_OneByte(0x03);//������
  //����24λ��ַ
  SPI2_WR_OneByte(addr>>16);
  SPI2_WR_OneByte(addr>>8);
  SPI2_WR_OneByte(addr&0xff);
  /*��ʼ��ȡ����*/
  for(i=0;i<count;i++)
  {
    buff[i]=SPI2_WR_OneByte(0xff);//������
  }
  F_CS=1;//�ָ�Ϊ����״̬
}
/*******************дʹ��************************/
static void W25Q64_WriteEnter(void)
{
  F_CS=0;//Ƭѡ���ͣ�ѡ��W25Q64
  SPI2_WR_OneByte(0x06);//дʹ��
  F_CS=1;
}
/*******************������������*********************
**
**�β�:u32 addr --Ҫ������������ַ
**һ�β���4096�ֽ�
******************************************************/
void W25Q64_SectorErase(u32 addr)
{
  W25Q64_WriteEnter();//дʹ��
  W25Q64_WaitBusy();//�ȴ�W25Q64����
  F_CS=0;
  SPI2_WR_OneByte(0x20);//��������ָ��
  //����24λ��ַ
  SPI2_WR_OneByte(addr>>16);
  SPI2_WR_OneByte(addr>>8);
  SPI2_WR_OneByte(addr&0xff);
  F_CS=1;
  W25Q64_WaitBusy();//�ȴ��������
}
/**********************ҳ���********************
**
**�β�:u32 addr --д�����ݵ�λ��
**     u8 *buff  --д�������׵�ַ
**     u16 count  --д����ֽڸ���
**ע:һҳ���256�ֽ�
****************************************************/
static void W25Q64_PagePragram(u32 addr,const u8 *buff,u16 count)
{
  u16 i=0;
  if(count==0)return ;
  W25Q64_WriteEnter();//дʹ��
  F_CS=0;
  SPI2_WR_OneByte(0x02);//ҳ���
  //����24λ��ַ
  SPI2_WR_OneByte(addr>>16);
  SPI2_WR_OneByte(addr>>8);
  SPI2_WR_OneByte(addr&0xff);
  for(i=0;i<count;i++)
  {
    SPI2_WR_OneByte(buff[i]);
  }
  F_CS=1;
  W25Q64_WaitBusy();//�ȴ�д���
}
/******************��ҳ���(������������)***************************/
static void W25Q64_PageWrite(u32 addr,const u8 *buff,u32 count)
{
  u16 page_remain=256-addr%256;//���㵱ǰҳʣ��ռ䣬addr=300,
  if(page_remain>=count)page_remain=count;//��ǰҳ����ֱ��д��
  while(1)
  {
    W25Q64_PagePragram(addr,buff,page_remain);//д������
    if(page_remain==count)break;
    addr+=page_remain;//��ַ����ƫ��
    buff+=page_remain;//���ݵ�ַ����ƫ��
    count-=page_remain;//����ʣ��δд����ֽ���
    if(count>=256)page_remain=256;//�ж�ʣ���ֽ����Ƿ񳬹�1ҳ
    else page_remain=count;
  }
}
static u8 sector_buff[4096];//������С������
/**************��ҳ���(����������)**********************/
void W25Q64_PageWrite_Erase(u32 addr,u8 *buff,u32 count)
{
  u16 i=0;
  u16 sector_remain=4096-addr%4096;//���㵱ǰ����ʣ��ռ�
  if(sector_remain>=count)sector_remain=count;//�жϵ�ǰ�����Ƿ����д��
  while(1)
  {
    W25Q64_ReadData(addr,sector_buff,sector_remain);
    for(i=0;i<sector_remain;i++)
    {
      if(sector_buff[i]!=0xff)break;//�жϵ�ǰ�ռ��Ƿ�ȫΪ0xff
    }
    if(i!=sector_remain)W25Q64_SectorErase(addr);//������ǰ����
    W25Q64_PageWrite(addr,buff,sector_remain);//д������
    if(count==sector_remain)break;
    count-=sector_remain;
    addr+=sector_remain;
    buff+=sector_remain;
    if(count>=4096)sector_remain=4096;
    else sector_remain=count;
  }
}
static u8 sector_buff2[4096];//������С������
/*****************����λ��д����������(���Ǹ�������)*********************/
void W25Q64_WriteData(u32 addr,const u8 *buff,u32 count)
{
  u16 i=0;
  u32 sector_cnt=0;
  u16 sector_remain=4096-addr%4096;//���㵱ǰ����ʣ��ռ�
  if(sector_remain>=count)sector_remain=count;//�жϵ�ǰ�����Ƿ����д��
  while(1)
  {
    sector_cnt=addr/4096;//�жϵ�ǰ��ַ�ڵڼ�������
    W25Q64_ReadData(addr,sector_buff,sector_remain);
    for(i=0;i<sector_remain;i++)
    {
      if(sector_buff[i]!=0xff)break;//�жϵ�ǰ�ռ��Ƿ�ȫΪ0xff
    } 
    if(i!=sector_remain)
    {
      W25Q64_ReadData(sector_cnt*4096,sector_buff,addr-sector_cnt*4096);//��ȡ����ǰ�벿��
      W25Q64_ReadData(addr+sector_remain,sector_buff2,(sector_cnt+1)*4096-(addr+sector_remain));//��ȡ��벿��
      W25Q64_SectorErase(sector_cnt*4096);//������ǰ����
      W25Q64_PageWrite(sector_cnt*4096,sector_buff,addr-sector_cnt*4096);//д��ǰ�벿��
      W25Q64_PageWrite(addr+sector_remain,sector_buff2,(sector_cnt+1)*4096-(addr+sector_remain));//д��ǰ�벿��
    }
    W25Q64_PageWrite(addr,buff,sector_remain);
    if(count==sector_remain)break;
    count-=sector_remain;
    addr+=sector_remain;
    buff+=sector_remain;
    if(count>=4096)sector_remain=4096;
    else sector_remain=count;  
  }
}

