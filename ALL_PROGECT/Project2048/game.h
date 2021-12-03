#ifndef __GAME_H__
#define __GAME_H__

#include "head.h"
//存放11张照片的指针数组
#include "bmp.h"
#include "lcd.h"
#include "touch.h"
#include <pthread.h>
#include "main.h"

//随机生成2或者4
void rand_digital();

//游戏失败结束
int False();

//游戏开始
void my2048_start();

//开始按钮
int game_start();

//音乐播放
void *start_routine(void *arg);

//音乐播放
void mp3_play();

//判断游戏是否胜利
int game_win();

#endif
