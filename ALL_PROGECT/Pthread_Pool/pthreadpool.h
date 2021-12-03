#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include "head.h"

//线程池中最大的线程数量
#define MAX_THREADS 100
//线程池中最大的任务数量
#define MAX_TASKS 10000

//任务节点
typedef struct task_list
{
	//do_task是一个指针，指向任务函数
	//所谓的执行一个任务实际上就是去执行一个函数
	//这个函数实际上就是我们的拷贝任务函数
	void (*do_task)(void *);

	//任务函数需要的参数
	void *arg;

	//下一个任务
	struct task_list *next;
} Task;

//线程池
typedef struct pthread_poolc
{
	//线程池的实现按照项目的不同也会有所不同，大体如下：
	//因为“任务队列”是一种共享资源，所以需要互斥访问
	//需要线程互斥锁来保护“任务队列”
	pthread_mutex_t lock;

	//在任务队列中没有任务的时候，线程池中的线程需要陷入休眠
	//线程条件变量用来表示任务队列中是否有任务
	pthread_cond_t cond;

	//任务队列(链表)指向第一个需要执行的任务
	//所有的线程都从这个任务链表中取任务去执行
	//并不是线程主动去取任务，而是由任务调配函数分配任务给线程去执行
	Task *task_list;

	//指向线程池中所有线程ID的数组
	pthread_t *tids;

	//线程池中正在服役的线程数->线程的个数
	unsigned int active_threads;

	//线程池任务队列最大的任务数量
	unsigned int max_waiting_tasks;

	//线程池中任务队列当前的任务数量
	unsigned int cur_waiting_tasks;

	//是否退出程序
	bool shutdown;
} Pthread_pool;

/*
	Init_Pool:线程池初始化函数
		初始化pool指定的线程池,线程池中有thread_num个线程
		返回值：成功返回0，失败的返回-1
*/
int Init_Pool(Pthread_pool *pool, unsigned int thread_num);

/*
	销毁线程池
		销毁线程池之前需要保证所有的任务都已经完成
*/
int Destory_Pool(Pthread_pool *pool);

/*
	Add_Task:给任务队列增加任务
		把fun_task指向的函数和fun_arg指向的函数参数保存到一个任务结点中去
		同时将任务结点添加到pool所表示的线程池的任务队列中去
*/
int Add_Task(Pthread_pool *pool, void (*fun_task)(void *), void *fun_arg);

/*
	往线程池中添加线程
		@pool:你要增加线程的线程池的指针
		@add_threads_num:你要增加的线程数量
		@返回值:成功返回增加后线程池总线程数,失败返回-1
*/
int Add_Threads(Pthread_pool *pool, unsigned int add_threads_num);

/*
	从线程池中删除线程
		@pool:要删除线程的线程池的地址
		@delete_threads_num:要删除的线程的数量
		@返回值:成功返回删除后线程池的总数,失败返回-1
*/
int Delete_Threads(Pthread_pool *pool, unsigned int delete_threads_num);

#endif
