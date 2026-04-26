#ifndef __DS1302_H__
#define __DS1302_H__

#include "stc15f2k60s2.h"

// DS1302接口定义
sbit SCE = P4 ^ 4; // DS1302选择端
sbit SIO = P2 ^ 2; // DS1302数据口
sbit SCK = P2 ^ 1; // DS1302时钟口

// 定义端口操作
#define DS1302_SCE_Clr() \
    {                    \
        SCE = 0;         \
    }
#define DS1302_SCE_Set() \
    {                    \
        SCE = 1;         \
    }

#define DS1302_SIO_Clr() \
    {                    \
        SIO = 0;         \
    }
#define DS1302_SIO_Set() \
    {                    \
        SIO = 1;         \
    }

#define DS1302_SCK_Clr() \
    {                    \
        SCK = 0;         \
    }
#define DS1302_SCK_Set() \
    {                    \
        SCK = 1;         \
    }

// 定义日期结构
typedef struct datetime {
    unsigned char Year;   // 年
    unsigned char Week;   // 星期
    unsigned char Month;  // 月
    unsigned char Day;    // 日
    unsigned char Hour;   // 时
    unsigned char Minute; // 分
    unsigned char Second; // 秒
} DAYTIME;
// 设定数据范围
#define Year_TOP      99 // 最大值
#define Year_BOTTOM   0  // 最小值

#define Week_TOP      7
#define Week_BOTTOM   1

#define Month_TOP     12
#define Month_BOTTOM  1

#define Day_TOP       31
#define Day_BOTTOM    1

#define Hour_TOP      23
#define Hour_BOTTOM   0

#define Minute_TOP    59
#define Minute_BOTTOM 0

#define Second_TOP    59
#define Second_BOTTOM 0

// 函数定义
unsigned char DS1302_ReadData(unsigned char addr);
void DS1302_WriteData(unsigned char addr, unsigned char dat);
void DS1302_GetTime(DAYTIME *pClock);
void DS1302_SetTime(DAYTIME *pClock);
void DS1302_Init(DAYTIME *pClock);

#endif