#include "SoundPlay.h"
#include "key.h"

//**************************************************************************

#define SYSTEM_OSC  11059200UL // 定义晶振频率，使用UL确保为无符号长整型
#define SOUND_SPACE 4 / 5      // 定义普通音符演奏的长度分率
sbit BeepIO = P1 ^ 6;          // 定义输出管脚
typedef struct {
    unsigned char code *Sound;
    unsigned char Signature;
    unsigned Octachord;
    unsigned int Speed;
} Music;
unsigned int code FreTab[12]    = {262, 277, 294, 311, 330, 349, 369, 392, 415, 440, 466, 494}; // 原始频率表
unsigned char code SignTab[7]   = {0, 2, 4, 5, 7, 9, 11};                                       // 1~7在频率表中的位置
unsigned char code LengthTab[7] = {1, 2, 4, 8, 16, 32, 64};
unsigned int idata PlayTimeTotal;
unsigned char idata Sound_Temp_TH0, Sound_Temp_TL0; // 音符定时器初值暂存
unsigned char idata Sound_Temp_TH1, Sound_Temp_TL1; // 音长定时器初值暂存
static unsigned int idata ResumePoint = 0;
static unsigned char code *LastSound  = 0;

static bit PauseRequested(void)
{
    if (Key_Scan() == KEY12L) {
        while (Key_Scan() != 0) {
        }
        return 1;
    }
    return 0;
}

//**************************************************************************
void InitialSound(void)
{

    unsigned int T1_Reload;
    BeepIO = 1;
    // 定时器1配置
    T1_Reload = 65536UL - (SYSTEM_OSC / 12 / 100);

    Sound_Temp_TH1 = T1_Reload / 256;
    Sound_Temp_TL1 = T1_Reload % 256;

    // 定时器配置寄存器设置
    TMOD = 0x11; // T0和T1都设置为模式1 (16位定时器)

    // STC15 AUXR寄存器设置:
    AUXR |= 0x80;  // T0设为1T
    AUXR &= ~0x40; // T1设为12T

    TH1 = Sound_Temp_TH1;
    TL1 = Sound_Temp_TL1;

    ET0 = 1;
    ET1 = 0; // T1用于查询，不开启中断
    TR0 = 0;
    TR1 = 0;
    EA  = 1;
}

void BeepTimer0(void) interrupt 1 // 音符发生中断 (T0)
{
    BeepIO = !BeepIO;
    // 重新加载初值
    TH0 = Sound_Temp_TH0;
    TL0 = Sound_Temp_TL0;
}

unsigned int getPlayTime(void)
{
    return PlayTimeTotal;
}

void Play(unsigned char code *Sound, unsigned char Signature, unsigned Octachord, unsigned int Speed, bit musicPlayFlag)
{
    unsigned char i;
    unsigned int Point, LDiv, LDiv0, LDiv1, LDiv2, LDiv4, CurrentFre, Temp_T, SoundLength;
    unsigned char Tone, Length, SL, SH, SM, SLen, XG, FD;
    unsigned char PlayTimes = 0;
    unsigned char NoteIndex;

    if (musicPlayFlag == 0) {
        TR0    = 0;
        TR1    = 0;
        BeepIO = 1;
        return;
    }

    SoundLength = 0;
    while (Sound[SoundLength] != 0x00) // 计算歌曲长度
    {
        SoundLength += 2;
    }

    if (Sound != LastSound) {
        LastSound  = Sound;
        ResumePoint = 0;
    }
    if (ResumePoint >= SoundLength) {
        ResumePoint = 0;
    }

    Point  = ResumePoint;
    Tone   = Sound[Point];
    Length = Sound[Point + 1]; // 读出第一个音符和它时时值

    LDiv0 = 12000 / Speed; // 算出1分音符的长度(单位:10ms)  <-- 这里假设基准是10ms
    LDiv4 = LDiv0 / 4;     // 算出4分音符的长度
    LDiv4 = LDiv4 - LDiv4 * SOUND_SPACE;

    TR0 = 0;
    TR1 = 1;

    while (Point < SoundLength) {
        if (PauseRequested()) {
            ResumePoint = Point;
            TR0         = 0;
            TR1         = 0;
            BeepIO      = 1;
            return;
        }
        SL = Tone % 10;      // 计算出音符
        SM = Tone / 10 % 10; // 计算出高低音
        SH = Tone / 100;     // 计算出是否升半
        if (SL != 0) {
            NoteIndex = SignTab[SL - 1] + SH;
            if (NoteIndex + Signature > 11) {
                CurrentFre = FreTab[NoteIndex + Signature - 12] * 2;
            } else {
                CurrentFre = FreTab[NoteIndex + Signature];
            }
            if (Octachord == 1)
                CurrentFre >>= 2; // 降八度
            else if (Octachord == 3)
                CurrentFre <<= 2;          // 升八度
            if (SM == 1) CurrentFre >>= 2; // 低音
            if (SM == 3) CurrentFre <<= 2; // 高音
            Temp_T         = 65536UL - (SYSTEM_OSC / CurrentFre / 2);
            Sound_Temp_TH0 = Temp_T / 256;
            Sound_Temp_TL0 = Temp_T % 256;
            TH0            = Sound_Temp_TH0;
            TL0            = Sound_Temp_TL0;
        }

        SLen = LengthTab[Length % 10]; // 算出是几分音符
        XG   = Length / 10 % 10;       // 算出音符类型(0普通1连音2顿音)
        FD   = Length / 100;           // 有无符点(0无1有)
        LDiv = LDiv0 / SLen;           // 算出连音音符演奏的长度(多少个10ms)
        if (FD == 1)
            LDiv = LDiv + LDiv / 2;
        if (XG != 1)
            if (XG == 0) // 算出普通音符的演奏长度
                if (SLen <= 4)
                    LDiv1 = LDiv - LDiv4;
                else
                    LDiv1 = LDiv * SOUND_SPACE; // 整数运算结果为0，即LDiv1=0
            else
                LDiv1 = LDiv / 2; // 算出顿音的演奏长度
        else
            LDiv1 = LDiv;
        if (SL == 0) LDiv1 = 0;
        LDiv2 = LDiv - LDiv1; // 算出不发音的长度
        if (SL != 0) {
            TR0 = 1;
            for (i = LDiv1; i > 0; i--) // 发规定长度的音
            {
                while (TF1 == 0); // 等待10ms到
                TH1 = Sound_Temp_TH1;
                TL1 = Sound_Temp_TL1;
                PlayTimes++;
                TF1 = 0;
                if (PauseRequested()) {
                    ResumePoint = Point;
                    TR0         = 0;
                    TR1         = 0;
                    BeepIO      = 1;
                    return;
                }
            }
        }
        if (LDiv2 != 0) {
            TR0    = 0;
            BeepIO = 1;
            for (i = LDiv2; i > 0; i--) // 音符间的间隔
            {
                while (TF1 == 0);
                TH1 = Sound_Temp_TH1;
                TL1 = Sound_Temp_TL1;
                PlayTimes++;
                TF1 = 0;
                if (PauseRequested()) {
                    ResumePoint = Point;
                    TR0         = 0;
                    TR1         = 0;
                    BeepIO      = 1;
                    return;
                }
            }
        }
        Point += 2;
        Tone   = Sound[Point];
        Length = Sound[Point + 1];
        if (PlayTimes >= 99) {
            PlayTimes = 0;
            PlayTimeTotal++;
        }
    }
    ResumePoint = 0;
    BeepIO = 1;
    TR0    = 0;
    TR1    = 0;
}
