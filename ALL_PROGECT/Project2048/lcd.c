#include "lcd.h"

/***************************************
	跟显示屏相关的一些功能函数
***************************************/

extern char *bmp_name[];
extern int array[4][4];

//屏幕文件的文件描述符

static int fd_lcd = -1;

//指向映射区域(实际上就是指向第一个像素点)
static int *plcd = NULL;
/*
	对LCD屏幕的初始化
		初始化成功返回0,失败返回-1
*/
int Lcd_Init(void)
{
	//打开文件
	fd_lcd = open(LCD_PATH, O_RDWR);
	if (fd_lcd == -1)
	{
		perror("Open Lcd Failed");
		return -1;
	}

	//将屏幕文件映射到内存中去
	plcd = mmap(NULL, LCD_HEIGHT * LCD_WIDTH * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);
	if (plcd == MAP_FAILED)
	{
		perror("MMAP Failed");
		return -1;
	}
	return 0;
}

/*
	LCD的解初始化函数
*/
void Lcd_Uinit(void)
{
	munmap(plcd, LCD_HEIGHT * LCD_WIDTH * 4);
	close(fd_lcd);
}

/*
	画点函数:将(x,y)处的像素点显示成color颜色
*/
void Lcd_Draw_Point(int x, int y, int color)
{
	if (x >= 0 && x < LCD_WIDTH && y >= 0 && y < LCD_HEIGHT)
	{
		*(plcd + LCD_WIDTH * y + x) = color;
	}
}

/*
	画矩形:以(x0,y0)处为起始点画一个宽为w高为h颜色为color的矩形
*/
void Lcd_Draw_Orth(int x0, int y0, int w, int h, int color)
{
	int x, y;
	for (y = y0; y <= y0 + h; y++)
	{
		for (x = x0; x <= x0 + w; x++)
		{
			Lcd_Draw_Point(x, y, color);
		}
	}
}

char *get_filename_by_score(int score)
{
	int index;
	switch (score)
	{
	case 2:
		index = 0;
		break;
	case 4:
		index = 1;
		break;
	case 8:
		index = 2;
		break;
	case 16:
		index = 3;
		break;
	case 32:
		index = 4;
		break;
	case 64:
		index = 5;
		break;
	case 128:
		index = 6;
		break;
	case 256:
		index = 7;
		break;
	case 512:
		index = 8;
		break;
	case 1024:
		index = 9;
		break;
	case 2048:
		index = 10;
		break;
	}
	return bmp_name[index];
}

void lcd_draw_array()
{
	int i, j;
	int fd_num;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (array[i][j] == 0)
			{
				//画一个颜色矩形
				//void Lcd_Draw_Orth(int x0, int y0, int w, int h, int color)
				Lcd_Draw_Orth(10 + 120 * j, 10 + 120 * i, 100, 100, 0x00BFEFFF);
			}
			else
			{
				//画一张对应数字的bmp图片
				//int Display_Bmp(int x0, int y0, const char *bmppath)
				Display_Bmp(10 + 120 * j, 10 + 120 * i, get_filename_by_score(array[i][j]));
			}
		}
	}
}
