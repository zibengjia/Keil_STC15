#ifndef __LCD1602_H__
#define __LCD1602_H__

#include "stc15f2k60s2.h"
#include "DS1302.h"

// LCD1602操作位定义
#define DataPort P0
sbit RS = P4 ^ 2; // 寄存器选择 0:指令寄存器 1:数据寄存器
sbit RW = P4 ^ 1; // 读写控制 0：写  1：读
sbit EN = P3 ^ 7; // 读写数据使能   0：停止 1：启动

// 定义端口操作
#define SET 1 // 置高
#define CLR 0 // 置低

#define RS_Set()  \
    {             \
        RS = SET; \
    } // 端口置高
#define RS_Clr()  \
    {             \
        RS = CLR; \
    } // 端口置低

#define RW_Set()  \
    {             \
        RW = SET; \
    } // 端口置高
#define RW_Clr()  \
    {             \
        RW = CLR; \
    } // 端口置低

#define EN_Set()  \
    {             \
        EN = SET; \
    } // 端口置高
#define EN_Clr()  \
    {             \
        EN = CLR; \
    } // 端口置低

#define LINE1 0x80 // 第一行地址
#define LINE2 0xc0 // 第二行地址

// 函数声明
void LCD1602_Init(void);
void LCD1602_Display_Str(unsigned char addr, unsigned char *str);
void App_FormatDec(unsigned char *pstr, unsigned char value);
void ToDateNonDispBuf(unsigned char set);
void ToDateOrgDispBuf(unsigned char set, DAYTIME *pClock);
void TimeSet(unsigned char set, DAYTIME *pClock, bit isAdd);
void LCD1602_Display_Clock(DAYTIME *pClock);
#endif