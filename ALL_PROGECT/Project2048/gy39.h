#ifndef __GY39_H__
#define __GY39_H__

#include "head.h"
#include <termios.h> //配置串口的头文件

//串口初始化函数
int uart_init(const char *uart_name);

//gy39获取光照函数
int gy39_getLUX();

//gy39获取温度湿度海拔气压
void gy39_getHTP();

#endif