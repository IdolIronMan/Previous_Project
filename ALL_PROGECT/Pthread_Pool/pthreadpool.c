#include "pthreadpool.h"

/*
	线程的清理函数：作用就是当线程意外退出时进行解锁操作->防止带锁退出而造成死锁
*/
void Handler(void *arg)
{
	pthread_mutex_unlock((pthread_mutex_t *)arg);
}

/*
	任务调配函数(线程函数)
		所有的线程刚开始都会执行此函数，此函数会不断的从线程池中的任务队列
	中获取任务(摘任务结点)然后交由线程池中的线程去执行
		arg:表示的是线程池的指针，在线程池中有任务队列，任务队列中有任务结点
		每一个任务结点上都包含了线程要执行的任务(函数)的地址和执行函数需要的参数
*/
void *Routine(void *arg)
{
	//将线程池先解析出来
	Pthread_pool *pool = (Pthread_pool *)arg;

	//将指向被摘下来的那个任务
	Task *p = NULL;

	while (1)
	{
		//在上锁之前可以设置一个线程退出清理函数，当线程被干掉的时候自动执行
		//我指定的清理函数(一般是为了防止带锁退出)
		//&pool->lock是Handler函数的实参
		pthread_cleanup_push(Handler, (void *)&pool->lock);

		//因为任务队列是共享资源所以需要先上锁
		//获取线程互斥锁，上锁
		pthread_mutex_lock(&pool->lock);

		//当条件不满足时(当任务队列中没有任务的时候)
		while (pool->cur_waiting_tasks == 0 && pool->shutdown == 0)
		{
			//当前线程陷入休眠
			pthread_cond_wait(&pool->cond, &pool->lock);
			//线程被唤醒的条件有两个：1.有新任务的时候 2.要销毁线程的时候
		}

		//当任务队列中没有任务的时候并且要退出时
		if (pool->cur_waiting_tasks == 0 && pool->shutdown == 1)
		{
			//线程退出之前需要解锁
			pthread_mutex_unlock(&pool->lock);
			pthread_exit(NULL);
		}

		//当条件满足的时候，将任务结点从任务链表中取下来(将任务结点从链表中摘下来)

		//找到任务链表中的第二个结点，因为第一个任务结点是空任务结点，类似于头结点，只是为了方便添加结点而设
		p = pool->task_list->next;

		//将任务结点摘下来
		pool->task_list->next = p->next;
		p->next = NULL;
		//同时可用的任务数量减1
		pool->cur_waiting_tasks--;

		//获取线程互斥锁，解锁
		pthread_mutex_unlock(&pool->lock);

		//来到这说明清理线程退出函数不执行
		pthread_cleanup_pop(0);

		//执行拷贝任务的时候不能被取消，所以需要设置线程属性为不可被取消
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

		//按照任务结点中的任务去执行即可
		(p->do_task)(p->arg);

		//执行完任务之后设置线程属性为可被取消
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

		//释放任务结点
		free(p);
	}
}

/*
	Init_Pool:线程池初始化函数
		初始化pool指定的线程池,线程池中有thread_num个线程
		返回值：成功返回0，失败的返回-1
*/
int Init_Pool(Pthread_pool *pool, unsigned int thread_num)
{
	//初始化线程池结构体
	//初始化线程互斥锁
	pthread_mutex_init(&pool->lock, NULL);

	//初始化线程条件变量
	pthread_cond_init(&pool->cond, NULL);

	//创建一个任务结点并让task_list指向他
	//但是需要注意的是这个任务结点中并没有任务,只是为了方便任务的插入而存在的
	pool->task_list = malloc(sizeof(Task));
	pool->task_list->next = NULL;

	//将用来保存线程池中线程ID的空间开辟出来,注意要预留一些空间因为有可能会新增线程
	pool->tids = malloc(sizeof(pthread_t) * MAX_THREADS);
	if (pool->task_list == NULL || pool->tids == NULL)
	{
		perror("Malloc Memory Failed");
		return -1;
	}

	//线程池中正在服役的线程数量->线程池中总的线程的数量
	pool->active_threads = thread_num;

	//线程池任务队列最大的任务数量
	pool->max_waiting_tasks = MAX_TASKS;

	//线程池中任务队列当前的任务数量
	pool->cur_waiting_tasks = 0;

	//不退出程序
	pool->shutdown = false;

	//创建thread_num个线程同时要将所有创建出来的线程的ID要保存起来
	//要创建出来的线程去执行任务调配函数(线程函数)
	int i;
	for (i = 0; i < thread_num; i++)
	{
		//*(p+i) = p[i]  ----> p+i = &p[i]               //pool 参数是传给Rotine的实参  因为要想从线程池里取任务
		if (pthread_create(pool->tids + i, NULL, Routine, (void *)pool) != 0) //pthread_create(&pool->tids[i], NULL, Routine, (void *)pool)
		{
			perror("Create Thread Failed");
			return -1;
		}

		//打印调试信息                                       //这里是主线程的ID
		printf("[%lu] create tids[%d] : [%lu] is success!\n", pthread_self(), i, pool->tids[i]);
	}
}

/*
	销毁线程池
		销毁线程池之前需要保证所有的任务都已经完成
*/
int Destory_Pool(Pthread_pool *pool)
{
	//释放所有的空间等待任务执行完毕

	//表示要销毁线程池了
	pool->shutdown = true;

	//唤醒所有的线程
	pthread_cond_broadcast(&pool->cond);

	//利用join函数回收每一个线程
	int i;
	for (i = 0; i < pool->active_threads; i++)
	{
		int ret = pthread_join(pool->tids[i], NULL);
		if (ret == 0)
		{
			printf("Recover tids[%d] : %lu success!\n", i, pool->tids[i]);
		}
		else
		{
			printf("Recover tids[%d] : %lu failed!\n", i, pool->tids[i]);
		}
	}

	//销毁掉线程互斥锁
	pthread_mutex_destroy(&pool->lock);

	//销毁掉线程条件变量
	pthread_cond_destroy(&pool->cond);

	//这里是把之前创建的空任务结点释放
	if (pool->task_list)
	{
		free(pool->task_list); //释放空任务结点
	}

	free(pool->tids);

	free(pool);

	return 0;
}

/*
	Add_Task:给任务队列增加任务
		把fun_task指向的函数和fun_arg指向的函数参数保存到一个任务结点中去
		同时将任务结点添加到pool所表示的线程池的任务队列中去
*/
int Add_Task(Pthread_pool *pool, void (*fun_task)(void *), void *fun_arg)
{
	//需要将fun_task和fun_arg封装到一个任务结点(结构体)中去
	//新建一个新的任务结点去保存任务(函数)
	Task *new_task = malloc(sizeof(Task));
	if (new_task == NULL)
	{
		perror("Malloc Failed");
		return -1;
	}

	new_task->do_task = fun_task;
	new_task->arg = fun_arg;
	new_task->next = NULL;

	//在往pool指向的线程池的任务队列中添加任务的时候，需要上锁和
	pthread_mutex_lock(&pool->lock);

	//任务队列已达上限,则不能将任务结点插入到任务链表中去
	if (pool->cur_waiting_tasks >= MAX_TASKS)
	{
		//解锁
		pthread_mutex_unlock(&pool->lock);

		printf("Too many task,Add task failed!\n");

		free(new_task->arg);
		free(new_task);
		return -1;
	}

	//将任务结点以尾插法的方式插入到任务链表中去
	//先找到任务链表的尾结点的地址
	Task *find = pool->task_list;
	while (find->next)
	{
		find = find->next;
	}

	//尾插
	find->next = new_task; //new_task的next已经被置为空了
	pool->cur_waiting_tasks++;

	//解锁
	pthread_mutex_unlock(&pool->lock);

	//加入任务后要唤醒等待的线程
	pthread_cond_signal(&pool->cond);
	return 0;
}

/*
	往线程池中添加线程
		@pool:你要增加线程的线程池的指针
		@add_threads_num:你要增加的线程数量
		@返回值:成功返回增加后线程池总线程数,失败返回-1
*/
int Add_Threads(Pthread_pool *pool, unsigned int add_threads_num)
{
	if (add_threads_num == 0)
	{
		return pool->active_threads;
	}

	//用一个变量来保存增加后的总线程数
	int total_threads = pool->active_threads + add_threads_num;

	int i;
	//线程数目不能超过最大的线程数
	for (i = pool->active_threads; i < total_threads && i < MAX_THREADS; i++)
	{

		if (pthread_create(pool->tids + i, NULL, Routine, (void *)pool) != 0)
		{
			perror("Create Thread Failed");
			return -1;
		}
		pool->active_threads++;

		//打印调试信息
		printf("[%lu] create tids[%d] : [%lu] is success!\n", pthread_self(), i, pool->tids[i]);
	}
	return pool->active_threads;
}

/*
	从线程池中删除线程
		@pool:要删除线程的线程池的地址
		@delete_threads_num:要删除的线程的数量
		@返回值:成功返回删除后线程池的总数,失败返回-1
*/
int Delete_Threads(Pthread_pool *pool, unsigned int delete_threads_num)
{
	if (delete_threads_num == 0)
	{
		return pool->active_threads;
	}

	//计算保留的线程的数量
	int remain_thread = pool->active_threads - delete_threads_num;

	//我们的任务是减少线程而不是结束任务,所以必须至少保留一个线程
	remain_thread = (remain_thread > 0) ? remain_thread : 1;

	int i; //数组下标
	for (i = pool->active_threads - 1; i > remain_thread - 1; i--)
	{
		//将线程取消  成功返回0，失败返回非0
		int r = pthread_cancel(pool->tids[i]);
		if (r != 0)
		{
			printf("Pthread Cancal Failed");
		}
		//退出线程还要回收
		pthread_join(pool->tids[i], NULL);
		pool->active_threads--;
		printf("Cancel Threads [%lu]\n", pool->tids[i]);
	}
	return pool->active_threads;
}
