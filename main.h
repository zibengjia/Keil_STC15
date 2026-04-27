#ifndef __main_H__
#define __main_H__

#include <STC15F2K60S2.H>
#include "key.h"
#include "lcd1602.h"
#include "hc595.h"
#include "DS1302.h"

void InitialSound(void);
void Play(unsigned char code *Sound, unsigned char Signature, unsigned Octachord, unsigned int Speed, bit musicPlayFlag);
unsigned int getPlayTime(void);

extern unsigned char code Music_Girl[];
extern unsigned char code Music_GGB[];
extern unsigned char code Music_Two[];
extern unsigned char code Music_Cao[];
extern unsigned char code Music_Chun[];
extern unsigned char code Music_ZuiXuan[];

enum Mode {
    KeyBoard = 0,
    MusicPlay,
    ClockSet,
    AlarmSet,
    setting,
    ClockMode,
} mode;
unsigned char menuIndex         = 0;
char code *menuItems[]          = {"1.KeyBoard", "2.MusicPlay", "3.ClockSet", "4.AlarmSet"}; // 菜单项名称数组（使用code存储在程序存储器）
unsigned char menuNum           = 4;
bit UpdateTimeFlag              = 0; // 更新时间标志
bit MusicPlayingFlag            = 0; // 音乐播放标志
unsigned char MusicIndex        = 0;
unsigned char code *MusicName[] = {"Haruhikage", "Keep Walking", "GGB", "Two Butterflies", "Orchid Grass", "The Coolest Ethnic Trend"};
unsigned char code *MusicData[] = {Music_Chun, Music_Girl, Music_GGB, Music_Two, Music_Cao, Music_ZuiXuan};
unsigned char musicNum          = sizeof(MusicName) / sizeof(MusicName[0]); // 计算歌曲数量
unsigned char clockSetIndex     = 1;                                        // 时钟设置项索引
void Delay1ms(unsigned int);
void Key_Process(void);
DAYTIME Clock; // 时钟结构体变量;
#endif // !__main_H__