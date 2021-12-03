#include "game.h"
/***************************************
	项目相关的主逻辑函数
***************************************/
char *bmp_name[] =
	{
		"/root/user/lifeiyang/project/bmpprofile/bmp/2.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/4.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/8.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/16.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/32.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/64.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/128.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/256.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/512.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/1024.bmp",
		"/root/user/lifeiyang/project/bmpprofile/bmp/2048.bmp",

};

//用来保存4*4矩阵的数值，初始化为两固定的位置
int array[4][4] =
	{
		0,
		0,
		0,
		0,
		0,
		2,
		0,
		0,
		0,
		0,
		0,
		0,
		4,
		0,
		0,
		0,
};

//生成随机的2或者4,根据当前的数组的情况来生成
void rand_digital()
{

	//i为横坐标，j为纵坐标,遍历一遍数组，看是否满
	int n = 0, i, j, num;
	//计算出当前的空位数
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (array[i][j] == 0)
			{
				n++;
			}
		}
	}

	//如果满了，直接返回
	if (!n)
	{
		printf("你情况很危险，游戏要结束了。\n");
		return;
	}

	// 没满，记录1——n
	num = rand() % n + 1;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (array[i][j] == 0)
			{
				n--;
				if (!n)
				{
					int t = rand() % 2;
					if (t == 0)
						array[i][j] = 2;
					else
						array[i][j] = 4;
				}
			}
}

//判断游戏是否结束
int False()
{
	int i, j;
	//通过判断左右任意一列是否有还有可以移动的数字来判断是否满
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (array[i][j] == 0 || array[i][j] == array[i][j + 1])
			{
				return 0;
			}
		}
	}
	//通过判断上下任意一列是否有还有可以移动的数字来判断是否满
	for (j = 0; j < 4; j++)
	{
		for (i = 0; i < 3; i++)
		{
			//
			if (array[i][j] == 0 || array[i + 1][j] == array[i][j])
			{
				return 0;
			}
		}
	}

	//数组都满了，返回 1
	return 1;
}

//点击开始游戏按钮
int game_start()
{
	while (1)
	{
		int x = get_x();
		int y = get_y();
		if (x > 380 && x < 640 && y > 400 && y < 550)
		{
			//printf("if里面的x:%d,y:%d\n", x, y);
			Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/background.bmp");
			return 0;
		}
		//printf("if外面的x:%d,y:%d\n", x, y);
		return 1;
	}
}

//重新开始游戏
void game_restart_or_back()
{
	while (1)
	{
		int direct;
		int x = get_x();
		int y = get_y();
		//重新开始游戏
		if (x > 730 && x < 920 && y > 350 && y < 450)
		{
			system("pkill -9 madplay");
			my2048_start();
		}
		//返回主界面,退出游戏
		if (x > 832 && x < 1024 && y > 0 && y < 125)
		{
			system("pkill -9 madplay");
			all_start();
		}
		printf("请点击restar按钮或者返回按钮\n");
	}
}
//判断游戏是否胜利
int game_win()
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (array[i][j] == 1024)
			{

				return 1;
			}
		}
	}

	return 0;
}

//清除数组
void game_clean(int array[4][4])
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			array[i][j] = 0;
		}
	}
	return;
}

//游戏主函数
void my2048_start()
{
	int direct;

	//清屏
	Lcd_Draw_Orth(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x250250);
	//system("madplay -Q ss.mp3 &");
	system("madplay -Q /root/user/lifeiyang/project/mp3profile/bgm.mp3 &");
	//展示封面
	Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/fengmian.bmp");
	sleep(1);
	Display_Bmp(300, 320, "/root/user/lifeiyang/project/bmpprofile/bmp/start.bmp");

	while (game_start())
	{
		printf("请按开始游戏按钮\n");
	}

	while (1)
	{

		lcd_draw_array();
		direct = get_finger_move_direct();

		handle(direct);
		//判断游戏失败   打印
		if (False())
		{
			printf("老铁，你凉了\n");
			system("pkill -9 madplay");
			system("madplay -Q /root/user/lifeiyang/project/mp3profile/defeat.mp3 &");
			Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/false.bmp");
			//system("pkill -9 madplay");
			sleep(2);
			//龙开口的图
			Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/restart.bmp");
			sleep(1);
			//展示restart按钮
			Display_Bmp(570, 280, "/root/user/lifeiyang/project/bmpprofile/bmp/re.bmp");
			//清空游戏数组
			game_clean(array);
			//展示返回按钮
			Display_Bmp(650, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/back.bmp");
			game_restart_or_back();

			//return;
		}
		//判断游戏是否胜利  打印
		if (game_win())
		{
			printf("老铁，你赢了\n");
			system("pkill -9 madplay");
			system("madplay -Q /root/user/lifeiyang/project/mp3profile/victory.mp3 &");
			Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/win.bmp");
			sleep(2);
			Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/restart.bmp");
			sleep(1);
			Display_Bmp(570, 280, "/root/user/lifeiyang/project/bmpprofile/bmp/re.bmp");
			game_clean(array);
			Display_Bmp(650, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/back.bmp");
			game_restart_or_back();
		}

		rand_digital();
	}
	system("pkill -9 madplay");
	return;
}

// //音乐播放
// void *start_routine(void *arg)
// {
// 	system("madplay ./1.mp3 &");
// }

// void mp3_play()
// {
// 	//双线程
// 	pthread_t pid;
// 	pthread_create(&pid, NULL, start_routine, NULL);
// }

// system("madplay -Q ss.mp3 &");

// system("pkill -9 madplay");