#include <stdlib.h>
#include <string.h>

#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "ff.h"  //文件系统头文件

#include "stm32f10x.h"
#include "beep.h"
#include "led.h"
#include "key.h"
#include "w25q64.h"
#include "at24c08.h"
#include "xpt2046.h"
#include "nt35310.h"
#include "sram.h"
#include "sdcard.h"

u8 SDcard_DownFont(const TCHAR *path, u32 addr, u16 font_size);  // 字库更新
u8 FATFS_printDir(const TCHAR *path);                            // 遍历目录
u16 List_CheckNode(FILE_INFO *head, u16 y, u8 *file_name);       // 查找文件
void FATFS_ReadFile(const char *file_name);                      // 读文件
FATFS fs;
int main()
{
    u16 y = 0;
    char buff[30];
    u8 res = 0;
    Beep_Init();
    Led_Init();
    Key_Init();
    Usartx_Init(USART1, 115200, 72);
    printf("串口初始化完成\r\n");
    W25Q64_Init();
    IIC_Init();
    LCD_Init();
    Sram_Init();
    XPT2046_Init();  // 初始化
    Touch_Calibration();
AA:
    LCD_Clear(WHITE);  // 清屏函数
    LCD_Refresh();
    LCD_Display_Str2(20, 20, 16, (u8 *)"SD卡状态", RED, WHITE);
    res = f_mount(&fs, "0:", 1);  // 磁盘挂载
    if (res) {
        printf("SD卡挂载失败ERR=%d\r\n", res);
        snprintf(buff, sizeof(buff), "err%d", res);
        LCD_Display_Str2(20 + strlen("SD卡状态") * 12 + 20, 20, 16, (u8 *)buff, RED, WHITE);
        LCD_Display_Str2(20, 50, 16, (u8 *)"请检查SD卡是否插入!", RED, WHITE);
        LCD_Refresh();
        Delay_Ms(500);
        goto AA;
    } else
        LCD_Display_Str2(20 + strlen("SD卡状态") * 12 + 20, 20, 16, (u8 *)"OK", RED, WHITE);
    /*字库检测*/
    LCD_Display_Str2(LCD_WIDTH / 2 - strlen("字库检测") / 2 * 16, 40, 16, (u8 *)"字库检测", RED, WHITE);
    LCD_Refresh();

GBK_16:
    W25Q64_ReadData(GBK_16_ADDR - 10, (u8 *)buff, 9);  // GBK16_OK
    if (strstr(buff, "GBK16_OK")) {
        LCD_Display_Str2(20, 60, 16, (u8 *)"GBK16    OK", RED, WHITE);
        LCD_Refresh();
    } else {
        LCD_Display_Str2(20, 60, 16, (u8 *)"GBK16    NO", RED, WHITE);
        LCD_Display_Str2(LCD_WIDTH / 2 - strlen("更新GBK16字库") / 2 * 16, 80, 16, (u8 *)"更新GBK16字库", RED, WHITE);
        LCD_Refresh();
        if (SDcard_DownFont("0:/font/GBK_16.DZK", GBK_16_ADDR, 16))  // 字库更新
        {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 80, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(10, 80, 16, (u8 *)"请将GBK_16.DZK放到/font/目录下,重启!", BLACK, WHITE);
            LCD_Refresh();
        } else {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 80, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(20, 100, 16, (u8 *)"                   ", WHITE, WHITE);
            LCD_Refresh();
            goto GBK_16;
        }
    }
    /*GBK24_OK*/
GBK_24:
    W25Q64_ReadData(GBK_24_ADDR - 10, (u8 *)buff, 9);
    if (strstr(buff, "GBK24_OK")) {
        LCD_Display_Str2(20, 100, 16, (u8 *)"GBK24    OK", RED, WHITE);
        LCD_Refresh();
    } else {
        LCD_Display_Str2(20, 100, 16, (u8 *)"GBK24    NO", RED, WHITE);
        LCD_Display_Str2(LCD_WIDTH / 2 - strlen("更新GBK24字库") / 2 * 16, 120, 16, (u8 *)"更新GBK24字库", RED, WHITE);
        LCD_Refresh();
        if (SDcard_DownFont("0:/font/GBK_24.DZK", GBK_24_ADDR, 24))  // 字库更新
        {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 120, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(10, 120, 16, (u8 *)"请将GBK_24.DZK放到/font/目录下,重启!", BLACK, WHITE);
            LCD_Refresh();
        } else {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 120, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(20, 140, 16, (u8 *)"                   ", WHITE, WHITE);
            LCD_Refresh();
            goto GBK_24;
        }
    }
    /*GBK32_OK*/
GBK_32:
    W25Q64_ReadData(GBK_32_ADDR - 10, (u8 *)buff, 9);
    if (strstr(buff, "GBK32_OK")) {
        LCD_Display_Str2(20, 140, 16, (u8 *)"GBK32    OK", RED, WHITE);
        LCD_Refresh();
    } else {
        LCD_Display_Str2(20, 140, 16, (u8 *)"GBK32    NO", RED, WHITE);
        LCD_Refresh();
        LCD_Display_Str2(LCD_WIDTH / 2 - strlen("更新GBK32字库") / 2 * 16, 160, 16, (u8 *)"更新GBK32字库", RED, WHITE);
        LCD_Refresh();
        if (SDcard_DownFont("0:/font/GBK_32.DZK", GBK_32_ADDR, 32))  // 字库更新
        {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 160, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(10, 160, 16, (u8 *)"请将GBK_32.DZK放到/font/目录下,重启!", BLACK, WHITE);
            LCD_Refresh();
        } else {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 160, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(20, 180, 16, (u8 *)"                   ", WHITE, WHITE);
            LCD_Refresh();
            goto GBK_32;
        }
    }
    Delay_Ms(1000);
    Delay_Ms(1000);
    Delay_Ms(1000);
    LCD_Clear(WHITE);  // 清屏函数
    LCD_Refresh();
    FATFS_printDir("0:/Text/");
    while (1) {
        res = XPT2046_ReadXY();
        if (res) {
            // printf("x,y=%d,%d\r\n",xpt2046_info.x,xpt2046_info.y);
            y = List_CheckNode(story_head, xpt2046_info.y, (u8 *)buff);
            if (y) {
                LcdFill(0, y - 8, 320, y + 24, LIGHTGREEN);
                LCD_Display_Str2(20, y, 16, (u8 *)buff, DARKBLUE, LIGHTGREEN);  // 显示字符串
                LCD_Refresh();
                while (T_PEN == 0); // 等待松开
                LcdFill(0, 0, 320, 480, WHITE);
                OLED_DrawRectangle(1, 415, 319, 479, BLACK);  // 绘制矩形
                OLED_DrawRectangle(2, 416, 318, 478, BLACK);  // 绘制矩形
                LCD_DrawLine(105, 415, 105, 479, BLACK);
                LCD_DrawLine(106, 415, 106, 479, BLACK);
                LCD_DrawLine(211, 415, 211, 479, BLACK);
                LCD_DrawLine(212, 415, 212, 479, BLACK);
                LCD_Display_Str2(5, 415 + 20, 24, (u8 *)"字体大小", BLACK, WHITE);        // 显示字符串
                LCD_Display_Str2(106 + 5, 415 + 20, 24, (u8 *)"字体颜色", BLACK, WHITE);  // 显示字符串
                LCD_Display_Str2(212 + 29, 415 + 20, 24, (u8 *)"返回", BLACK, WHITE);     // 显示字符串
                LCD_Refresh();
                FATFS_ReadFile(buff);
            }
        }
    }
}
