#ifndef __main_H__
#define __main_H__

#include <STC15F2K60S2.H>
#include "SoundPlay.h"
#include "key.h"
#include "lcd1602.h"
#include "hc595.h"
#include "DS1302.h"

enum Mode {
    setting = 0,
    ClockMode,
    KeyBoard,
    MusicPlay,
    ClockSet,
    AlarmSet
} mode;
unsigned char menuIndex = 0;
char code *menuItems[] = {"1.KeyBoard", "2.MusicPlay", "3.ClockSet", "4.AlarmSet"}; // 菜单项名称数组（使用code存储在程序存储器）
unsigned char menuNum   = 4;
bit UpdateTimeFlag      = 0; // 更新时间标志

void Delay1ms(unsigned int);
void Key_Process(void);
DAYTIME Clock;                          // 时钟结构体变量
void LCD1602_Display_Clock(DAYTIME *pClock);
#endif                                  // !__main_H__