#include "key.h"

void Delay_ms(unsigned int ms)
{
    unsigned int i;
    while ((ms--) != 0) {
        for (i = 0; i < 600; i++);
    }
}

/***********************************************
函数名称：IO_Init
功    能：单片机IO端口模式初始化
入口参数：无
返 回 值：无
备    注：无
************************************************/
void IO_Init(void)
{
    // 将P32 P33 P34 P35 P36设置为准双向口
    P3M1 &= ~((1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6));
    P3M0 &= ~((1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6));
}

/***********************************************
函数名称：Key_Scan
功    能：按键扫描函数
入口参数：无
返 回 值：键值
备    注：无
************************************************/
unsigned char Key_Scan(void)
{
    unsigned char KeyTemp1, KeyTemp2;
    unsigned char KeyValue = 0;
    static unsigned char KeyCount = 0;
    unsigned char longKey = 0;
    // 以下为S12，S13，S14独立按键扫描
    // 读入的端口先置高,准双向口做输入必须先置高
    // 1<<K12相当于1<<4 = (0001 0000) 即K12所在的P34口置高
    KEYPORT |= (1 << K12) | (1 << K13) | (1 << K14);
    // 将读入端口不用的位屏蔽掉
    KeyTemp1 = KEYPORT | (~((1 << K12) | (1 << K13) | (1 << K14)));
    if (KeyTemp1 != 0xff) // 如果有键按下
    {
        Delay_ms(10);                                                   // 延时，防抖动
        KeyTemp1 = KEYPORT | (~((1 << K12) | (1 << K13) | (1 << K14))); // 再次读取

        if (KeyTemp1 != 0xff) {
            KeyTemp2   = KeyTemp1;
            KeyCount   = 0;
            longKey    = 0;
            while (KeyTemp1 != 0xff) // 等待按键释放并判断长按
            {
                Delay_ms(10);
                KeyTemp1 = KEYPORT | (~((1 << K12) | (1 << K13) | (1 << K14)));
                if (KeyCount++ > 50) {
                    longKey = 1;
                }
            }
            switch (KeyTemp2) {
                case ~(1 << K12): // S12按下
                {
                    if (longKey == 0)
                        KeyValue = KEY12;
                    else
                        KeyValue = KEY12L;

                } break;
                case ~(1 << K13): // S13按下
                {
                    if (longKey == 0)
                        KeyValue = KEY13;
                    else
                        KeyValue = KEY13L;
                } break;
                case ~(1 << K14): // S14按下
                {
                    if (longKey == 0)
                        KeyValue = KEY14;
                    else
                        KeyValue = KEY14L;
                } break;
            }
            return KeyValue;
        } else {
            return 0;
        }
    }

    // 以下为S6~S11行列按键扫描
    // 所有列置低
    KEYPORT &= (~((1 << Column1) | (1 << Column2) | (1 << Column3)));
    // 读入端口先置高
    // 1<<Line1相当于1<<5 = (0010 0000) 即Line1所在的P35口置高
    KEYPORT |= (1 << Line1) | (1 << Line2);
    // 读取按键，将读入端口不用的位屏蔽掉
    KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2)));
    if (KeyTemp1 != 0xff) // 如果有键按下
    {
        Delay_ms(10);                                          // 延时，防抖动
        KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2))); // 再次读取
        if (KeyTemp1 != 0xff)                                  // 确实有按键按下
        {
            // 将第一列拉低，其他列置高,扫描第一列
            KEYPORT &= (~(1 << Column1));
            KEYPORT |= (1 << Column2) | (1 << Column3);
            KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2)));
            if (KeyTemp1 != 0xff) // 第一列有键按下
            {
                while (KeyTemp1 != 0xff) // 等待按键释放
                {
                    KeyTemp2 = KeyTemp1;
                    KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2))); // 重新读取
                }
                switch (KeyTemp2) {
                    case ~(1 << Line1): // S6按下
                    {
                        KeyValue = KEY6;

                    } break;
                    case ~(1 << Line2): // S9按下
                    {
                        KeyValue = KEY9;

                    } break;
                }
            }

            // 将第二列拉低，其他列置高,扫描第二列
            KEYPORT &= (~(1 << Column2));
            KEYPORT |= (1 << Column1) | (1 << Column3);
            KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2))); // 读取行
            if (KeyTemp1 != 0xff)                                  // 第二列有键按下
            {
                while (KeyTemp1 != 0xff) // 等待按键释放
                {
                    KeyTemp2 = KeyTemp1;
                    KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2))); // 重新读取
                }
                switch (KeyTemp2) {
                    case ~(1 << Line1): // S7按下
                    {
                        KeyValue = KEY7;

                    } break;
                    case ~(1 << Line2): // S10按下
                    {
                        KeyValue = KEY10;

                    } break;
                }
            }

            // 将第三列拉低，其他列置高,扫描第三列
            KEYPORT &= (~(1 << Column3));
            KEYPORT |= (1 << Column1) | (1 << Column2);
            KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2))); // 读取行
            if (KeyTemp1 != 0xff)                                  // 第三列有键按下
            {
                while (KeyTemp1 != 0xff) // 等待按键释放
                {
                    KeyTemp2 = KeyTemp1;
                    KeyTemp1 = KEYPORT | (~((1 << Line1) | (1 << Line2))); // 重新读取
                }
                switch (KeyTemp2) {
                    case ~(1 << Line1): // S8按下
                    {
                        KeyValue = KEY8;

                    } break;
                    case ~(1 << Line2): // S11按下
                    {
                        KeyValue = KEY11;

                    } break;
                }
            }
            return KeyValue;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}
