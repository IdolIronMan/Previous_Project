#ifndef __CPDIR_H__
#define __CPDIR_H__

#include "head.h"
#include "pthreadpool.h"

//代表最大的路径名的长度
#define MAXNAME_LEN 4096

#define MAX_LEN 1024

//用来保存要拷贝的源文件和目标文件的路径
typedef struct cpfile
{
	char src[MAXNAME_LEN];		//用来保存要拷贝的源文件
	char dest[MAXNAME_LEN];		//用来保存目标文件的路径
}CpFile;

/*
	普通函数(拷贝任务)
*/
void Cp_File(void *arg);

/*
	将目录src_dir以及目录下所有的东西保持原有架构的基础上拷贝到dest_dir目录下
*/
void Cp_Dir(Pthread_pool *pool,char *src_dir,char *dest_dir);

#endif

