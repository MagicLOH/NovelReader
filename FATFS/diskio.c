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
#define DEV_SD		0	/*卷标号(C盘)，最大支持10个(0~9)*/



/*-----------------------------------------------------------------------*/
/* 获取设备状态                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* 设备初始化                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS stat;
	switch (pdrv) {
	case DEV_SD :
    #if !SD_SDIO
    SDcardDevice_Init();//SD卡初始化
    return FR_OK;
    #else
    stat=SD_Init();//SD卡初始化
    return stat;
    #endif
		
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* 读扇区                                                      */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* 设备卷标号 */
	BYTE *buff,		/* 读数据缓冲区 */
	LBA_t sector,	/* 扇区地址*/
	UINT count		/*扇区数量 */
)
{
	int result;

	switch (pdrv) {
	case DEV_SD :
      #if !SD_SDIO
      result=SDcard_ReadData(sector,buff,count);//读扇区
      if(result)res=RES_ERROR;//读扇区错误
      else res=RES_OK;
      return res;
      #else
      result=SD_ReadDisk(buff,sector,count);//读扇区
      if(result==0)return RES_OK;
      #endif
  
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* 写扇区                                                      */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* 设备卷标号 */
	const BYTE *buff,	/* 要写入的数据 */
	LBA_t sector,		/* 写入扇区地址 */
	UINT count			/* 写入的扇区数量*/
)
{
	//DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD :
    #if !SD_SDIO
		result = SDcard_WriteData(sector,(BYTE*)buff,count);//写扇区
    if(result)res=RES_ERROR;//读扇区错误
    else res=RES_OK;
    return res; 
    #else  
     result=SD_WriteDisk((u8 *)buff,sector,count);//写扇区
     if(result==0)return RES_OK; 
    #endif
  
	}
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/*  其他函数                                         */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* 设备卷标号 */
	BYTE cmd,		/* 命令参数*/
	void *buff		/* 数据保存地址 */
)
{
	switch (pdrv) {
	case DEV_SD :
    switch(cmd)
    {
      case GET_SECTOR_COUNT://扇区数量
        #if !SD_SDIO
        *((DWORD*)buff) = SDcard_GetSectorCount();//获取扇区数量
        #else  
          *(DWORD *)buff=SDCardInfo.CardCapacity/512;
        #endif
        break;
      case GET_SECTOR_SIZE://扇区大小
        *((DWORD*)buff)=512;
        break;
      case GET_BLOCK_SIZE://块大小(以扇区为单位)
        *((DWORD*)buff)=8;//一个块4096字节，8个扇区
        break;
    }
		return RES_OK;
	}
	return RES_PARERR;
}
/*RTC时间*/
DWORD get_fattime(void)
{
  DWORD time;
	time=(2021-1980)<<25|//年
				7<<21|//月
				27<<16|//日
				11<<11|//时
				12<<5|//分
				23<<0;//秒  
  return time;
}
