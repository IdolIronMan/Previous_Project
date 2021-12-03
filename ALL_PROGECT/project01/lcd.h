#ifndef __LCD_H__
#define __LCD_H__

#include "head.h"

#define LCD_PATH "/dev/fb0" //屏幕文件
#define LCD_HEIGHT 480

#define LCD_WIDTH 800

int LCD_Init(void);

void lcd_uinit(void);

void lcd_draw_point(int x, int y, int color);

void lcd_draw_orth(int x0, int y0, int w, int h, int color);
void lcd_draw_cricl(int x0, int y0, int r, int color1);
void lcd_draw_tai();
#endif
