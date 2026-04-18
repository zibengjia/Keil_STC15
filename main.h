#ifndef __main_H__
#define __main_H__

#include <STC15F2K60S2.H>
#include "SoundPlay.h"
#include "key.h"
#include "lcd1602.h"
#include "hc595.h"
#include "DS1302.h"

enum mode {
    setting = 0,
    KeyBoard,
    MusicPlay,
    ClockSet,
    AlarmSet
} mode;
unsigned char menuIndex = 0;
char *menuItems[]       = {{"1.KeyBoard"}, {"2.MusicPlay"}, {"3.ClockSet"}, {"4.AlarmSet"}}; // 菜单项名称数组
unsigned char menuNum   = 4;
bit UpdateTimeFlag      = 0; // 更新时间标志
/*
|乐曲名           : 要播放的乐曲指针,结尾以(0,0)结束;
|调号(0-11)       :	是指乐曲升多少个半音演奏;
|升降八度(1-3)	  : 1:降八度, 2:不升不降, 3:升八度;
|演奏速度(1-12000):	值越大速度越快;
*/

Music Music1 = {Music_Chun, "Haruhikage", 6, 2, 54};
Music Music2 = {Music_Girl, "Keep Walking", 6, 2, 54};
Music Music3 = {Music_GGB, "GGB", 6, 2, 54};
Music Music4 = {Music_Two, "Two Butterflies", 6, 2, 54};
Music Music5 = {Music_Cao, "Orchid Grass", 6, 2, 54};
Music Music6 = {Music_ZuiXuan, "The Coolest Ethnic Trend", 6, 2, 54};

void Delay1ms(unsigned int);
void Key_Process(void);
DAYTIME Clock; // 时钟结构体变量
void ToTimeDisplayBuf(DAYTIME *pClock); // 将时间转换为显示缓冲区格式的函数声明
#endif // !__main_H__