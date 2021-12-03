#include "head.h"
#include "bmp.h"
#include "lcd.h"
#include "touch.h"

#define FIFO_PATH "/root/user/lifeiyang/myfifo1"
#define FILENAME_MAXLEN 4096

//数据结点
typedef struct node
{
	char filename[FILENAME_MAXLEN]; //用来保存某路径下所有的文件的文件名
	struct node *next;
	struct node *prev;
	int file_type; //用来保存某路径下所有的文件的文件类型
} Node;

//头结点
typedef struct head
{
	struct node *first;
	struct node *last;
} Head;

enum FILE_TYPE
{
	FILEFORMAT_C,
	FILEFORMAT_H,
	FILEFORMAT_BMP,
	FILEFORMAT_PNG,
	FILEFORMAT_JPG,
	FILEFORMAT_MP3,
	FILEFORMAT_MP4,
	FILEFORMAT_UNKOWN
};
enum CMD_TYPE
{
	stop, //0
	seek0,
	seek1,
	mute0,
	mute1,
	quit,
	nextvideo,
	prevideo,
};
char *cmd[] = {"pause\n", "seek -5\n", "seek 5\n", "mute 0\n", "mute 1\n", "q", "nextvideo", "prevideo"};

/*
	通过filename指定的文件名的后缀名判断此文件的类型
*/
enum FILE_TYPE Judge_Format(const char *filename)
{
	//文件名的最后一个元素的下标
	int len = strlen(filename) - 1;

	//从文件名的后面开始往前找第一个"."
	while (len)
	{
		if (*(filename + len) == '.')
		{
			break;
		}
		len--;
	}

	if (!strcmp(filename + len, ".bmp"))
	{
		return FILEFORMAT_BMP;
	}
	else if (!strcmp(filename + len, ".png"))
	{
		return FILEFORMAT_PNG;
	}
	else if (!strcmp(filename + len, ".jpg"))
	{
		return FILEFORMAT_JPG;
	}
	else if (!strcmp(filename + len, ".mp3"))
	{
		return FILEFORMAT_MP3;
	}
	else if (!strcmp(filename + len, ".mp4"))
	{
		return FILEFORMAT_MP4;
	}
	else
	{
		return FILEFORMAT_UNKOWN;
	}
}

/*
	将dirname指定的路径下的所有的文件的文件名以及文件对应的类型
	扫描出来之后保存到一条双向带头结点的链表中去
	并将这条带头结点双向链表返回
*/
Head *Scanf_File(const char *dirname)
{
	//先将头结点创建出来
	Head *list = malloc(sizeof(*list));
	list->first = list->last = NULL;

	//将dirname指定的目录打开后读取目录项
	DIR *dir = opendir(dirname);
	if (dir == NULL)
	{
		perror("Opendir Failed");
		exit(-1);
	}

	struct dirent *dirp = NULL;
	//读取目录项
	while (dirp = readdir(dir))
	{
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
		{
			continue;
		}

		//新开辟一个结点保存这个目录项
		Node *pnew = malloc(sizeof(*pnew));
		sprintf(pnew->filename, "%s/%s", dirname, dirp->d_name);
		pnew->next = pnew->prev = NULL;
		pnew->file_type = Judge_Format(dirp->d_name);

		//插入链表中去
		if (list->first == NULL)
		{
			list->first = list->last = pnew;
		}
		else
		{
			list->last->next = pnew;
			pnew->prev = list->last;
			list->last = pnew;
		}
	}
	return list;
}

/*
	获取p结点后的下一首歌
*/
Node *Get_Next_Song(Head *list, Node *p)
{

	p = p->next;
	if (p == NULL)
	{
		p = list->first;
	}
	while (p->file_type != FILEFORMAT_MP3 && p->file_type != FILEFORMAT_MP4)
	{
		p = p->next;
		if (p == NULL)
		{
			p = list->first;
		}
	}
	printf("正在播放%s..........................\n", p->filename);
	return p;
}

/*
	获取p结点后的上一首歌
*/
Node *Get_Prev_Song(Head *list, Node *p)
{
	p = p->prev;
	if (p == NULL)
	{
		p = list->last;
	}
	while (p->file_type != FILEFORMAT_MP3 && p->file_type != FILEFORMAT_MP4)
	{
		p = p->prev;
		if (p == NULL)
		{
			p = list->last;
		}
	}
	printf("正在播放%s..........................\n", p->filename);
	return p;
}

void Print_List(Head *list)
{
	Node *p = list->first;
	while (p)
	{
		printf("%s\n", p->filename);
		p = p->next;
	}
}
//创建管道并获取文件描述符
int creatfifo()
{
	if (mkfifo(FIFO_PATH, 0664) == -1)
	{
		if (errno != EEXIST)
		{
			perror("fail to mkfifo");
			return -1;
		}
	}
	int fd;
	fd = open(FIFO_PATH, O_RDWR);
	if (fd == -1)
	{
		perror("faile to open");
		exit(1);
	}
	return fd;
}

int gettouchcmd()
{
	while (1)
	{
		int x = get_x();
		int y = get_y();

		if (x > 720 * 1.28 && x < 800 * 1.28 && y > 0 * 1.25 && y < 80 * 1.25)
		{
			return 0;
		}
		else if (x > 720 * 1.28 && x < 800 * 1.28 && y > 80 * 1.25 && y < 160 * 1.25)
		{
			return 1;
		}
		else if (x > 720 * 1.28 && x < 800 * 1.28 && y > 160 * 1.25 && y < 240 * 1.25)
		{
			return 2;
		}
		else if (x > 720 * 1.28 && x < 800 * 1.28 && y > 240 * 1.25 && y < 320 * 1.25)
		{
			return 5;
		}
		else if (x > 720 * 1.28 && x < 800 * 1.28 && y > 320 * 1.25 && y < 400 * 1.25)
		{
			return 6;
		}
		else if (x > 720 * 1.28 && x < 800 * 1.28 && y > 400 * 1.25 && y < 480 * 1.25)
		{
			return 7;
		}
	}
}

//根据指令来操作
char *switchcmd(int intcmd)
{
	// 	pause, //0
	// seek0,
	// seek1,
	// mute0,
	// mute1,
	// quiet,
	switch (intcmd)
	{

	case stop:
		return cmd[0];
		break;
	case seek0:
		return cmd[1];
		break;
	case seek1:
		return cmd[2];
		break;
	case mute0:
		return cmd[3];
		break;
	case mute1:
		return cmd[4];
		break;
	case quit:
		return cmd[5];
		break;
	case nextvideo:
		return cmd[6];
		break;
	case prevideo:
		return cmd[7];
		break;
	default:
		printf("获取cmd失败");
		break;
	}
}

//启动显示界面
void video_Init()
{
	Lcd_Init();
	//显示图片

	Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/zhuye.bmp");
	int x = get_x();
	int y = get_y();
	if (x > 0 && x < 400 && y > 0 && y < 120)
	{

		Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/cmd.bmp");
	}
}

int main(int argc, char **argv)
{
	if (argc != 1)
	{
		//输入一个文件名
		fprintf(stderr, "Usage : %s <MP4_PATH>\n", argv[0]);
		exit(-1);
	}
	//屏幕初始化
	video_Init();
	//创建一个管道
	int ffd = creatfifo();
	printf("ffd=%d\n", ffd);

	//搜索一个目录下面的所有的MP3/MP4文件的
	//带路径的文件名保存到一条双向链表中去
	Head *list = Scanf_File("/root/user/lifeiyang/mp34file");
	Print_List(list);
	char buf[256] = {0};
	struct node *p = list->first;
	p = Get_Next_Song(list, p);
	printf("line=%d---------------------------\n", __LINE__);
	while (1)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("Fork Failed");
			exit(-1);
		}
		else if (pid == 0) //子进程
		{

			char fifoname[256] = {0};
			sprintf(fifoname, "file=%s", FIFO_PATH);

			//通过管道给儿子传送命令
			int ret = read(ffd, fifoname, strlen(fifoname));
			//exec-->让儿子去播放mp3/mp4
			char *argv[] = {"mplayer", "-slave", "-quiet", "-input", fifoname, "-zoom", "-x", "720", "-y", "480", p->filename, NULL};
			printf("line=%d---------------------------\n", __LINE__);
			execvp("mplayer", argv);
		}
		else //父进程
		{

			//根据指令来给儿子传指令
			int intcmd = gettouchcmd();
			char *s = switchcmd(intcmd);
			if (intcmd == 7 || intcmd == 8)
			{
				printf("line=%d---------------------------\n", __LINE__);
				if (intcmd == 7)
				{
					printf("line=%d---------------------------\n", __LINE__);
					p = Get_Next_Song(list, p);
					if (p->file_type == FILEFORMAT_MP3)
					{
						Display_Bmp(0, 0, "");
					}
					printf("line=%d---------------------------\n", __LINE__);
				}
				else
				{
					printf("line=%d---------------------------\n", __LINE__);
					p = Get_Next_Song(list, p->prev->prev);
					if (p->file_type == FILEFORMAT_MP3)
					{
						printf("3bmp***********************************\n");
						Display_Bmp(0, 0, "/root/user/lifeiyang/project/bmpprofile/bmp/lod.bmp");
					}
					printf("line=%d---------------------------\n", __LINE__);
				}

				write(ffd, s, strlen(buf));

				printf("line=%d---------------------------\n", __LINE__);
				//等儿子播放完一首歌之后自动播放下一首
				wait(NULL);
				p = Get_Next_Song(list, p);
				printf("line=%d---------------------------\n", __LINE__);
			}
		}
	}
