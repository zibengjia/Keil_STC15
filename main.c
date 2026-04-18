#include "main.h"

void main()
{
    mode       = setting;
    DispBuf[0] = 0x3F;
    DispBuf[1] = 0x06;
    DispBuf[2] = 0x5B;
    DispBuf[3] = 0x4F;
    DispBuf[4] = 0x66;
    DispBuf[5] = 0x6D;
    DispBuf[6] = 0x7D;
    DispBuf[7] = 0x07;

    IO_Init();
    LCD1602_Init();
    HC595_Init();
    DS1302_Init(&Clock);
    InitialSound();
    while (1) {
        Key_Process();
        if ((UpdateTimeFlag == 1)) {
            UpdateTimeFlag = 0;
            // DS1302_GetTime(&Clock);   // 读取时钟
            HC595_SendWord(0x3f, 0x00);
            // ToTimeDisplayBuf(&Clock); // 显示时钟
        }
    }
}
void timer2_isr(void) interrupt 12
{
    static unsigned int T0_Cnt = 0; // 静态计数器
    // 10ms

    // 定时读取时钟
    T0_Cnt++;
    if (T0_Cnt == 100) // 定时1000ms
    {
        T0_Cnt         = 0;
        UpdateTimeFlag = 1; // 更新标记有效
    }
    IE2 &= 0xFB;
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
    if ((KeyNum = Key_Scan()) != 0) // 检测是否有键按下
    {
        switch (mode) {
            case KeyBoard:
                mode = KeyBoard;
                break;
            case MusicPlay:
                mode = MusicPlay;
                break;
            case AlarmSet:
                mode = AlarmSet;
                break;
            case ClockSet:
                mode = ClockSet;
                break;
            case setting:
                mode = setting;
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
                    case KEY12L:
                        // 确认
                        mode = menuIndex;
                        break;
                }
                break;
        }
    }
}
void ToTimeDisplayBuf(DAYTIME *pClock)
{
    DispBuf[7] = (pClock->Second) & 0x0f; // 秒低位
    DispBuf[6] = (pClock->Second) >> 4;   // 秒高位

    DispBuf[5] = 32; //-

    DispBuf[4] = (pClock->Minute) & 0x0f;
    ;                                          // 分钟低位
    DispBuf[3] = (pClock->Minute & 0xf0) >> 4; // 分钟高位

    DispBuf[2] = 32; //-

    DispBuf[1] = (pClock->Hour) & 0x0f;      // 小时低位
    DispBuf[0] = (pClock->Hour & 0xf0) >> 4; // 小时高位
}