#ifndef __key_h__
#define __key_h__
#include "STC15F2K60S2.H"

// 行列按键所在端口的序号
// 例如Line1在端口P35，则对应的序号为5
#define Line1   5 // 行1的序号
#define Line2   6 // 行2的序号
#define Column1 4 // 列1的序号
#define Column2 3 // 列2的序号
#define Column3 2 // 列3的序号

// 独立按键所在端口的序号
// 例如KEY12在端口P34，则对应的序号为4
#define K12 4 // 按键12序号
#define K13 3 // 按键13序号
#define K14 2 // 按键14序号

// 行列按键所在的端口
#define KEYPORT P3

// 按键定义
#define KEY6   6
#define KEY7   7
#define KEY8   8
#define KEY9   9
#define KEY10  10
#define KEY11  11
#define KEY12  12
#define KEY13  13
#define KEY14  14
#define KEY12L 15
#define KEY13L 16
#define KEY14L 17

void IO_Init(void);
unsigned char Key_Scan(void);
void Key_Process(void);

#endif