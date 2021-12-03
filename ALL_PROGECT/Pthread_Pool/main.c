#include "pthreadpool.h"
#include "cpdir.h"

/*
	目录的拷贝
*/
int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "%s <src_dir> <dest_dir>\n", argv[0]);
		exit(-1);
	}

	Pthread_pool *pool = malloc(sizeof(*pool)); //定义一个线程池
	Init_Pool(pool, 20);						//对线程池的初始化

	Add_Threads(pool, 10);
	Delete_Threads(pool, 2);

	//将目录argv[1]以及目录下所有的东西保持原有架构的基础上拷贝到argv[2]目录下
	Cp_Dir(pool, argv[1], argv[2]);

	Destory_Pool(pool); //销毁线程池

	return 0;
}
