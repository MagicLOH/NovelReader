#include "ff.h"//�ļ�ϵͳͷ�ļ�
#include "w25q64.h"
#include <stdlib.h>
#include <string.h>
#include "sdcard.h"
#include "nt35310.h"
#include "xpt2046.h"
/***********************SD���ֿ����***********************/
u8 SDcard_DownFont(const TCHAR* path,u32 addr,u16 font_size)
{
  FIL fp;
  u8 res;
  UINT br;
  u32 size;
  u32 cnt=0;
	u16 y=0;
	float load=0,load2=0;
  u8 buff[1024];
	char buff2[20];
  /*1.��ȡ�ļ���С*/
  FILINFO file_info;
  f_stat(path,&file_info);
  size=file_info.fsize;
  if(size==0)return 1;
  //printf("�ļ���С:%u byte\r\n",size);
  /*2.���ļ�*/
  res=f_open(&fp,path,FA_READ);
  if(res)
  {
   // printf("�ļ���ʧ��res=%d\r\n",res);
    return 2;
  }
	W25Q64_WriteData(addr-10,(u8 *)"        ",9);//�����־λ
	if(font_size==16)y=100;
	else if(font_size==24)y=140;
	else if(font_size==32)y=180;
	LCD_Display_Str2(20,y,16,(u8 *)"���½���:",BLACK,WHITE);
	LCD_Refresh();
  while(1)
  {
    f_read(&fp,buff,1024,&br);
    W25Q64_WriteData(addr+cnt,buff,br);
    cnt+=br;
		load=(cnt*1.0/size)*100;
		if(load!=load2)
		{
			load2=load;
			LCD_Display_Str2(30+strlen("���½���")/2*16,y,16,(u8 *)"        ",WHITE,WHITE);
			snprintf(buff2,sizeof(buff2),"%.1f %%",load2);
			LCD_Display_Str2(30+strlen("���½���")/2*16,y,16,(u8 *)buff2,RED,WHITE);
			LCD_Refresh();
		}
    //printf("���½���:%d\r\n",cnt);
    if(br!=1024)break;
  }
	/*д���־λ*/
	if(font_size==16)
	{
		strcpy((char *)buff,"GBK16_OK");
	}
	else if(font_size==24)
	{
		strcpy((char *)buff,"GBK24_OK");
	}
	else if(font_size==32)
	{
		strcpy((char *)buff,"GBK32_OK");
	}
	W25Q64_WriteData(addr-10,buff,9);//GBK16_OK
  f_close(&fp);//�ر��ļ�
	return 0;
}

FILE_INFO *story_head=NULL;
/*��������*/
FILE_INFO *List_CreateHead(FILE_INFO *head)
{
	if(head!=NULL)return head;
	head=malloc(sizeof(FILE_INFO));
	head->next=NULL;
	head->pre=NULL;
	return head;
}
/*��ӽڵ�*/
FILE_INFO *List_AddNode(FILE_INFO *head)
{
	if(head==NULL)return NULL;//����ͷ������
	FILE_INFO *phead=head;
	while(phead->next!=NULL)
	{
		phead=phead->next;
	}
	FILE_INFO *new_node=malloc(sizeof(FILE_INFO));
	new_node->pre=phead;
	phead->next=new_node;
	new_node->next=NULL;
	return new_node;
}
/*�����ڵ�*/
void List_PrintNode(FILE_INFO *head)
{
	u16 x=20,y=50;
	LCD_Display_Str2(LCD_WIDTH/2-24,10,16,(u8 *)"�� ��",BLACK,WHITE);
	if(head==NULL)return ;//����ͷ������
	FILE_INFO *phead=head;
	while(phead->next!=NULL)
	{
		phead=phead->next;
		phead->y=y;
		LCD_Display_Str2(x,phead->y,16,(u8 *)phead->file_name,DARKBLUE,WHITE);//��ʾ�ַ���
		y+=35;
		//printf("%s,%d\r\n",phead->file_name,phead->file_size);
	}
	LCD_Refresh();
}
u16 List_CheckNode(FILE_INFO *head,u16 y,u8 *file_name)
{
	if(head==NULL)return 0;//����ͷ������
	FILE_INFO *phead=head;
	while(phead->next!=NULL)
	{
		phead=phead->next;
		if(y<=phead->y+26 && y>=phead->y-5)
		{
			strcpy((char *)file_name,phead->file_name);
			return phead->y;
		}
	}
	return 0;//δ�ҵ� 
}
/*************************************Ŀ¼����(��ȡС˵�ļ���Ϣ)************************/
u8 FATFS_printDir(const TCHAR* path)
{
  DIR dp;
  u8 res;
	u8 stat=0;
	story_head=List_CreateHead(story_head);//��������ͷ
  res=f_opendir(&dp,path);
  FILINFO file_info;
  if(res)
  {
    printf("��Ŀ¼ʧ��res=%d\r\n",res);
		free(story_head);//�ͷ�����ͷ
    return 1;
  }
	FILE_INFO *temp=NULL;
  while(1)
  {
    res=f_readdir(&dp,&file_info);
    if(res!=FR_OK || file_info.fname[0]==0)break;
		if(strstr(file_info.fname,".txt"))
		{
			temp=List_AddNode(story_head);
			if(temp==NULL)
			{
				stat=2;//��̬����ռ�ʧ��
				goto AA;
			}
			//printf("�ļ���:%s,",file_info.fname);
			strcpy(temp->file_name,file_info.fname);//�ļ���
			if(file_info.fattrib == AM_ARC)//��ͨ�ļ�
			{
				temp->file_size=file_info.fsize;//�ļ���С
				//printf("�ļ���С:%u byte\r\n",file_info.fsize);
			}
		}
  }
AA:
	f_closedir(&dp);//�ر�Ŀ¼
	List_PrintNode(story_head);
	return stat;
}

/*��С˵*/
u8 buff_read[4098];
const u16 font_corlour[]={BLACK,BLUE,RED,LIGHTGREEN};
void FATFS_ReadFile(const char *file_name)
{
	FIL fp;//�ļ�ָ��
	u8 res=0;
	UINT br;
	u32 font_buff[20];//����ÿҳ�ֽ�����
	u16 font_cnt=0;
	u16 font_len=0;
	u16 font_size=16;//�����С
	u16 x=0,y=20;
	u8 corlour=0;
	char name[100];
	snprintf(name,sizeof(name),"0:/Text/%s",file_name);
	res=f_open(&fp,name,FA_READ);//���ļ�
	if(res)
	{
		printf("�ļ���ʧ��res=%d\r\n",res);
		return ;
	}
	u8 *p;
	u16 x1,y1;
	u8 stat=0;
	while(1)
	{	
		if(stat)
		{
			buff_read[0]=stat;
			stat=0;
			res=f_read(&fp,&buff_read[1],4095,&br);
			br++;
			buff_read[br]='\0';
		}
		else
		{
			res=f_read(&fp,buff_read,4096,&br);
			buff_read[br]='\0';
		}
		p=buff_read;
		while(*p)
		{
			font_len=LCD_Display_Str(x,y,font_size,p,font_corlour[corlour]);//��ʾ�ַ���
			LCD_Refresh();
			while(1)
			{
				res=XPT2046_ReadXY();//���������
				if(res)
				{
					x1=xpt2046_info.x;
					y1=xpt2046_info.y;	
					while(T_PEN==0){}//�ȴ��ɿ�
					//printf("x1=%d,y1=%d\r\n",x1,y1);
					if((x1>=212 && x1<=320) && (y1>=416 && y1<=480))//����
					{
						LcdFill(0,0,320,480,WHITE);
						List_PrintNode(story_head);
						return ;
					}
					else if((x1>=106 && x1<=210) && (y1>=416 && y1<=480))//������ɫ
					{
						corlour++;
						if(corlour>=4)corlour=0;
						LcdFill(0,0,320,410,WHITE);
						break;
					}
					else if((x1>=1 && x1<=105) && (y1>=416 && y1<=480))//�����С����
					{
						if(font_size==16)font_size=24;
						else if(font_size==24)font_size=32;
						else if(font_size==32)font_size=16;
						LcdFill(0,0,320,410,WHITE);
						break;
					}
					else if(x1>160 && y1<=380)
					{
						p+=font_len&0x7fff;
						if(*p!='\0')
						{
							font_buff[font_cnt++]=font_len&0x7fff;
							LcdFill(0,0,320,410,WHITE);
							break;
						}
						else 
						{
							if(font_len&0x8000)
							{
								stat=*(p-1);
							}
							LcdFill(0,0,320,410,WHITE);
							break;
						}
					}
					else if(x1<160 && y1<=380)
					{
						if(p!=buff_read && font_cnt>0)
						{
							font_cnt--;
							p-=font_buff[font_cnt];
							LcdFill(0,0,320,410,WHITE);
							break;
						}
					}
				}
			}
		}

		font_cnt=0;
		memset(buff_read,0,sizeof(buff_read));
		if(br!=4096)break;
	}
}
/********************д�ļ�************************/
u8 FATFS_WriteData(const TCHAR* path,u8 *buff,u32 len)
{
  FIL fp;//�ļ�ָ��
  u8 res=0;
  UINT bw;  
  res=f_open(&fp,path,FA_WRITE|FA_OPEN_ALWAYS);//�����ļ�
  if(res)
  {
    printf("���ļ��򴴽��ļ�ʧ��err=%d\r\n",res);
    return 1;
  }  
  res=f_write(&fp,buff,len,&bw);
  f_close(&fp);

  if(res)
  {
    printf("��ȡ����ʧ��ERR=%d\r\n",res);
    return 2;
  }
  else 
  {
    printf("��ȡ�ɹ�,��ȡ�ֽ���br=%d\r\n",bw);
    return 0;
  }
}

