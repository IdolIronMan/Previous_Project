#include "touch.h"

/***************************************
	跟触摸屏相关的一些功能函数
***************************************/
//获取触摸屏的坐标函数
extern char *bmp_name[];
extern int array[4][4];

void Get_Touch(void)
{
	//打开触摸屏文件
	int fd_touch = open(TOUCH_PATH, O_RDWR);
	if (fd_touch == -1)
	{
	}

	int x = -1, y = -1;
	int ret;
	struct input_event ev;
	while (1)
	{
		ret = read(fd_touch, &ev, sizeof(ev));
		if (ret != sizeof(ev))
		{
			continue;
		}

		if (ev.type == EV_ABS && ev.code == ABS_X)
		{
			//意味着这是一个x轴的坐标输入事件
			x = ev.value;
		}
		else if (ev.type == EV_ABS && ev.code == ABS_Y)
		{
			//意味着这是一个y轴的坐标输入事件
			y = ev.value;
		}
		else if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
		{
			//意味着手指离开了屏幕
		}

		if (x >= 0 && y >= 0)
		{
			printf("Touch Point [ %d %d ]\n", x, y);
		}
	}
}

int get_x()
{
	int fd = open("/dev/input/event0", O_RDONLY);
	//判断
	int x;
	struct input_event ev;
	while (1)
	{
		read(fd, &ev, sizeof(ev));
		//判断

		if (ev.type == EV_ABS && ev.code == ABS_X)
		{
			x = ev.value;
			break;
		}
	}
	return x;
}

int get_y()
{
	int fd = open("/dev/input/event0", O_RDONLY);
	//判断
	int y;
	struct input_event ev;
	while (1)
	{
		read(fd, &ev, sizeof(ev));
		//判断
		if (ev.type == EV_ABS && ev.code == ABS_Y)
		{
			y = ev.value;
			break;
		}
	}
	return y;
}

int get_finger_move_direct() //获取滑动方向
{
	int fd = open("/dev/input/event0", O_RDONLY);
	//判断
	int x, y;
	int x_start, y_start, y_stop, x_stop;
	struct input_event ev;
	while (1)
	{
		read(fd, &ev, sizeof(ev));
		//判断

		if (ev.type == EV_ABS && ev.code == ABS_X)
		{
			x = ev.value;
		}
		if (ev.type == EV_ABS && ev.code == ABS_Y)
		{
			y = ev.value;
		}
		//判断按下去了
		if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value != 0)
		{
			x_start = x;
			y_start = y;
			printf("down(%d , %d )\n", x_start, y_start);
		}
		//判断抬起来了
		if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
		{
			x_stop = x;
			y_stop = y;
			printf("up (%d ,%d)\n", x_stop, y_stop);
			break;
		}
	}
	//发生了X轴方向的滑动
	if (abs(x_stop - x_start) - abs(y_stop - y_start) > 0 && abs(x_stop - x_start) > 30)
	{
		if (x_stop - x_start > 0) //右滑
			return RIGHT;
		else
			return LEFT;
	}
	//发生了Y轴方向的滑动
	if (abs(x_stop - x_start) - abs(y_stop - y_start) < 0 && abs(y_stop - y_start) > 25)
	{
		if (y_stop - y_start > 0)
			return DOWN;
		else
			return UP;
	}
}

//向上移
void handle_up()
{
	int i, j, k;
	//向上移动要靠拢
	for (j = 0; j < 4; j++)
	{
		int flag = 1;
		for (i = 1; i < 4; i++)
		{
			//移动点i，j
			for (k = i; k >= 1; k--)		   //移动到的现在的位置
				if (array[k - 1][j] || k == 1) //此时停住
				{
					if (k == 1 && array[k - 1][j] == 0)
					{
						array[k - 1][j] = array[i][j];
						array[i][j] = 0;
						continue;
					}
					if (array[i][j] == array[k - 1][j] && flag)
					{
						array[k - 1][j] = 2 * array[i][j];
						array[i][j] = 0;
						flag = 0;
					}
					else if (k != i) //如果移动了
					{
						array[k][j] = array[i][j];
						array[i][j] = 0;
					}
					break;
				}
		}
	}
}

//向下移动
void handle_down()
{
	int i, j, k;
	for (j = 0; j < 4; ++j)
	{
		int flag = 1;
		for (i = 2; i >= 0; --i)
		{ //移动点i，j

			for (k = i; k <= 2; ++k)		   //移动到的现在的位置
				if (array[k + 1][j] || k == 2) //此时停住
				{
					if (k == 2 && array[k + 1][j] == 0)
					{
						array[k + 1][j] = array[i][j];
						array[i][j] = 0;
						continue;
					}
					if (array[i][j] == array[k + 1][j] && flag)
					{
						array[k + 1][j] = 2 * array[i][j];
						array[i][j] = 0;
						flag = 0;
					}
					else if (k != i) //如果移动了
					{
						array[k][j] = array[i][j];
						array[i][j] = 0;
					}
					break;
				}
		}
	}
}

//向左移
void handle_left()
{
	int i, j, k;
	for (i = 0; i < 4; ++i)
	{
		int flag = 1;
		for (j = 1; j < 4; ++j)
			if (array[i][j])
			{									   //移动点i，j
				for (k = j; k >= 1; --k)		   //移动到的现在的位置
					if (array[i][k - 1] || k == 1) //此时停住
					{
						if (k == 1 && array[i][k - 1] == 0)
						{
							array[i][k - 1] = array[i][j];
							array[i][j] = 0;
							continue;
						}
						if (array[i][j] == array[i][k - 1] && flag)
						{
							array[i][k - 1] = 2 * array[i][j];
							array[i][j] = 0;
							flag = 0;
						}
						else if (k != j) //如果移动了
						{
							array[i][k] = array[i][j];
							array[i][j] = 0;
						}
						break;
					}
			}
	}
}

//向右移动
void handle_right()
{
	int i, j, k;

	for (j = 2; j >= 0; --j)
	{
		for (i = 0; i < 4; ++i)
		{
			//移动点i，j
			int flag = 1;
			for (k = j; k <= 2; ++k)		   //移动到的现在的位置
				if (array[i][k + 1] || k == 2) //此时停住
				{
					if (k == 2 && array[i][k + 1] == 0)
					{
						array[i][k + 1] = array[i][j];
						array[i][j] = 0;
						continue;
					}
					if (array[i][j] == array[i][k + 1] && flag)
					{
						array[i][k + 1] = 2 * array[i][j];
						array[i][j] = 0;
						flag = 0;
					}
					else if (k != j) //如果移动了
					{
						array[i][k] = array[i][j];
						array[i][j] = 0;
					}
					break;
				}
		}
	}
}

//通过方向来调用四个方向的函数
void handle(int direct)
{
	switch (direct)
	{
	case UP:
		handle_up();
		break;
	case DOWN:
		handle_down();
		break;
	case LEFT:
		handle_left();
		break;
	case RIGHT:
		handle_right();
		break;
	}
}
