#include "cpdir.h"

/*
//原来是线程函数，现在变为普通函数
	普通函数(拷贝任务)
*/
void Cp_File(void *arg)
{
	//将源文件和目标文件解析出来
	CpFile *p = (CpFile *)arg;

	//以只读的方式打开源文件
	int fd_src = open(p->src, O_RDONLY);
	if (fd_src == -1)
	{
		fprintf(stderr, "Open %s Failed\n", p->src);
		goto cp_return;
	}

	//打开目标文件,如果目标文件存在则截短,如果不存在则创建
	int fd_des = open(p->dest, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd_des == -1)
	{
		fprintf(stderr, "Open %s Failed\n", p->dest);
		goto cp_return;
	}

	//将源文件中的内容读取到目标文件中去
	int ret;
	char buf[MAX_LEN] = {0};
	while (1)
	{
		ret = read(fd_src, buf, MAX_LEN);
		if (ret == 0)
		{
			break;
		}
		else if (ret > 0)
		{
			int w = write(fd_des, buf, ret);
			if (w != ret)
			{
				perror("Write Failed");
			}
		}
		else
		{
			perror("Read Failed");
			break;
		}
	}

cp_return:
	free(p); //p指向的那块空间是malloc出来的，如果不free就会造成内存泄露
	close(fd_des);
	close(fd_src);
	return;
}

/*
	将目录src_dir以及目录下所有的东西保持原有架构的基础上拷贝到dest_dir目录下
*/
void Cp_Dir(Pthread_pool *pool, char *src_dir, char *dest_dir)
{
	//打开源目录src_dir读取目录项
	DIR *dir = opendir(src_dir); //   /home/china/cs2110
	if (dir == NULL)
	{
		fprintf(stderr, "Open %s Failed\n", src_dir);
		return;
	}

	//获取到要复制的目录的目录名(不带路径的)
	char *dirname = basename(src_dir); //       cs2110

	//根据获取到的不带路径的目录名和目录路径合成目标目录(带路径的)
	char newdirname[MAXNAME_LEN] = {0};
	sprintf(newdirname, "%s/%s", dest_dir, dirname); //    /home/ubuntu/    +  cs2110
	//printf("%s\n",newdirname);

	//把目标目录创建出来
	mkdir(newdirname, 0777); //         创建好了一个目录   /home/ubuntu/cs2110
	dest_dir = newdirname;	 //        dest_dir现在也为     /home/ubuntu/cs2110

	//目录
	struct dirent *dirp = NULL;
	while (dirp = readdir(dir)) //取的是  /home/chian/cs2110下的第一个目录项 比如/home/china/cs2110/1.c
	{
		//不考虑当前目录和上一层目录  //防止死循环拷贝
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
		{
			continue;
		}

		//用来保存源目录下目录项的完整路径名
		char file_src[MAXNAME_LEN] = {0};
		sprintf(file_src, "%s/%s", src_dir, dirp->d_name); //dirp->d_name 为1.c
		//printf("file_src : %s\n",file_src);

		//获取目录项的属性
		struct stat st;
		lstat(file_src, &st);

		//如果是普通文件和链接文件直接复制
		if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode))
		{
			//获取到目标文件的完整路径名
			char file_dest[MAXNAME_LEN] = {0};
			sprintf(file_dest, "%s/%s", dest_dir, dirp->d_name); //为 /home/ubuntu/cs2110/1.c

			//将源文件的路径和目标文件的路径保存到结构体中并且将此结构体的地址传递给线程
			CpFile *cp = malloc(sizeof(*cp));
			strcpy(cp->src, file_src);
			strcpy(cp->dest, file_dest);
			printf("要拷贝的两个文件的文件名为:\n");
			printf("src:%s\n", cp->src);
			printf("dest:%s\n", cp->dest);
			printf("---------------------------------------------------------------------------\n");

			Add_Task(pool, Cp_File, cp);
			/*
			//开辟一个线程去对两个文件实现复制
			pthread_t tid;
			int res = pthread_create(&tid,NULL,Cp_File,(void *)cp);
			if(res == -1)
			{
				perror("Pthread Create Failed");
			}

			//等待执行拷贝任务的线程结束
			//pthread_join(tid,NULL);
			*/
		}
		else if (S_ISDIR(st.st_mode))
		{
			printf("要拷贝的目录为：\n");
			printf("src:%s\n", file_src);
			printf("dest:%s\n", dest_dir);
			printf("---------------------------------------------------------------------------\n");
			Cp_Dir(pool, file_src, dest_dir);
		}
	}

	closedir(dir);
	return;
}
