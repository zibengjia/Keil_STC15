#include "main.h"

/*
|乐曲名           : 要播放的乐曲指针,结尾以(0,0)结束;
|调号(0-11)       :	是指乐曲升多少个半音演奏;
|升降八度(1-3)	  : 1:降八度, 2:不升不降, 3:升八度;
|演奏速度(1-12000):	值越大速度越快;
*/

/* 键盘模式单音(音高, 音长, 结束符) */
unsigned char code KeyTone_Do[]  = {0x15, 0x03, 0x00, 0x00};
unsigned char code KeyTone_Re[]  = {0x16, 0x03, 0x00, 0x00};
unsigned char code KeyTone_Mi[]  = {0x17, 0x03, 0x00, 0x00};
unsigned char code KeyTone_Fa[]  = {0x18, 0x03, 0x00, 0x00};
unsigned char code KeyTone_Sol[] = {0x19, 0x03, 0x00, 0x00};
unsigned char code KeyTone_La[]  = {0x1A, 0x03, 0x00, 0x00};
unsigned char code KeyTone_Si[]  = {0x1B, 0x03, 0x00, 0x00};

void main()
{
    mode = ClockMode; // 初始模式为时钟模式
    IO_Init();
    DS1302_Init(&Clock);
    InitialSound();
    Timer2_Init();
    LCD1602_Init();

    while (1) {
        Key_Process();
        if (UpdateTimeFlag == 1 && mode == ClockMode) { // 只有在时钟模式下才更新时间
            // 定时读取时钟
            DS1302_GetTime(&Clock);        // 读取时钟
            LCD1602_Display_Clock(&Clock); // 显示时钟
            UpdateTimeFlag = 0;
        }
    }
}
void timer2_isr(void) interrupt 12
{
    static unsigned int T0_Cnt  = 0;
    static unsigned char T1_Cnt = 0;
    // 10ms

    T0_Cnt++;
    if (T0_Cnt == 99) // 定时1000ms
    {
        UpdateTimeFlag = 1;
        T0_Cnt         = 0;
    }
    if (T1_Cnt++ > 49) // 定时500ms
    {
        T1_Cnt = 0;
        if (mode == ClockSet) {
            ToDateNonDispBuf(clockSetIndex + 1); // 消隐当前设置项
        } else if (mode == ClockMode) {
            ToDateOrgDispBuf(clockSetIndex + 1, &Clock); // 恢复显示当前设置项
        }
    }
}
void Delay1ms(unsigned int count)
{
    unsigned int i, j;
    for (i = 0; i < count; i++)
        for (j = 0; j < 1200; j++);
}
void Key_Process(void)
{
    unsigned char KeyNum;
    static unsigned char speedKeyBoard   = 30; // 键盘模式下的演奏速度
    static signed char signatureKeyBoard = 2;  // 键盘模式下的调号
    if ((KeyNum = Key_Scan()) != 0)            // 检测是否有键按下
    {
        if (mode != setting) {
            if (KeyNum == KEY13L) {
                // 返回菜单
                mode = setting;
                LCD1602_Display_Str(LINE1, "Main Menu");
                LCD1602_Display_Str(LINE2, menuItems[menuIndex]); // 显示当前菜单项
            }
            switch (mode) {
                case ClockMode:
                    // 时钟显示模式下按键处理
                    if (KeyNum == KEY12L) {
                        // 进入设置模式
                        mode = setting;
                        LCD1602_Display_Str(LINE1, "Main Menu");
                        LCD1602_Display_Str(LINE2, menuItems[menuIndex]); // 显示当前菜单项
                    }
                    break;
                case KeyBoard:
                    // 键盘演奏模式下按键处理
                    switch (KeyNum) {
                        case KEY14:
                            LCD1602_Display_Str(LINE2, "Do");
                            Play(KeyTone_Do, 0, signatureKeyBoard, speedKeyBoard, 1);

                            break;
                        case KEY9:
                            LCD1602_Display_Str(LINE2, "Re");
                            Play(KeyTone_Re, 0, signatureKeyBoard, speedKeyBoard, 1);

                            break;
                        case KEY10:
                            LCD1602_Display_Str(LINE2, "Mi");
                            Play(KeyTone_Mi, 0, signatureKeyBoard, speedKeyBoard, 1);

                            break;
                        case KEY11:
                            LCD1602_Display_Str(LINE2, "Fa");
                            Play(KeyTone_Fa, 0, signatureKeyBoard, speedKeyBoard, 1);

                            break;
                        case KEY6:
                            LCD1602_Display_Str(LINE2, "Sol");
                            Play(KeyTone_Sol, 0, signatureKeyBoard, speedKeyBoard, 1);

                            break;
                        case KEY7:
                            LCD1602_Display_Str(LINE2, "La");
                            Play(KeyTone_La, 0, signatureKeyBoard, speedKeyBoard, 1);

                            break;
                        case KEY8:
                            LCD1602_Display_Str(LINE2, "Si");
                            Play(KeyTone_Si, 0, signatureKeyBoard, speedKeyBoard, 1);

                            break;
                    }
                    break;
                case MusicPlay:
                    // 音乐播放模式下按键处理
                    switch (KeyNum) {
                        case KEY12:
                            // 上一首
                            if (MusicIndex > 0) {
                                MusicIndex--;
                            } else {
                                MusicIndex = musicNum - 1; // 循环到最后一首
                            }
                            LCD1602_Display_Str(LINE2, MusicName[MusicIndex]); // 显示当前歌曲名称
                            break;
                        case KEY13:
                            // 下一首
                            if (MusicIndex < musicNum - 1) {
                                MusicIndex++;
                            } else {
                                MusicIndex = 0; // 循环到第一首
                            }
                            LCD1602_Display_Str(LINE2, MusicName[MusicIndex]); // 显示当前歌曲名称
                            break;
                        case KEY12L:
                            MusicPlayingFlag = 1; // 进入播放(播放中再次按KEY12L会在Play内部暂停)
                            Play(MusicData[MusicIndex], 0, 2, 54, 1);
                            MusicPlayingFlag = 0;
                            break;
                    }
                    break;
                case AlarmSet:
                    mode = AlarmSet;
                    break;
                case ClockSet:
                    // 时钟校准模式下按键处理

                    break;
            }
        } else {
            switch (KeyNum) {
                case KEY12:
                    // 上翻
                    if (menuIndex > 0) {
                        menuIndex--;
                    } else {
                        menuIndex = menuNum - 1; // 循环到最后一个菜单项
                    }
                    LCD1602_Display_Str(LINE2, menuItems[menuIndex]); // 显示当前菜单项
                    break;
                case KEY13:
                    // 下翻
                    if (menuIndex < menuNum - 1) {
                        menuIndex++;
                    } else {
                        menuIndex = 0; // 循环到第一个菜单项
                    }
                    LCD1602_Display_Str(LINE2, menuItems[menuIndex]); // 显示当前菜单项
                    break;
                case KEY13L:
                    // 返回初始界面
                    mode = ClockMode;
                    break;
                case KEY12L:
                    // 确认
                    mode = menuIndex;
                    switch (mode) {
                        case KeyBoard:
                            LCD1602_Display_Str(LINE2, "KeyBoard Mode");
                            break;
                        case MusicPlay:
                            LCD1602_Display_Str(LINE1, "Music:");
                            LCD1602_Display_Str(LINE2, MusicName[MusicIndex]); // 显示当前歌曲名称
                            break;
                        case ClockSet:
                            LCD1602_Display_Str(LINE2, "ClockSet Mode");
                            break;
                        case AlarmSet:
                            LCD1602_Display_Str(LINE2, "AlarmSet Mode");
                            break;
                    }
                    break;
            }
        }
    }
}
void LCD1602_Display_Clock(DAYTIME *pClock)
{
    // 先BCD码转换为十进制
    unsigned char DateStr[9];
    unsigned char TimeStr[9];                        // 存储日期字符串，格式为 "YY/MM/DD"
    DateStr[0] = ((pClock->Year >> 4) & 0x0F) + '0'; // 年十位
    DateStr[1] = (pClock->Year & 0x0F) + '0';        // 年个位
    DateStr[2] = '/';
    DateStr[3] = ((pClock->Month >> 4) & 0x0F) + '0'; // 月十位
    DateStr[4] = (pClock->Month & 0x0F) + '0';        // 月个位
    DateStr[5] = '/';
    DateStr[6] = ((pClock->Day >> 4) & 0 + 0x0F) + '0'; // 日十位
    DateStr[7] = (pClock->Day & 0x0F) + '0';            // 日个位
    DateStr[8] = '\0';                                  // 字符串结束标志

    TimeStr[0] = ((pClock->Hour >> 4) & 0x0F) + '0'; // 时十位
    TimeStr[1] = (pClock->Hour & 0x0F) + '0';        // 时个位
    TimeStr[2] = ':';
    TimeStr[3] = ((pClock->Minute >> 4) & 0x0F) + '0'; // 分十位
    TimeStr[4] = (pClock->Minute & 0x0F) + '0';        // 分个位
    TimeStr[5] = ':';
    TimeStr[6] = ((pClock->Second >> 4) & 0x0F) + '0'; // 秒十位
    TimeStr[7] = (pClock->Second & 0x0F) + '0';        // 秒个位
    TimeStr[8] = '\0';                                 // 字符串结束标志

    LCD1602_Display_Str(LINE1 + 3, TimeStr); // 显示日期
    LCD1602_Display_Str(LINE2 + 3, DateStr); // 显示时间
}