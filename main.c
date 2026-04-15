#include <STC15F2K60S2.H>
#include "SoundPlay.h"
#include "key.h"

/*
|乐曲名           : 要播放的乐曲指针,结尾以(0,0)结束;
|调号(0-11)       :	是指乐曲升多少个半音演奏;
|升降八度(1-3)	  : 1:降八度, 2:不升不降, 3:升八度;
|演奏速度(1-12000):	值越大速度越快;*/
Music Music1 = {Music_Chun, 6, 2, 54};

void Delay1ms(unsigned int count)
{
    unsigned int i, j;
    for (i = 0; i < count; i++)
        for (j = 0; j < 1200; j++);
}

/***********************************************
函数名称：Key_Process
功    能：按键处理函数
入口参数：无
返 回 值：无
备    注：无
************************************************/
void Key_Process(void)
{
    unsigned char KeyNum;
    if ((KeyNum = Key_Scan()) != 0) // 检测是否有键按下
    {
        switch (KeyNum) {
            case KEY6: // S6：按下
            {

            } break;
            case KEY7: // S7: 按下
            {

            } break;
            case KEY8: // S8：按下
            {

            } break;
            case KEY9: // S9：按下
            {

            } break;
            case KEY10: // S10: 按下
            {

            } break;
            case KEY11: // S11：按下
            {

            } break;
            case KEY12: // S12：按下
            {

            } break;
            case KEY13: // S13: 按下
            {

            } break;
            case KEY14: // S14：按下
            {

            } break;
            case KEY12L: // S12：长按
            {

            } break;
            case KEY13L: // S13: 长按
            {

            } break;
            case KEY14L: // S14：长按
            {
            }
        }
    }
}

void main()
{
    IO_Init();
    InitialSound();
    Delay1ms(3000);
    Play(Music1);
    Delay1ms(3000);
    while (1) {
        Key_Process();
    }
}
