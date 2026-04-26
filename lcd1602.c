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
            LCD1602_WriteCMD(LINE1 + 4);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 2: // 月
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 7);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 3: // 日
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 10);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 4: // 星期
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 14);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 5: // 时
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 4);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;
        case 6: // 分
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 7);

            LCD1602_CheckBusy(); // 显示空格
            LCD1602_WriteDAT(' ');
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(' ');
        } break;

        case 7: // 秒
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE2 + 10);

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
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 4);

            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII(((pClock->Year) & 0xf0) >> 4)); // 年高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Year) & 0x0f)); // 年低位
        } break;

        case 2: // 月
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 7);

            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Month & 0xf0) >> 4)); // 月高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Month) & 0x0f)); // 月低位
        } break;

        case 3: // 日
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 10);

            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII(((pClock->Day) & 0xf0) >> 4)); // 日高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Day) & 0x0f)); // 日低位
        } break;

        case 4: // 星期
        {
            LCD1602_CheckBusy(); // 显示地址
            LCD1602_WriteCMD(LINE1 + 14);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Week) & 0x0f)); // 星期
        } break;

        case 5: // 时
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE2 + 4);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Hour & 0xf0) >> 4)); // 时高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Hour) & 0x0f)); // 时低位
        } break;

        case 6: // 分
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE2 + 7);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Minute & 0xf0) >> 4)); // 分高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Minute) & 0x0f)); // 分低位
        } break;

        case 7: // 秒
        {
            LCD1602_CheckBusy();
            LCD1602_WriteCMD(LINE2 + 10);
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Second) >> 4)); // 秒高位
            LCD1602_CheckBusy();
            LCD1602_WriteDAT(HEX2ASCII((pClock->Second) & 0x0f)); // 秒低位
        } break;
    }
}
