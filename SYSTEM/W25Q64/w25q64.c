#include "w25q64.h"
/**********W25Q64初始化**********
**
**F_CS -- PB12 片选(低电平选中，高电平取消选中)
**
*********************************/
void W25Q64_Init(void)
{
  RCC->APB2ENR|=1<<3;//PB
  GPIOB->CRH&=~(0xF<<4*4);//清除PB12配置
  GPIOB->CRH|=0x3<<4*4;
  GPIOB->ODR|=1<<12;//上拉
  SPI2_Init();
}
/***************读取设备ID**************/
u16 W25Q64_ReadID(void)
{
  u16 id=0;
  F_CS=0;//片选拉低，选中W25Q64
  SPI2_WR_OneByte(0x90);//发送0x90
  //发送24位地址
  SPI2_WR_OneByte(0);
  SPI2_WR_OneByte(0);
  SPI2_WR_OneByte(0);
  /*读取制造商ID和设备ID*/
  id=SPI2_WR_OneByte(0xff);//造商ID
  id<<=8;
  id|=SPI2_WR_OneByte(0xff);//设备ID
  F_CS=1;//拉高片选，恢复为空闲状态
  return id; 
}
/*************获取芯片状态(0x5)*************/
static void W25Q64_WaitBusy(void)
{
  u8 stat=0xff;
  while(stat&0x01)
  {
    F_CS=0;//片选拉低，选中W25Q64
    SPI2_WR_OneByte(0x05);//获取状态
    stat=SPI2_WR_OneByte(0xff);//读取状态
    F_CS=1;//拉高片选，恢复为空闲状态
  }
}
/************************读数据******************
**
**形参:u32 addr --读取数据的位置
**     u8 *buff  --读数据缓冲区
**     u32 count  --读取的字节数
**
**************************************************/
void W25Q64_ReadData(u32 addr,u8 *buff,u32 count)
{
  u32 i=0;
  W25Q64_WaitBusy();//等待W25Q64正常工作
  F_CS=0;//片选拉低，选中W25Q64
  SPI2_WR_OneByte(0x03);//读数据
  //发送24位地址
  SPI2_WR_OneByte(addr>>16);
  SPI2_WR_OneByte(addr>>8);
  SPI2_WR_OneByte(addr&0xff);
  /*开始读取数据*/
  for(i=0;i<count;i++)
  {
    buff[i]=SPI2_WR_OneByte(0xff);//读数据
  }
  F_CS=1;//恢复为空闲状态
}
/*******************写使能************************/
static void W25Q64_WriteEnter(void)
{
  F_CS=0;//片选拉低，选中W25Q64
  SPI2_WR_OneByte(0x06);//写使能
  F_CS=1;
}
/*******************扇区擦除函数*********************
**
**形参:u32 addr --要擦除的扇区地址
**一次擦除4096字节
******************************************************/
void W25Q64_SectorErase(u32 addr)
{
  W25Q64_WriteEnter();//写使能
  W25Q64_WaitBusy();//等待W25Q64正常
  F_CS=0;
  SPI2_WR_OneByte(0x20);//扇区擦除指令
  //发送24位地址
  SPI2_WR_OneByte(addr>>16);
  SPI2_WR_OneByte(addr>>8);
  SPI2_WR_OneByte(addr&0xff);
  F_CS=1;
  W25Q64_WaitBusy();//等待擦除完成
}
/**********************页编程********************
**
**形参:u32 addr --写入数据的位置
**     u8 *buff  --写入数据首地址
**     u16 count  --写入的字节个数
**注:一页最大256字节
****************************************************/
static void W25Q64_PagePragram(u32 addr,const u8 *buff,u16 count)
{
  u16 i=0;
  if(count==0)return ;
  W25Q64_WriteEnter();//写使能
  F_CS=0;
  SPI2_WR_OneByte(0x02);//页编程
  //发送24位地址
  SPI2_WR_OneByte(addr>>16);
  SPI2_WR_OneByte(addr>>8);
  SPI2_WR_OneByte(addr&0xff);
  for(i=0;i<count;i++)
  {
    SPI2_WR_OneByte(buff[i]);
  }
  F_CS=1;
  W25Q64_WaitBusy();//等待写完成
}
/******************跨页编程(不带擦除功能)***************************/
static void W25Q64_PageWrite(u32 addr,const u8 *buff,u32 count)
{
  u16 page_remain=256-addr%256;//计算当前页剩余空间，addr=300,
  if(page_remain>=count)page_remain=count;//当前页可以直接写完
  while(1)
  {
    W25Q64_PagePragram(addr,buff,page_remain);//写入数据
    if(page_remain==count)break;
    addr+=page_remain;//地址往后偏移
    buff+=page_remain;//数据地址往后偏移
    count-=page_remain;//计算剩余未写入的字节数
    if(count>=256)page_remain=256;//判读剩余字节数是否超过1页
    else page_remain=count;
  }
}
static u8 sector_buff[4096];//扇区大小缓冲区
/**************跨页编程(带擦除功能)**********************/
void W25Q64_PageWrite_Erase(u32 addr,u8 *buff,u32 count)
{
  u16 i=0;
  u16 sector_remain=4096-addr%4096;//计算当前扇区剩余空间
  if(sector_remain>=count)sector_remain=count;//判断当前扇区是否可以写完
  while(1)
  {
    W25Q64_ReadData(addr,sector_buff,sector_remain);
    for(i=0;i<sector_remain;i++)
    {
      if(sector_buff[i]!=0xff)break;//判断当前空间是否全为0xff
    }
    if(i!=sector_remain)W25Q64_SectorErase(addr);//擦除当前扇区
    W25Q64_PageWrite(addr,buff,sector_remain);//写入数据
    if(count==sector_remain)break;
    count-=sector_remain;
    addr+=sector_remain;
    buff+=sector_remain;
    if(count>=4096)sector_remain=4096;
    else sector_remain=count;
  }
}
static u8 sector_buff2[4096];//扇区大小缓冲区
/*****************任意位置写入任意数据(考虑覆盖问题)*********************/
void W25Q64_WriteData(u32 addr,const u8 *buff,u32 count)
{
  u16 i=0;
  u32 sector_cnt=0;
  u16 sector_remain=4096-addr%4096;//计算当前扇区剩余空间
  if(sector_remain>=count)sector_remain=count;//判断当前扇区是否可以写完
  while(1)
  {
    sector_cnt=addr/4096;//判断当前地址在第几个扇区
    W25Q64_ReadData(addr,sector_buff,sector_remain);
    for(i=0;i<sector_remain;i++)
    {
      if(sector_buff[i]!=0xff)break;//判断当前空间是否全为0xff
    } 
    if(i!=sector_remain)
    {
      W25Q64_ReadData(sector_cnt*4096,sector_buff,addr-sector_cnt*4096);//读取扇区前半部分
      W25Q64_ReadData(addr+sector_remain,sector_buff2,(sector_cnt+1)*4096-(addr+sector_remain));//读取后半部分
      W25Q64_SectorErase(sector_cnt*4096);//擦除当前扇区
      W25Q64_PageWrite(sector_cnt*4096,sector_buff,addr-sector_cnt*4096);//写入前半部分
      W25Q64_PageWrite(addr+sector_remain,sector_buff2,(sector_cnt+1)*4096-(addr+sector_remain));//写入前半部分
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

