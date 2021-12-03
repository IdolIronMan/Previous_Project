#ifndef __BMP_H__
#define __BMP_H__

#include "lcd.h"
#include "head.h"

//BMP图片素材的路径
#define BMP_PATH "/root/user/lifeiyang/project/bmpprofile/bmp/"

/*
	将bmppath所指向的BMP图片显示到屏幕的(x0,y0)处
		成功显示返回0,失败返回-1
*/
int Display_Bmp(int x0, int y0, const char *bmppath);

#endif
