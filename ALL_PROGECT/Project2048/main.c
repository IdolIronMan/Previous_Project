#include "main.h"

extern char *bmp_name[];
extern int array[4][4];

void all_start()
{
	int x1, y1;
	//LCD屏幕的初始化
	Lcd_Init();
	//显示智能家居封面
	Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/mainintface3.bmp");
	while (1)
	{
		//点击控制灯
		led_make();

		int x1 = get_x();
		int y1 = get_y();
		if (x1 > 0 && x1 < 195 && y1 > 0 && y1 < 255)
		{
			//点击启动游戏
			my2048_start();
		}
		//点击获取光照强度，温湿度，海拔，大气压
		if (x1 > 205 && x1 < 625 && y1 > 0 && y1 < 275)
		{
			// //检测光照强度
			//gy39_getLUX();
			//printf("当前光照强度为:%d\n", lux);

			//检测温湿度，海拔，大气压
			gy39_getHTP();
		}
	}
}

int main()
{
	// //开始运行
	all_start();
	// //LCD屏幕的解初始化
	Lcd_Uinit();
}
