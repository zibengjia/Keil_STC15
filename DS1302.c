#include "ds1302.h"
#include <intrins.h>
// 秒    分    时    日    月   星期   年
// 初始化时间				  				//0	    30	  8	     2	   1	   1	  17
unsigned char code Init[7] = {0x00, 0x30, 0x08, 0x02, 0x01, 0x01, 0x17};


/***********************************************
函数名称：Delay_us
功    能：STC 1T单片机1us延时程序
入口参数：us:延时的微秒数
返 回 值：无
备    注：外部时钟11.0592MHz
************************************************/
void Delay_us(unsigned int us)
{
    while (us--) {
        _nop_();
    }
}
/***********************************************
函数名称：DS1302_ReadByte
功    能：从DS1302读一个字节数据
入口参数：无
返 回 值：unsigned char：读出的数据
备    注：无
************************************************/
unsigned char DS1302_ReadByte(void)
{
    unsigned char i;
    unsigned char dat = 0;
    for (i = 0; i < 8; i++) // 8位计数器
    {
        DS1302_SCK_Clr(); // 时钟线拉低
        Delay_us(1);      // 延时等待
        dat >>= 1;        // 数据右移一位
        if (SIO) {
            dat |= 0x80; // 读取数据
        }
        DS1302_SCK_Set(); // 时钟线拉高
        Delay_us(1);      // 延时等待
    }
    return dat;
}
/***********************************************
函数名称：DS1302_WriteByte
功    能：向DS1302写一个字节数据
入口参数：dat：写入的数据
返 回 值：无
备    注：无
************************************************/
void DS1302_WriteByte(unsigned char dat)
{
    unsigned char i;
    unsigned char temp;
    for (i = 0; i < 8; i++) // 8位计数器
    {
        temp = dat;
        DS1302_SCK_Clr();          // 时钟线拉低
        Delay_us(1);               // 延时等待
        if ((temp & 0x01) == 0x01) // 先发送低字节
        {
            DS1302_SIO_Set();
        } else {
            DS1302_SIO_Clr();
        }
        DS1302_SCK_Set(); // 时钟线拉高
        dat >>= 1;        // 移出数据
        Delay_us(1);      // 延时等待
    }
}
/***********************************************
函数名称：DS1302_ReadData
功    能：读DS1302某地址的的数据
入口参数：addr:地址。
返 回 值：读出的数据。
备    注：无
************************************************/
unsigned char DS1302_ReadData(unsigned char addr)
{
    unsigned char dat;

    DS1302_SCE_Clr(); // 片选线拉低
    Delay_us(1);
    DS1302_SCK_Clr(); // 时钟线拉低
    Delay_us(1);
    DS1302_SCE_Set(); // 片选线拉高
    Delay_us(1);
    DS1302_WriteByte(addr);  // 写地址
    dat = DS1302_ReadByte(); // 读数据
    DS1302_SCK_Set();        // 时钟线拉高
    DS1302_SCE_Clr();        // 片选线拉低

    return dat;
}
/***********************************************
函数名称：DS1302_WriteData
功    能：往DS1302的某个地址写入数据
入口参数：addr:地址。dat：写入的数据。
返 回 值：无
备    注：无
************************************************/
void DS1302_WriteData(unsigned char addr, unsigned char dat)
{
    DS1302_SCE_Clr(); // 片选线拉低
    Delay_us(1);
    DS1302_SCK_Clr(); // 时钟线拉低
    Delay_us(1);
    DS1302_SCE_Set(); // 时钟线拉高
    Delay_us(1);
    DS1302_WriteByte(addr); // 写地址
    DS1302_WriteByte(dat);  // 写数据
    DS1302_SCK_Set();       // 时钟线拉高
    DS1302_SCE_Clr();       // 片选线拉低
}
/***********************************************
函数名称：DS1302_WriteRAM
功    能：向DS1302的RAM中写入数据函数
入口参数：pBuf：指向设置值数据的指针
返 回 值：无
备    注：详见数据手册时间格式。
************************************************/
void DS1302_WriteRAM(unsigned char *pBuf)
{
    unsigned char addr = 0xc0; // 写ram开始地址
    unsigned char n    = 7;    // 使用7个字节的ram

    DS1302_WriteData(0x8e, 0x00); // 允许写操作
    while (n--)                   // 连续写7个RAM寄存器，地址增2.
    {
        DS1302_WriteData(addr, *pBuf++);
        addr += 2; // 按数据手册定义，地址以2递增
    }
    DS1302_WriteData(0x8e, 0x80); // 写保护
}
/***********************************************
函数名称：DS1302_ReadRAM
功    能：从DS1302的RAM中读出数据函数
入口参数：pBuf：指向保存时间数据的指针
返 回 值：无
备    注：无
************************************************/
void DS1302_ReadRAM(unsigned char *pBuf)
{
    unsigned char addr = 0xc1; // 读ram开始地址
    unsigned char n    = 7;    // 使用7个字节的ram

    while (n--) // 连续读7个RAM寄存器，地址增2.
    {
        *pBuf++ = DS1302_ReadData(addr);
        addr += 2;
    }
}
/***********************************************
函数名称：DS1302_GetTime
功    能：DS1302读取时钟函数
入口参数：pClock：指向时间对象的指针
返 回 值：无
备    注：读取到的时钟数据格式为BCD格式。
************************************************/
void DS1302_GetTime(DAYTIME *pClock)
{
    // 开始读取的地址
    unsigned char addr = 0x81;
    // 读取秒数
    pClock->Second = DS1302_ReadData(addr);
    addr += 2;
    // 读取分钟
    pClock->Minute = DS1302_ReadData(addr);
    addr += 2;
    // 读取小时
    pClock->Hour = DS1302_ReadData(addr);
    addr += 2;
    // 读取日
    pClock->Day = DS1302_ReadData(addr);
    addr += 2;
    // 读取月
    pClock->Month = DS1302_ReadData(addr);
    addr += 2;
    // 读取星期
    pClock->Week = DS1302_ReadData(addr);
    addr += 2;
    // 读取年
    pClock->Year = DS1302_ReadData(addr);
    addr += 2;
}
/***********************************************
函数名称：DS1302_SetTime
功    能：DS1302时间设置函数
入口参数：pClock：指向时间对象的指针
返 回 值：无
备    注：详见数据手册时间格式。
************************************************/
void DS1302_SetTime(DAYTIME *pClock)
{
    // 开始写入的地址
    unsigned char addr = 0x80;

    DS1302_WriteData(0x8e, 0x00); // 允许写操作

    // 写入秒数
    DS1302_WriteData(addr, pClock->Second);
    addr += 2;
    // 写入分钟
    DS1302_WriteData(addr, pClock->Minute);
    addr += 2;
    // 写入时
    DS1302_WriteData(addr, pClock->Hour);
    addr += 2;
    // 写入日
    DS1302_WriteData(addr, pClock->Day);
    addr += 2;
    // 写入月
    DS1302_WriteData(addr, pClock->Month);
    addr += 2;
    // 写入星期
    DS1302_WriteData(addr, pClock->Week);
    addr += 2;
    // 写入年
    DS1302_WriteData(addr, pClock->Year);
    addr += 2;

    DS1302_WriteData(0x8e, 0x80); // 写保护
}
/***********************************************
函数名称：DS1302_Init
功    能：DS1302初始化函数
入口参数：无
返 回 值：无
备    注：
          DS1302具体命令详见数据手册
************************************************/
void DS1302_Init(DAYTIME *pClock)
{
    unsigned char cnt;
    // 保存RAM数据
    unsigned char RAMData[7];

    // 将P21 P22设置为准双向口
    P2M1 &= ~((1 << 1) | (1 << 2));
    P2M0 &= ~((1 << 1) | (1 << 2));

    // 初始化P44口为准双向口
    P4M1 &= ~(1 << 4);
    P4M0 &= ~(1 << 4);

    DS1302_SCE_Clr();
    DS1302_SCK_Clr();

    DS1302_WriteData(0x8e, 0x00); // 允许写操作
    // 读取振荡器工作状态
    if ((DS1302_ReadData(0x81) & 0x80) == 0x80) {
        // 如果振荡器停止，则启动
        DS1302_WriteData(0x80, 0x00); // 时钟启动
    }
    DS1302_WriteData(0x90, 0xa6); // 一个二极管＋4K电阻充电
    DS1302_WriteData(0x8e, 0x80); // 写保护

    Delay_us(1000);
    DS1302_ReadRAM(RAMData); // 读取内部RAM数据

    for (cnt = 0; cnt < 7; cnt++) {
        if (RAMData[cnt] != Init[cnt]) // 如果不同，则认为未初始化
        {
            // 初始化时钟
            pClock->Second = Init[0];
            pClock->Minute = Init[1];
            pClock->Hour   = Init[2];
            pClock->Day    = Init[3];
            pClock->Month  = Init[4];
            pClock->Week   = Init[5];
            pClock->Year   = Init[6];

            DS1302_SetTime(pClock); // 重新初始化
            DS1302_WriteRAM(Init);
        }
    }
}
