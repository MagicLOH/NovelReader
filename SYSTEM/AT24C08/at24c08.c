#include "at24c08.h"
#if IIC_MODE 
/**********************AT24C08ҳд��ʵ�ֿ����У�***********
**AT24C08�ռ��С:1024�ֽ�
**���ڵ�ַд��Ϊ1���ֽڣ�������Ҫa9a8ѡ��洢���С�
**a9a8=00 --������ַΪ0~255
**a9a8=01 --������ַΪ256~511
**a9a8=10 --������ַΪ512~767
**a9a8=11 --������ַΪ768~1023

**�β�:u8 addr -- д��λ��
**     u8 *buff --Ҫд��������׵�ַ
**     u8 count --д����ֽ���
**����ֵ:0 --�ɹ�������ֵ--ʧ��
**AT24C08һҳ��С:16�ֽ�
*********************************************************/
static u8 AT24C08_PagePragram(u16 addr,u8 *buff,u8 count)
{
  u8 i=0;
  u8 cnt=addr/256;//�����ַ���ĸ��洢������
  u8 addr_w=addr%256;//����д���ַ�ڴ洢�����ĸ�λ��
  IIC_Send_Start();//��ʼ�ź�
  IIC_Send_Addr(IIC_ADDR_W|(cnt<<1));//���ʹ��豸��ַ
  IIC_Send_OneByte(addr_w);//����Ҫд���λ��
  for(i=0;i<count;i++)
  {
     IIC_Send_OneByte(buff[i]);//����Ҫд���λ��

  }
  IIC_Send_Stop();//ֹͣ�ź�
  Delay_Ms(10);//�ȴ�����д��ɹ�
  return 0;  
}
/*****************������(������)*********************
**
**AT24C08�ռ��С:1024�ֽ�
**���ڵ�ַд��Ϊ1���ֽڣ�������Ҫa9a8ѡ��洢���С�
**a9a8=00 --������ַΪ0~255
**a9a8=01 --������ַΪ256~511
**a9a8=10 --������ַΪ512~767
**a9a8=11 --������ַΪ768~1023

**�β�:u8 addr -- ��ȡλ��
**     u8 *buff --�����ȡ���ݻ�����
**     u8 count --��ȡ���ֽ���
**����ֵ:0 --�ɹ�������ֵ--ʧ��
**
*******************************************************/
u8 AT24C08_ReadData(u16 addr,u8 *buff,u16 count)
{
  u16 i=0;
  u8 cnt=addr/256;//�����ַ���ĸ��洢������
  u8 addr_r=addr%256;//����д���ַ�ڴ洢�����ĸ�λ��
  IIC_Send_Start();//��ʼ�ź�
  IIC_Send_Addr(IIC_ADDR_W|(cnt<<1));//���ʹ��豸��ַ
  IIC_Send_OneByte(addr_r);//����Ҫ��ȡ��λ��
  IIC_Send_Start();//��ʼ�ź�
  IIC_Send_Addr(IIC_ADDR_R|(cnt<<1));//���ʹ��豸��ַ
  for(i=0;i<count;i++)
  {
    buff[i]=IIC_Read_OneByte();//��һ�ֽ�����

  }
  IIC_Send_Stop();//ֹͣ�ź�
  return 0;
}
#else 
/**********************AT24C08ҳд��ʵ�ֿ����У�***********
**AT24C08�ռ��С:1024�ֽ�
**���ڵ�ַд��Ϊ1���ֽڣ�������Ҫa9a8ѡ��洢���С�
**a9a8=00 --������ַΪ0~255
**a9a8=01 --������ַΪ256~511
**a9a8=10 --������ַΪ512~767
**a9a8=11 --������ַΪ768~1023

**�β�:u8 addr -- д��λ��
**     u8 *buff --Ҫд��������׵�ַ
**     u8 count --д����ֽ���
**����ֵ:0 --�ɹ�������ֵ--ʧ��
**AT24C08һҳ��С:16�ֽ�
*********************************************************/
static u8 AT24C08_PagePragram(u16 addr,u8 *buff,u8 count)
{
  u8 i=0;
  u8 cnt=addr/256;//�����ַ���ĸ��洢������
  u8 addr_w=addr%256;//����д���ַ�ڴ洢�����ĸ�λ��
  IIC_Send_Start();//��ʼ�ź�
  IIC_Send_OneByte(IIC_ADDR_W|(cnt<<1));//���ʹ��豸��ַ
  if(IIC_GetAck())return 1;//��ȡӦ��
  IIC_Send_OneByte(addr_w);//����Ҫд���λ��
  if(IIC_GetAck())return 2;//��ȡӦ��
  for(i=0;i<count;i++)
  {
     IIC_Send_OneByte(buff[i]);//����Ҫд���λ��
     if(IIC_GetAck())return 3;//��ȡӦ��
  }
  IIC_Send_Stop();//ֹͣ�ź�
  Delay_Ms(10);//�ȴ�����д��ɹ�
  return 0;  
}
/*****************������(������)*********************
**
**AT24C08�ռ��С:1024�ֽ�
**���ڵ�ַд��Ϊ1���ֽڣ�������Ҫa9a8ѡ��洢���С�
**a9a8=00 --������ַΪ0~255
**a9a8=01 --������ַΪ256~511
**a9a8=10 --������ַΪ512~767
**a9a8=11 --������ַΪ768~1023

**�β�:u8 addr -- ��ȡλ��
**     u8 *buff --�����ȡ���ݻ�����
**     u8 count --��ȡ���ֽ���
**����ֵ:0 --�ɹ�������ֵ--ʧ��
**
*******************************************************/
u8 AT24C08_ReadData(u16 addr,u8 *buff,u16 count)
{
  u16 i=0;
  u8 cnt=addr/256;//�����ַ���ĸ��洢������
  u8 addr_r=addr%256;//����д���ַ�ڴ洢�����ĸ�λ��
  IIC_Send_Start();//��ʼ�ź�
  IIC_Send_OneByte(IIC_ADDR_W|(cnt<<1));//���ʹ��豸��ַ
  if(IIC_GetAck())return 1;//��ȡӦ��
  IIC_Send_OneByte(addr_r);//����Ҫ��ȡ��λ��
  if(IIC_GetAck())return 2;//��ȡӦ��
  IIC_Send_Start();//��ʼ�ź�
  IIC_Send_OneByte(IIC_ADDR_R|(cnt<<1));//���ʹ��豸��ַ
  if(IIC_GetAck())return 3;//��ȡӦ��
  for(i=0;i<count-1;i++)
  {
    buff[i]=IIC_Read_OneByte();//��һ�ֽ�����
    IIC_SendAck(0);//����Ӧ��
  }
  buff[i]=IIC_Read_OneByte();//��һ�ֽ�����
  IIC_SendAck(1);//����Ӧ��
  IIC_Send_Stop();//ֹͣ�ź�
  return 0;
}
#endif
/************************����λ��д�����ⳤ������******************/
void AT24C08_WriteData(u16 addr,u8 *buff,u16 count)
{
  u8 page_remain=16-addr%16;//���㵱ǰҳʣ��ռ�
  if(page_remain>=count)page_remain=count;
  while(1)
  {
    AT24C08_PagePragram(addr,buff,page_remain);
    if(count == page_remain)break;
    addr+=page_remain;
    buff+=page_remain;
    count-=page_remain;
    if(count>=16)page_remain=16;
    else page_remain=count;
  }
}
