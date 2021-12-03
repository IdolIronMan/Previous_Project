#ifndef __LED_H__
#define __LED_H__

#include "head.h"
#include "touch.h"

int led_init();

//控制每盏灯的亮灭,亮返回1，灭返回0

int led_control(int fd_led, int a);

int led_touch();

int led_make();
#endif