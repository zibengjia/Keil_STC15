#include "lcd1602.h"

#include <intrins.h>
/***********************************************
函数名称：LCD1602_Delay1us
功    能：STC 1T单片机1us延时程序
入口参数：us:延时的微秒数
返 回 值：无
备    注：外部时钟11.0592MHz
************************************************/
void LCD1602_Delay1us(unsigned int us)
{
    while (us--) {
        _nop_();
    }
}
/***********************************************
函数名称：LCD1602_Delay1ms
功    能：STC 1T单片机1ms延时程序
入口参数：ms:延时的毫秒数
返 回 值：无
备    注：示波器实测1.05ms 时钟11.0592MHz
************************************************/
void LCD1602_Delay1ms(unsigned int ms)
{
    unsigned int De_Cnt;
    while ((ms--) != 0) {
        for (De_Cnt = 0; De_Cnt < 600; De_Cnt++);
    }
}
/*************************************
 * 函 数 名: LCD1602_WriteCMD
 * 函数功能: 向lcd1602写指令
 * 入口参数: cmd:指令
 * 返    回: 无
 * 备    注：无
 **************************************/
void LCD1602_WriteCMD(unsigned char cmd)
{
    EN_Clr();
    RS_Clr();
    RW_Clr();
    LCD1602_Delay1us(10);
    EN_Set();
    LCD1602_Delay1us(10);
    DataPort = cmd;
    LCD1602_Delay1us(10);
    EN_Clr();
}
/***********************************************
函数名称：LCD1602_WriteDAT
功    能：向lcd1602写数据
入口参数：dat：数据
返 回 值：无
备    注：无
************************************************/
void LCD1602_WriteDAT(unsigned char dat)
{
    EN_Clr();
    RS_Set();
    RW_Clr();
    LCD1602_Delay1us(10);
    EN_Set();
    LCD1602_Delay1us(10);
    DataPort = dat;
    LCD1602_Delay1us(10);
    EN_Clr();
}
/***********************************************
函数名称：LCD1602_CheckBusy
功    能：检测lcd1602忙信号
入口参数：无
返 回 值：无
备    注：无
************************************************/
void LCD1602_CheckBusy(void)
{
    unsigned char temp;
    DataPort = 0xff; // 做输入先置高,51系列单片机读之前需要置高
    while (1) {
        EN_Clr();
        RS_Clr();
        RW_Set();
        LCD1602_Delay1us(10);
        EN_Set();
        LCD1602_Delay1us(10);
        temp = DataPort; // 读取忙通道数据
        LCD1602_Delay1us(10);
        EN_Clr();
        if ((temp & 0x80) != 0x80) {
            break;
        }
    }
}
/***********************************************
函数名称：LCD1602_Clear_Line
功    能：lcd1602清除行函数
入口参数：line：行地址
返 回 值：无
备    注：无
************************************************/
void LCD1602_Clear_Line(unsigned char line)
{
    unsigned char i;
    if ((line & LINE2) == LINE2) // 判断行地址
    {
        line = LINE2;
    } else {
        line = LINE1;
    }
    LCD1602_CheckBusy();
    LCD1602_WriteCMD(line);
    for (i = 0; i < 16; i++) // 写入空格，清除行
    {
        LCD1602_CheckBusy();
        LCD1602_WriteDAT(' ');
    }
}
/***********************************************
函数名称：LCD1602_Display_Str
功    能：lcd1602显示字符函数
入口参数：str指针地址
返 回 值：无
备    注：无
************************************************/
void LCD1602_Display_Str(unsigned char addr, unsigned char *str)
{
    LCD1602_Clear_Line(addr);

    // 设置地址
    LCD1602_CheckBusy();
    LCD1602_WriteCMD(addr);

    while (*str != '\0') // 未到字符串末尾
    {
        LCD1602_CheckBusy();
        LCD1602_WriteDAT(*str);
        str++;
    }
}
/***********************************************
函数名称：LCD1602_Init
功    能：LCD1602初始化
入口参数：无
返 回 值：无
备    注：详见数据手册定义。
************************************************/
void LCD1602_Init(void)
{
    // 初始化P0口为准双向口
    P0M1 = 0x00;
    P0M0 = 0x00;

    // 初始化P41,P42口为准双向口
    P4M1 &= ~((1 << 1) | (1 << 2));
    P4M0 &= ~((1 << 1) | (1 << 2));

    // 初始化P37口为准双向口
    P3M1 &= ~(1 << 7);
    P3M0 &= ~(1 << 7);

    // 分析
    //   1<<0等价于0x01 即 0000 0001
    //   1<<1等价于0x02 即 0000 0010
    //   1<<2等价于0x04 即 0000 0100
    //   1<<3等价于0x08 即 0000 1000
    //   以此类推1<<n 即第n位为1，其余位是0

    //  x |=(1<<n)  即对x执行按位取或
    //  即x中的第n位置为1，不改变其他位状态

    //  y &=~(1<<n)  即将1<<n按位取反，然后对y按位取与
    //  即y中的第n位置为0，不改变其他位状态

    LCD1602_Delay1ms(15);   // 上电延时15ms
    LCD1602_WriteCMD(0x38); // 写显示指令(不检测忙信号)
    LCD1602_Delay1ms(5);
    LCD1602_WriteCMD(0x38); // 写显示指令(不检测忙信号)
    LCD1602_Delay1ms(5);
    LCD1602_WriteCMD(0x38); // 写显示指令(不检测忙信号)
    LCD1602_CheckBusy();
    LCD1602_WriteCMD(0x38); // 写显示指令
    LCD1602_CheckBusy();
    LCD1602_WriteCMD(0x08); // 显示关闭
    LCD1602_CheckBusy();
    LCD1602_WriteCMD(0x01); // 清屏
    LCD1602_CheckBusy();
    LCD1602_WriteCMD(0x06); // 显示光标移动设置
    LCD1602_CheckBusy();
    LCD1602_WriteCMD(0x0c); // 显示开及光标设置

    // 显示欢迎信息
    LCD1602_Display_Str(LINE1 + 3, "MainMenu");
    LCD1602_Display_Str(LINE2 + 4, "Welcome!");
}
/***********************************************
函数名称：App_FormatDec
功    能：整型数据转字符串函数
入口参数：value:整型数据
返 回 值：pstr：指向字符串的指针。
备    注：无
************************************************/
void App_FormatDec(unsigned char *pstr, unsigned char value)
{
    unsigned char i;
    unsigned int mult;
    unsigned int nbr;

    mult = 1;
    // 因为这里要处理的value输入格式为三位数，例如123
    // 故初始让mult=100
    for (i = 0; i < 2; i++) {
        mult *= 10;
    }

    i = 0;
    while (mult > 0) {
        i++;
        // 第一次整除，获取最高位数据，例如123/100=1
        // 其他位类推
        nbr = value / mult;
        // 如果得到的数据不是0
        if (nbr != 0) {
            // 整型数据+'0'，将自动转换成字符数据，例如1+'0'将变成字符'1'
            *pstr = nbr + '0';
        } else {
            // 如果数据是0，则直接转换成字符'0'
            *pstr = '0';
        }
        // 移动指针，进行其他位数据处理
        pstr++;
        value %= mult;
        mult /= 10;
    }
}

/***********************************************
函数名称：HEX2ASCII
功    能：4位十六进制数转ASCII字符
入口参数：hex:0x00~0x0F
返 回 值：对应ASCII字符
备    注：无
************************************************/
static unsigned char HEX2ASCII(unsigned char hex)
{
    hex &= 0x0F;
    if (hex < 10) {
        return (unsigned char)('0' + hex);
    }
    return (unsigned char)('A' + (hex - 10));
}

/***********************************************
函数名称：ToDateNonDispBuf
功    能：时钟设置部分消隐
入口参数：无
返 回 值：无
备    注：无
************************************************/
void ToDateNonDispBuf(unsigned char set)
{
    switch (set) {
        case 1: // 年
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 3);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 2: // 月
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 6);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 3: // 日
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 9);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 4: // 星期
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 12);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 5: // 时
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 3);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 6: // 分
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 6);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;

        case 7: // 秒
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 9);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
    }
}

/***********************************************
函数名称：ToTimeOriDispBuf
功    能：显示消隐前的时间值
入口参数：无
返 回 值：无
备    注：无
************************************************/

void ToDateOrgDispBuf(unsigned char set, DAYTIME *pClock)
{
    switch (set) {
        case 1: // 年
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE2 + 3);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII(((pClock->Year) & 0xf0) >> 4)); // 年高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Year) & 0x0f)); // 年低位
        } break;

        case 2: // 月
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE2 + 5);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT('/');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Month & 0xf0) >> 4)); // 月高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Month) & 0x0f)); // 月低位
        } break;

        case 3: // 日
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 8);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT('/');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII(((pClock->Day) & 0xf0) >> 4)); // 日高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Day) & 0x0f)); // 日低位
        } break;

        case 4: // 星期
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 11);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT('/');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Week) & 0x0f)); // 星期
        } break;

        case 5: // 时
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE1 + 3);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Hour & 0xf0) >> 4)); // 时高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Hour) & 0x0f)); // 时低位
        } break;

        case 6: // 分
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE1 + 5);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(':');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Minute & 0xf0) >> 4)); // 分高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Minute) & 0x0f)); // 分低位
        } break;

        case 7: // 秒
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE1 + 8);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(':');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Second) >> 4)); // 秒高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Second) & 0x0f)); // 秒低位
        } break;
    }
}

void BCDToDecimal(unsigned char bcd, unsigned char *decimal)
{
    *decimal = ((bcd >> 4) * 10) + (bcd & 0x0F);
}
void DecimalToBCD(unsigned char decimal, unsigned char *bcd)
{
    *bcd = ((decimal / 10) << 4) | (decimal % 10);
}
/*
函数名称：TimeSet
功    能：时钟设置函数
入口参数：set:设置项索引，pClock:指向时钟结构体的指针，isAdd:增加或减少标志
*/
void TimeSet(unsigned char set, DAYTIME *pClock, bit isAdd)
{
    unsigned char add;
    if (isAdd)
        add = 1;
    else
        add = -1;
    switch (set) {
        case 1:
            // 年操作
            {
                unsigned char decimalYear;
                BCDToDecimal(pClock->Year, &decimalYear);
                decimalYear += add;
                if (decimalYear > Year_TOP) {
                    decimalYear = Year_BOTTOM;
                } else if (decimalYear < Year_BOTTOM) {
                    decimalYear = Year_TOP;
                }
                DecimalToBCD(decimalYear, &pClock->Year);
            }
            break;
        case 2:
            // 月操作
            {
                unsigned char decimalMonth;
                BCDToDecimal(pClock->Month, &decimalMonth);
                decimalMonth += add;
                if (decimalMonth > Month_TOP) {
                    decimalMonth = Month_BOTTOM;
                } else if (decimalMonth < Month_BOTTOM) {
                    decimalMonth = Month_TOP;
                }
                DecimalToBCD(decimalMonth, &pClock->Month);
            }
            break;
        case 3:
            // 日操作
            {
                unsigned char decimalDay;
                unsigned char month;
                BCDToDecimal(pClock->Day, &decimalDay);
                decimalDay += add;
                // 考虑到不同月份的天数

                BCDToDecimal(pClock->Month, &month);
                if (month == 2) { // 二月
                    if (decimalDay > 28) {
                        decimalDay = 1;
                    } else if (decimalDay < 1) {
                        decimalDay = 28;
                    }
                } else if (month == 4 || month == 6 || month == 9 || month == 11) { // 小月
                    if (decimalDay > 30) {
                        decimalDay = 1;
                    } else if (decimalDay < 1) {
                        decimalDay = 30;
                    }
                } else { // 大月
                    if (decimalDay > Day_TOP) {
                        decimalDay = Day_BOTTOM;
                    } else if (decimalDay < Day_BOTTOM) {
                        decimalDay = Day_TOP;
                    }
                }
                DecimalToBCD(decimalDay, &pClock->Day);
            }
            break;
        case 4:
            // 星期操作
            {
                unsigned char decimalWeek;
                BCDToDecimal(pClock->Week, &decimalWeek);
                decimalWeek += add;
                if (decimalWeek > Week_TOP) {
                    decimalWeek = Week_BOTTOM;
                } else if (decimalWeek < Week_BOTTOM) {
                    decimalWeek = Week_TOP;
                }
                DecimalToBCD(decimalWeek, &pClock->Week);
            }
            break;
        case 5:
            // 时操作
            {
                unsigned char decimalHour;
                BCDToDecimal(pClock->Hour, &decimalHour);
                decimalHour += add;
                if (decimalHour > Hour_TOP) {
                    decimalHour = Hour_BOTTOM;
                } else if (decimalHour < Hour_BOTTOM) {
                    decimalHour = Hour_TOP;
                }
                DecimalToBCD(decimalHour, &pClock->Hour);
            }
            break;
        case 6:
            // 分操作
            {
                unsigned char decimalMinute;
                BCDToDecimal(pClock->Minute, &decimalMinute);
                decimalMinute += add;
                if (decimalMinute > 59) {
                    decimalMinute = 0;
                } else if (decimalMinute < 0) {
                    decimalMinute = 59;
                }
                DecimalToBCD(decimalMinute, &pClock->Minute);
            }
            break;
        case 7:
            // 秒操作
            {
                unsigned char decimalSecond;
                BCDToDecimal(pClock->Second, &decimalSecond);
                decimalSecond += add;
                if (decimalSecond > 59) {
                    decimalSecond = 0;
                } else if (decimalSecond < 0) {
                    decimalSecond = 59;
                }
                DecimalToBCD(decimalSecond, &pClock->Second);
            }
    }
}
void LCD1602_Display_Clock(DAYTIME *pClock)
{
    // 先BCD码转换为十进制
    unsigned char DateStr[11];
    unsigned char TimeStr[9];                         // 存储日期字符串，格式为 "YY/MM/DD/W"
    DateStr[0]  = ((pClock->Year >> 4) & 0x0F) + '0'; // 年十位
    DateStr[1]  = (pClock->Year & 0x0F) + '0';        // 年个位
    DateStr[2]  = '/';
    DateStr[3]  = ((pClock->Month >> 4) & 0x0F) + '0'; // 月十位
    DateStr[4]  = (pClock->Month & 0x0F) + '0';        // 月个位
    DateStr[5]  = '/';
    DateStr[6]  = ((pClock->Day >> 4) & 0 + 0x0F) + '0'; // 日十位
    DateStr[7]  = (pClock->Day & 0x0F) + '0';            // 日个位
    DateStr[8]  = '/';
    DateStr[9]  = (pClock->Week & 0x0F) + '0'; // 星期，显示为0-6
    DateStr[10] = '\0';                        // 字符串结束标志

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
