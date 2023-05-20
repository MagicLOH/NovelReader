/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdcard.h"
#include "rtc.h"
/* Definitions of physical drive number for each drive */
#define DEV_SD		0	/*����(C��)�����֧��10��(0~9)*/



/*-----------------------------------------------------------------------*/
/* ��ȡ�豸״̬                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS stat;
	switch (pdrv) {
	case DEV_SD :
    #if !SD_SDIO
    SDcardDevice_Init();//SD����ʼ��
    return FR_OK;
    #else
    stat=SD_Init();//SD����ʼ��
    return stat;
    #endif
		
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* ������                                                      */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* �豸���� */
	BYTE *buff,		/* �����ݻ����� */
	LBA_t sector,	/* ������ַ*/
	UINT count		/*�������� */
)
{
	int result;

	switch (pdrv) {
	case DEV_SD :
      #if !SD_SDIO
      result=SDcard_ReadData(sector,buff,count);//������
      if(result)res=RES_ERROR;//����������
      else res=RES_OK;
      return res;
      #else
      result=SD_ReadDisk(buff,sector,count);//������
      if(result==0)return RES_OK;
      #endif
  
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* д����                                                      */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* �豸���� */
	const BYTE *buff,	/* Ҫд������� */
	LBA_t sector,		/* д��������ַ */
	UINT count			/* д�����������*/
)
{
	//DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD :
    #if !SD_SDIO
		result = SDcard_WriteData(sector,(BYTE*)buff,count);//д����
    if(result)res=RES_ERROR;//����������
    else res=RES_OK;
    return res; 
    #else  
     result=SD_WriteDisk((u8 *)buff,sector,count);//д����
     if(result==0)return RES_OK; 
    #endif
  
	}
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/*  ��������                                         */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* �豸���� */
	BYTE cmd,		/* �������*/
	void *buff		/* ���ݱ����ַ */
)
{
	switch (pdrv) {
	case DEV_SD :
    switch(cmd)
    {
      case GET_SECTOR_COUNT://��������
        #if !SD_SDIO
        *((DWORD*)buff) = SDcard_GetSectorCount();//��ȡ��������
        #else  
          *(DWORD *)buff=SDCardInfo.CardCapacity/512;
        #endif
        break;
      case GET_SECTOR_SIZE://������С
        *((DWORD*)buff)=512;
        break;
      case GET_BLOCK_SIZE://���С(������Ϊ��λ)
        *((DWORD*)buff)=8;//һ����4096�ֽڣ�8������
        break;
    }
		return RES_OK;
	}
	return RES_PARERR;
}
/*RTCʱ��*/
DWORD get_fattime(void)
{
  DWORD time;
	time=(2021-1980)<<25|//��
				7<<21|//��
				27<<16|//��
				11<<11|//ʱ
				12<<5|//��
				23<<0;//��  
  return time;
}
