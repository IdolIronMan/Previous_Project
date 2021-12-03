#ifndef __HEAD_H__
#define __HEAD_H__

#include <stdio.h>
#include <sys/socket.h> //socket
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h> //sockaddr_in

#include <arpa/inet.h> //htons inet_addr 地址转换和字节区转换函数
#include <unistd.h>    //close

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

//进程所需头文件
#include <sys/wait.h>
#include <signal.h>

#include <dirent.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>

#endif