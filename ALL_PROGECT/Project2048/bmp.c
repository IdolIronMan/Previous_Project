#include "bmp.h"

/***************************************
	跟BMP图片显示相关的功能模块(函数)
***************************************/

/*
	将bmppath所指向的BMP图片显示到屏幕的(x0,y0)处
		成功显示返回0,失败返回-1
*/
// #define MAX_SIZE 512
// int fd_num[MAX_SIZE] = {0};
// static int num;

int Display_Bmp(int x0, int y0, const char *bmppath)
{
	//打开BMP文件
	int fd_bmp = open(bmppath, O_RDONLY);
	if (fd_bmp == -1)
	{
		perror("Open Bmp File Failed");
		return -1;
	}

	unsigned char buf[4] = {0};
	//判断此文件是否为BMP文件
	int ret = read(fd_bmp, buf, 2);
	if (ret == -1)
	{
		perror("Read Bmp Failed");
		return -1;
	}
	//判断是否为bmp文件
	if (buf[0] != 0x42 || buf[1] != 0x4D)
	{
		printf("%s is not bmp file,bye bye!\n", bmppath);
		return -1;
	}

	//读取宽度数据
	lseek(fd_bmp, 0x12, SEEK_SET);
	ret = read(fd_bmp, buf, 4);
	if (ret == -1)
	{
		perror("Read Bmp Width Failed");
		return -1;
	}
	int width = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

	//读取高度数据
	lseek(fd_bmp, 0x16, SEEK_SET);
	ret = read(fd_bmp, buf, 4);
	if (ret == -1)
	{
		perror("Read Bmp Height Failed");
		return -1;
	}
	int height = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

	//读取色深数据
	lseek(fd_bmp, 0x1C, SEEK_SET);
	ret = read(fd_bmp, buf, 2);
	if (ret == -1)
	{
		perror("Read Bmp Depth Failed");
		return -1;
	}
	int depth = (buf[1] << 8) | buf[0];

	//测试用
	//printf("[WIDTH : %d] [HEIGHT : %d] [DEPTH : %d]\n", width, height, depth);

	//计算出BMP文件的每行所占的字节数
	int bytes_line_bmp = (depth / 8) * abs(width);

	//通过BMP文件每行所占的字节数计算出赖子的个数
	int laizi = 0;
	if (bytes_line_bmp % 4 != 0)
	{
		laizi = 4 - bytes_line_bmp % 4;
	}

	//用来保存像素数组的内存空间一行占字节数=BMP文件每行所占的字节数+赖子的个数
	int bytes_line_mem = bytes_line_bmp + laizi;

	//用来保存像素数组的内存空间大小为=一行占字节数*高度
	int size = bytes_line_mem * abs(height);

	//开辟空间
	unsigned char *piexl = malloc(size);

	//将像素数组读取到刚开辟出来的空间中去
	lseek(fd_bmp, 54, SEEK_SET);
	ret = read(fd_bmp, piexl, size);
	if (ret == -1)
	{
		perror("Read Piexl Failed");
		return -1;
	}

	//数据的处理
	int x, y;
	unsigned char a, r, g, b;
	int i = 0;
	int color;
	for (y = 0; y < abs(height); y++)
	{
		for (x = 0; x < abs(width); x++)
		{
			//取像素点的四个分量值
			b = piexl[i++];
			g = piexl[i++];
			r = piexl[i++];
			if (depth == 24)
			{
				a = 0x00;
			}
			else if (depth == 32)
			{
				a = piexl[i++];
			}
			//合成像素点的值
			color = (a << 24) | (r << 16) | (g << 8) |
					b;
			Lcd_Draw_Point((width > 0) ? (x0 + x) : (x0 + abs(width) - 1 - x), (height > 0) ? (y0 + height - 1 - y) : (y0 + y), color);
		}

		i = i + laizi;
	}
	close(fd_bmp);
	// fd_num[num++] = fd_bmp;

	return 0;
}

// int get_fd_num(int *fd_num)
// {
// 	int i, j;
// 	if ((sizeof(fd_num) / sizeof(fd_num[0])) == MAX_SIZE)
// 	{
// 		for (i = 0; i < (sizeof(fd_num) / sizeof(fd_num[0]) / 2);)

// 			close(fd_num[i]);
// 	}
// }