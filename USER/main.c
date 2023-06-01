#include <stdlib.h>
#include <string.h>

#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "ff.h"  //�ļ�ϵͳͷ�ļ�

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

u8 SDcard_DownFont(const TCHAR *path, u32 addr, u16 font_size);  // �ֿ����
u8 FATFS_printDir(const TCHAR *path);                            // ����Ŀ¼
u16 List_CheckNode(FILE_INFO *head, u16 y, u8 *file_name);       // �����ļ�
void FATFS_ReadFile(const char *file_name);                      // ���ļ�
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
    printf("���ڳ�ʼ�����\r\n");
    W25Q64_Init();
    IIC_Init();
    LCD_Init();
    Sram_Init();
    XPT2046_Init();  // ��ʼ��
    Touch_Calibration();
AA:
    LCD_Clear(WHITE);  // ��������
    LCD_Refresh();
    LCD_Display_Str2(20, 20, 16, (u8 *)"SD��״̬", RED, WHITE);
    res = f_mount(&fs, "0:", 1);  // ���̹���
    if (res) {
        printf("SD������ʧ��ERR=%d\r\n", res);
        snprintf(buff, sizeof(buff), "err%d", res);
        LCD_Display_Str2(20 + strlen("SD��״̬") * 12 + 20, 20, 16, (u8 *)buff, RED, WHITE);
        LCD_Display_Str2(20, 50, 16, (u8 *)"����SD���Ƿ����!", RED, WHITE);
        LCD_Refresh();
        Delay_Ms(500);
        goto AA;
    } else
        LCD_Display_Str2(20 + strlen("SD��״̬") * 12 + 20, 20, 16, (u8 *)"OK", RED, WHITE);
    /*�ֿ���*/
    LCD_Display_Str2(LCD_WIDTH / 2 - strlen("�ֿ���") / 2 * 16, 40, 16, (u8 *)"�ֿ���", RED, WHITE);
    LCD_Refresh();

GBK_16:
    W25Q64_ReadData(GBK_16_ADDR - 10, (u8 *)buff, 9);  // GBK16_OK
    if (strstr(buff, "GBK16_OK")) {
        LCD_Display_Str2(20, 60, 16, (u8 *)"GBK16    OK", RED, WHITE);
        LCD_Refresh();
    } else {
        LCD_Display_Str2(20, 60, 16, (u8 *)"GBK16    NO", RED, WHITE);
        LCD_Display_Str2(LCD_WIDTH / 2 - strlen("����GBK16�ֿ�") / 2 * 16, 80, 16, (u8 *)"����GBK16�ֿ�", RED, WHITE);
        LCD_Refresh();
        if (SDcard_DownFont("0:/font/GBK_16.DZK", GBK_16_ADDR, 16))  // �ֿ����
        {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 80, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(10, 80, 16, (u8 *)"�뽫GBK_16.DZK�ŵ�/font/Ŀ¼��,����!", BLACK, WHITE);
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
        LCD_Display_Str2(LCD_WIDTH / 2 - strlen("����GBK24�ֿ�") / 2 * 16, 120, 16, (u8 *)"����GBK24�ֿ�", RED, WHITE);
        LCD_Refresh();
        if (SDcard_DownFont("0:/font/GBK_24.DZK", GBK_24_ADDR, 24))  // �ֿ����
        {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 120, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(10, 120, 16, (u8 *)"�뽫GBK_24.DZK�ŵ�/font/Ŀ¼��,����!", BLACK, WHITE);
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
        LCD_Display_Str2(LCD_WIDTH / 2 - strlen("����GBK32�ֿ�") / 2 * 16, 160, 16, (u8 *)"����GBK32�ֿ�", RED, WHITE);
        LCD_Refresh();
        if (SDcard_DownFont("0:/font/GBK_32.DZK", GBK_32_ADDR, 32))  // �ֿ����
        {
            LCD_Display_Str2(LCD_WIDTH / 2 - strlen("                 ") / 2 * 16, 160, 16, (u8 *)"                 ", WHITE, WHITE);
            LCD_Display_Str2(10, 160, 16, (u8 *)"�뽫GBK_32.DZK�ŵ�/font/Ŀ¼��,����!", BLACK, WHITE);
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
    LCD_Clear(WHITE);  // ��������
    LCD_Refresh();
    FATFS_printDir("0:/Text/");
    while (1) {
        res = XPT2046_ReadXY();
        if (res) {
            // printf("x,y=%d,%d\r\n",xpt2046_info.x,xpt2046_info.y);
            y = List_CheckNode(story_head, xpt2046_info.y, (u8 *)buff);
            if (y) {
                LcdFill(0, y - 8, 320, y + 24, LIGHTGREEN);
                LCD_Display_Str2(20, y, 16, (u8 *)buff, DARKBLUE, LIGHTGREEN);  // ��ʾ�ַ���
                LCD_Refresh();
                while (T_PEN == 0); // �ȴ��ɿ�
                LcdFill(0, 0, 320, 480, WHITE);
                OLED_DrawRectangle(1, 415, 319, 479, BLACK);  // ���ƾ���
                OLED_DrawRectangle(2, 416, 318, 478, BLACK);  // ���ƾ���
                LCD_DrawLine(105, 415, 105, 479, BLACK);
                LCD_DrawLine(106, 415, 106, 479, BLACK);
                LCD_DrawLine(211, 415, 211, 479, BLACK);
                LCD_DrawLine(212, 415, 212, 479, BLACK);
                LCD_Display_Str2(5, 415 + 20, 24, (u8 *)"�����С", BLACK, WHITE);        // ��ʾ�ַ���
                LCD_Display_Str2(106 + 5, 415 + 20, 24, (u8 *)"������ɫ", BLACK, WHITE);  // ��ʾ�ַ���
                LCD_Display_Str2(212 + 29, 415 + 20, 24, (u8 *)"����", BLACK, WHITE);     // ��ʾ�ַ���
                LCD_Refresh();
                FATFS_ReadFile(buff);
            }
        }
    }
}
