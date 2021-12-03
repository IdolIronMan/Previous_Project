#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "head.h"

//定义4个方向
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#define TOUCH_PATH "/dev/input/event0" //触摸文件

//获取屏幕的坐标点
void Get_Touch(void);

//获取手指滑动方向
int get_finger_move_direct();

void handle_up();
void handle_down();
void handle_left();
void handle_right();

void handle(int direct);

int get_x();
int get_y();
#endif
