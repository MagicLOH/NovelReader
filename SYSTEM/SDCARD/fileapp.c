#include "ff.h"//文件系统头文件
#include "w25q64.h"
#include <stdlib.h>
#include <string.h>
#include "sdcard.h"
#include "nt35310.h"
#include "xpt2046.h"
/***********************SD卡字库更新***********************/
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
  /*1.读取文件大小*/
  FILINFO file_info;
  f_stat(path,&file_info);
  size=file_info.fsize;
  if(size==0)return 1;
  //printf("文件大小:%u byte\r\n",size);
  /*2.打开文件*/
  res=f_open(&fp,path,FA_READ);
  if(res)
  {
   // printf("文件打开失败res=%d\r\n",res);
    return 2;
  }
	W25Q64_WriteData(addr-10,(u8 *)"        ",9);//清除标志位
	if(font_size==16)y=100;
	else if(font_size==24)y=140;
	else if(font_size==32)y=180;
	LCD_Display_Str2(20,y,16,(u8 *)"更新进度:",BLACK,WHITE);
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
			LCD_Display_Str2(30+strlen("更新进度")/2*16,y,16,(u8 *)"        ",WHITE,WHITE);
			snprintf(buff2,sizeof(buff2),"%.1f %%",load2);
			LCD_Display_Str2(30+strlen("更新进度")/2*16,y,16,(u8 *)buff2,RED,WHITE);
			LCD_Refresh();
		}
    //printf("更新进度:%d\r\n",cnt);
    if(br!=1024)break;
  }
	/*写入标志位*/
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
  f_close(&fp);//关闭文件
	return 0;
}

FILE_INFO *story_head=NULL;
/*创建链表*/
FILE_INFO *List_CreateHead(FILE_INFO *head)
{
	if(head!=NULL)return head;
	head=malloc(sizeof(FILE_INFO));
	head->next=NULL;
	head->pre=NULL;
	return head;
}
/*添加节点*/
FILE_INFO *List_AddNode(FILE_INFO *head)
{
	if(head==NULL)return NULL;//链表头不存在
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
/*遍历节点*/
void List_PrintNode(FILE_INFO *head)
{
	u16 x=20,y=50;
	LCD_Display_Str2(LCD_WIDTH/2-24,10,16,(u8 *)"书 架",BLACK,WHITE);
	if(head==NULL)return ;//链表头不存在
	FILE_INFO *phead=head;
	while(phead->next!=NULL)
	{
		phead=phead->next;
		phead->y=y;
		LCD_Display_Str2(x,phead->y,16,(u8 *)phead->file_name,DARKBLUE,WHITE);//显示字符串
		y+=35;
		//printf("%s,%d\r\n",phead->file_name,phead->file_size);
	}
	LCD_Refresh();
}
u16 List_CheckNode(FILE_INFO *head,u16 y,u8 *file_name)
{
	if(head==NULL)return 0;//链表头不存在
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
	return 0;//未找到 
}
/*************************************目录遍历(读取小说文件信息)************************/
u8 FATFS_printDir(const TCHAR* path)
{
  DIR dp;
  u8 res;
	u8 stat=0;
	story_head=List_CreateHead(story_head);//创建链表头
  res=f_opendir(&dp,path);
  FILINFO file_info;
  if(res)
  {
    printf("打开目录失败res=%d\r\n",res);
		free(story_head);//释放链表头
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
				stat=2;//动态分配空间失败
				goto AA;
			}
			//printf("文件名:%s,",file_info.fname);
			strcpy(temp->file_name,file_info.fname);//文件名
			if(file_info.fattrib == AM_ARC)//普通文件
			{
				temp->file_size=file_info.fsize;//文件大小
				//printf("文件大小:%u byte\r\n",file_info.fsize);
			}
		}
  }
AA:
	f_closedir(&dp);//关闭目录
	List_PrintNode(story_head);
	return stat;
}

/*打开小说*/
u8 buff_read[4098];
const u16 font_corlour[]={BLACK,BLUE,RED,LIGHTGREEN};
void FATFS_ReadFile(const char *file_name)
{
	FIL fp;//文件指针
	u8 res=0;
	UINT br;
	u32 font_buff[20];//保存每页字节数据
	u16 font_cnt=0;
	u16 font_len=0;
	u16 font_size=16;//字体大小
	u16 x=0,y=20;
	u8 corlour=0;
	char name[100];
	snprintf(name,sizeof(name),"0:/Text/%s",file_name);
	res=f_open(&fp,name,FA_READ);//打开文件
	if(res)
	{
		printf("文件打开失败res=%d\r\n",res);
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
			font_len=LCD_Display_Str(x,y,font_size,p,font_corlour[corlour]);//显示字符串
			LCD_Refresh();
			while(1)
			{
				res=XPT2046_ReadXY();//触摸屏检测
				if(res)
				{
					x1=xpt2046_info.x;
					y1=xpt2046_info.y;	
					while(T_PEN==0){}//等待松开
					//printf("x1=%d,y1=%d\r\n",x1,y1);
					if((x1>=212 && x1<=320) && (y1>=416 && y1<=480))//返回
					{
						LcdFill(0,0,320,480,WHITE);
						List_PrintNode(story_head);
						return ;
					}
					else if((x1>=106 && x1<=210) && (y1>=416 && y1<=480))//字体颜色
					{
						corlour++;
						if(corlour>=4)corlour=0;
						LcdFill(0,0,320,410,WHITE);
						break;
					}
					else if((x1>=1 && x1<=105) && (y1>=416 && y1<=480))//字体大小调节
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
/********************写文件************************/
u8 FATFS_WriteData(const TCHAR* path,u8 *buff,u32 len)
{
  FIL fp;//文件指针
  u8 res=0;
  UINT bw;  
  res=f_open(&fp,path,FA_WRITE|FA_OPEN_ALWAYS);//创建文件
  if(res)
  {
    printf("打开文件或创建文件失败err=%d\r\n",res);
    return 1;
  }  
  res=f_write(&fp,buff,len,&bw);
  f_close(&fp);

  if(res)
  {
    printf("读取数据失败ERR=%d\r\n",res);
    return 2;
  }
  else 
  {
    printf("读取成功,读取字节数br=%d\r\n",bw);
    return 0;
  }
}

