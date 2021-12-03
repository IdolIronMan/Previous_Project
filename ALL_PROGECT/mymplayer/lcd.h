#ifndef __LCD_H__
#define __LCD_H__

#include "head.h"
#include "bmp.h"

//屏幕文件的路径
#define LCD_PATH "/dev/fb0"
//屏幕文件的宽度
#define LCD_WIDTH 800
//屏幕文件的高度
#define LCD_HEIGHT 480

/*
	对LCD屏幕的初始化
		初始化成功返回0,失败返回-1
*/
int Lcd_Init(void);

/*
	画点函数:将(x,y)处的像素点显示成color颜色
*/
void Lcd_Draw_Point(int x, int y, int color);

/*
	画矩形:以(x0,y0)处为起始点画一个宽为w高为h颜色为color的矩形
*/
void Lcd_Draw_Orth(int x0, int y0, int w, int h, int color);

/*
	LCD的解初始化函数
*/
void Lcd_Uinit(void);

//根据score 找到要返回的图片的路径
char *get_filename_by_score(int score);

#endif
