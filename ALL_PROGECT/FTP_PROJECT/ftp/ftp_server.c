#include "head.h"
/*
        客户端功能：
            负责链接服务器后，向服务器发送请求，并等待服务器的回应，同时处理服务回应的
            数据。

          
*/
#define MAX_FILENAMESIZE 4096
#define PACKET_SIZE 8192

#define MAX_LEN 1024
#define ROOTPATH "/home/ubuntu/C_code/brouadcast"

typedef enum ftp_cmd_no
{
    FTP_LS = 0,
    FTP_GET, //发送get指令 1
    FTP_PUT, // 2
    FTP_BYE, //断开连接 3
    UNKOWN_CMD = 99
} FTP_CMD;

/*
    任务结构体,每一个都保存了客户端的信息
*/
typedef struct
{
    struct sockaddr_in addr;
    int acceptfd;
} MSG;

/*
    组包函数，将要发送的数据放到发送数据包里，参数为一个字符串
    格式如下：
      0xC0 pkg_len cmd_no resp_len result res_conent 0xC0
		
			包头:可以以0xC0作为包头(数据包中的第一个字节，规定每一个数据包都以0xC0
			为开头)，这个包头占1个字节，在实际应用中，为了保证包头的唯一性，一般会
			设置多个字节。
			在连续的16个bit中，0xEB 0x90是出现概率最低的组合。
			
			pkg_len：占4个字节，表示这个数据包的总长度(不包括包头和包尾)，以小端模式
			进行存储(先存低字节)
			
			cmd_no:占4个字节，表示这个数据包的命令号，小端模式存储	

			resp_len：占4个字节，回复的内容的长度result + resp_conent
				
			result:占1个字节，表示的是执行命令成功或者失败
				为0表示成功，为1表示失败

			resp_conent：
				回复的内容
				失败：
					失败可以回一个错误码，这个错误码由程序猿自己定义。
					此时回复的内容就是错误的原因。
				成功：
					ls:服务器主目录下所有的目录项的名称，每一个名称以空格隔开
					get:先把文件的大小发送过去，下一次再把文件内容发送过去，占4个字节



*/

/*
    将接收到的数据包中的命令解析出来

*/
int Get_Repackcmd(unsigned char *cmd)
{
    return cmd[5];
}

/*
    将数据包准备好发送
        @参数：filename：
        resp:要发送的包名
        cmdtype:命令
*/
void prepar_packet(unsigned char *filename, unsigned char *resp, int cmdtype, int filesize)
{
    int i = 0;
    unsigned char *filenames = filename; //1.所有的文件名已经就绪保存在此数组中  2.保存要传输的文件名的
    int pkg_len = 0;
    int file_size = filesize;
    FTP_CMD cmd_no = cmdtype;
    switch (cmdtype) //命令号   FTP_LS = 0   FTP_GET = 1
    {
    case FTP_LS:

        pkg_len = 10 + strlen(filenames);
        cmd_no = cmdtype;

        resp[i++] = 0xC0; //包头
        //八字节
        resp[i++] = pkg_len & 0xFF;
        resp[i++] = (pkg_len >> 8) & 0xFF;
        resp[i++] = (pkg_len >> 16) & 0xFF;
        resp[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        resp[i++] = cmd_no & 0xFF;
        resp[i++] = (cmd_no >> 8) & 0xFF;
        resp[i++] = (cmd_no >> 16) & 0xFF;
        resp[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        strcpy(resp + i, filenames);
        resp[i + strlen(filenames) + 1] = 0xC0; //包尾
                                                // printf("cmdtype=%d\n", cmdtype);
                                                // printf("i=%d\n", i);
                                                //printf("packet:%s\n", resp + i);        //文件名放在第10字节
                                                //注意这个break;
        break;

    case FTP_GET:
        //服务器将客户端要的文件名保存在包里，并返回文件的大小
        pkg_len = 12 + strlen(filenames);

        cmd_no = cmdtype;

        resp[i++] = 0xC0; //包头
        //八字节
        resp[i++] = pkg_len & 0xFF;
        resp[i++] = (pkg_len >> 8) & 0xFF;
        resp[i++] = (pkg_len >> 16) & 0xFF;
        resp[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        resp[i++] = cmd_no & 0xFF;
        resp[i++] = (cmd_no >> 8) & 0xFF;
        resp[i++] = (cmd_no >> 16) & 0xFF;
        resp[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        resp[i++] = file_size & 0xFF; //文件的大小 第10字节
        resp[i++] = (file_size >> 8) & 0xFF;
        resp[i++] = (file_size >> 16) & 0xFF;
        resp[i++] = (file_size >> 24) & 0xFF; //回复的内容的长度，以小端模式存储
        strcpy(resp + i, filenames);
        resp[i + strlen(filenames) + 1] = 0xC0; //包尾

        // printf("i=%d\n", i);
        // printf("cmdtype=%d\n", cmdtype);
        // printf("file_size=%d\n", file_size);
        // printf("packet:%s\n", resp + i);

        break;
    case FTP_PUT:
        //回应客户端，收到了文件
        pkg_len = 12 + filesize;

        cmd_no = cmdtype;

        resp[i++] = 0xC0; //包头
        //八字节
        resp[i++] = pkg_len & 0xFF;
        resp[i++] = (pkg_len >> 8) & 0xFF;
        resp[i++] = (pkg_len >> 16) & 0xFF;
        resp[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        resp[i++] = cmd_no & 0xFF;
        resp[i++] = (cmd_no >> 8) & 0xFF;
        resp[i++] = (cmd_no >> 16) & 0xFF;
        resp[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        resp[i++] = file_size & 0xFF; //文件的大小 第10字节
        resp[i++] = (file_size >> 8) & 0xFF;
        resp[i++] = (file_size >> 16) & 0xFF;
        resp[i++] = (file_size >> 24) & 0xFF; //回复的内容的长度，以小端模式存储
        strcpy(resp + i, filenames);
        resp[i + strlen(filenames) + 1] = 0xC0; //包尾

        // printf("i=%d\n", i);
        // printf("cmdtype=%d\n", cmdtype);
        // printf("file_size=%d\n", file_size);
        // printf("packet:%s\n", resp + i);

        break;

    case FTP_BYE:
        pkg_len = 8;

        cmd_no = cmdtype;

        resp[i++] = 0xC0; //包头
        //八字节
        resp[i++] = pkg_len & 0xFF;
        resp[i++] = (pkg_len >> 8) & 0xFF;
        resp[i++] = (pkg_len >> 16) & 0xFF;
        resp[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        resp[i++] = cmd_no & 0xFF;
        resp[i++] = (cmd_no >> 8) & 0xFF;
        resp[i++] = (cmd_no >> 16) & 0xFF;
        resp[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        resp[i] = 0xC0; //包尾
        // printf("cmdtype=%d\n", cmdtype);
        // printf("i=%d\n", i);

        break;

    default:
        pkg_len = 10;
        cmd_no = cmdtype;

        resp[i++] = 0xC0; //包头
        //八字节
        resp[i++] = pkg_len & 0xFF;
        resp[i++] = (pkg_len >> 8) & 0xFF;
        resp[i++] = (pkg_len >> 16) & 0xFF;
        resp[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        resp[i++] = cmd_no & 0xFF;
        resp[i++] = (cmd_no >> 8) & 0xFF;
        resp[i++] = (cmd_no >> 16) & 0xFF;
        resp[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        resp[i] = 0xC0; //包尾

        break;
    }
}
/*
        接收客户端要上传的文件，接收到服务器根目录下
*/
int sendrep(int sock, unsigned char *putfile, unsigned char *buf)
{
    //要发送的数据包
    unsigned char resp[PACKET_SIZE] = {0};
    //将文件名和文件名长度发给客户端
    prepar_packet(putfile, resp, Get_Repackcmd(buf), buf[1]);

    //发送允许上传文件
    ssize_t ret = send(sock, resp, sizeof(resp), 0);
    if (ret < 0)
    {
        perror("fail to send resp");
        return -1;
    }
    printf("--------------\n");
    return 0;
}

/*
        将客户端指定的文件下载到客户端目录下
*/
int download(int sock, unsigned char *pathname, unsigned char *file, unsigned char *buf)
{
    printf("download file:%s\n", file);
    //打开源目录src_dir读取目录项
    DIR *dir = opendir(pathname);
    if (dir == NULL)
    {
        fprintf(stderr, "Open %s Failed\n", pathname);
        return -1;
    }

    int ret = -1;
    //读取目录项
    struct dirent *dirp = NULL;

    //读取目录项
    while (dirp = readdir(dir))
    {
        //对于 . 和 .. 目录直接略过不能考虑
        if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
        {
            continue;
        }
        if (strcmp(file, dirp->d_name) != 0) //如果当前文件目录项不是指定文件则继续找
        {
            continue;
        }
        else //来到这肯定已经找到了文件了
        {
            //获取到这个文件目录项的完整的带路径名
            unsigned char filename[512] = {0};
            sprintf(filename, "%s/%s", pathname, dirp->d_name);

            struct stat st;
            //获取此目录项的属性
            ret = lstat(filename, &st);
            if (ret == -1)
            {
                perror("Stat Failed");
                continue;
            }
            //判断filename是一个目录还是一个普通文件
            if (S_ISDIR(st.st_mode)) //暂不考虑下载目录
            {
                continue;
            }
            else if (S_ISREG(st.st_mode))
            {
                //将这个文件打开
                int fd = open(filename, O_RDWR);
                if (fd == -1)
                {
                    fprintf(stderr, "Open %s filed\n", filename);
                    return -1;
                }
                int filesize = st.st_size; //获取到文件的大小

                //send(sock, &filename, sizeof(filesize), 0); //先把文件大小发送过去

                unsigned char resp[PACKET_SIZE] = {0}; //要发送的包
                                                       //将文件的名字大小和命令都封装成包
                prepar_packet(dirp->d_name, resp, Get_Repackcmd(buf), filesize);
                int ret = send(sock, resp, sizeof(resp), 0);
                if (ret < 0)
                {
                    perror("fail to send resp to client");
                    return -1;
                }
                //开始将文件下载到客户端
                //将文件内容分多次发送给客户端
                unsigned char buf[MAX_LEN] = {0};
                while (1)
                {
                    int ret = read(fd, buf, sizeof(buf));
                    if (ret == 0)
                    {
                        //文件读取完毕跳出while
                        break;
                    }
                    else if (ret < 0)
                    {
                        perror("Read File Failed");
                        return -1;
                    }
                    else
                    {
                        //将读取到的文件内容发送给客户端
                        ret = write(sock, buf, ret);
                        if (ret < 0)
                        {
                            perror("Write File Failed");
                            return -1;
                        }
                    }
                }
                //执行完拷贝
                return 0;
            }
        }
    }

    //来到这有就是没找到
    if (dirp == NULL)
    {
        printf("Don't have this file\n");
        unsigned char resp[PACKET_SIZE] = {0};            //要发送的包
        prepar_packet(file, resp, Get_Repackcmd(buf), 0); //客户端通过filesize是否为0 判断服务器是否有这个文件
        // unsigned char resp[MAX_LEN] = {"Server don't have this file,place input again\n"};

        int ret = send(sock, resp, sizeof(resp), 0);
        if (ret < 0)
        {
            perror("fail to send(downfile)\n");
            return -1;
        }
    }
    //关闭打开的目录
    closedir(dir);
    return 0;
}

/*
       将当前目录下的所有文件名保存到一个数组中，每保存一个用\t隔开
       @返回值：成功返回0,失败返回-1。
*/
int Show_Filename(int sock, const char *pathname, unsigned char *buf)
{
    //ftp服务器的根目录下的所有文件名
    unsigned char all_filename[MAX_FILENAMESIZE] = "";

    //将pathname目录打开
    DIR *dir = opendir(pathname);
    if (dir == NULL)
    {
        fprintf(stderr, "Open %s Failde\n", pathname);
        return -1;
    }

    int ret = -1;
    struct dirent *dirp = NULL; //读取目录项

    while (dirp = readdir(dir))
    {
        //对于 . 和 .. 目录直接略过不能考虑
        if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
        {
            continue;
        }

        //获取到目录项name,将name接到all-filename中去
        strcat(all_filename, dirp->d_name);
        strcat(all_filename, "    ");
    }
    //全部文件的名字已经拷贝到all_filename中去了
    unsigned char resp[PACKET_SIZE] = {0};
    //组包
    prepar_packet(all_filename, resp, Get_Repackcmd(buf), 0);

    //printf("resp=%s\n", resp + 9); //测试
    //向客户端发送数据
    if (send(sock, resp, PACKET_SIZE, 0) < 0)
    {
        perror("fail to send");
        return -1;
    }

    closedir(dir);
    return 0;
}

/*
        解析数据包，查看数据包中的命令,并将数据包组好发送给客户端
*/
int Analyz_packet(int sock, unsigned char *buf, struct sockaddr_in addr)
{
    int ret = -1;
    //根据第5个字节进行命令解析
    if (buf[5] == FTP_LS)
    {
        ret = Show_Filename(sock, ROOTPATH, buf);
        if (ret < 0)
        {
            perror("fail to show_filename");
            return -1;
        }
        printf("[ IP : %s ] [ PORT : %d ] get allfile\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    else if (buf[5] == FTP_GET)
    {
        //将客户端要获取的文件名取出来
        unsigned char getfile[512] = "";
        strncpy(getfile, buf + 9, buf[1]); //从第9字节开始就是客户端想要获取的文件名
        printf("[ IP : %s ] [ PORT : %d ]  want to get file: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), getfile);

        //下载服务器指定的文件
        ret = download(sock, ROOTPATH, getfile, buf);
        if (ret < 0)
        {
            perror("fail to download");
            return -1;
        }
    }
    else if (buf[5] == FTP_PUT)
    {
        // signal(SIGALRM, NULL);

        //将客户端要上传的文件名取出来
        unsigned char putfile[512] = "";
        strncpy(putfile, buf + 9, buf[1]); //从第9字节开始就是客户端想要上传的文件名
        printf("[ IP : %s ] [ PORT : %d ]  want to put file: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), putfile);

        //发送允许上传包给客户端
        ret = sendrep(sock, putfile, buf);
        if (ret < 0)
        {
            perror("fail to sendrep");
            return -1;
        }

        unsigned char filename[512] = {0};
        sprintf(filename, "%s/%s", ROOTPATH, putfile);
        //printf("putfilename=%s\n", filename);
        //等待客户端发送文件
        if (buf[1] == 0) //根据put的文件名长度来判断客户端是否有这个文件
        {
            return 0;
        }

        int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0664);
        if (fd < 0)
        {
            perror("fail to open\n");
            return -1;
        }
        //printf("创建文件成功，fd=%d\n", fd);

        unsigned char buf1[MAX_LEN] = {0};
        while (1)
        {
            //alarm(8); //超时则将文件删除
            int ret = read(sock, buf1, sizeof(buf1));
            if (ret < 1024) //最后一次传输，传完就跳出循环
            {
                write(fd, buf1, ret);
                break;
            }
            else if (ret < 0)
            {
                perror("Read File Failed");
                return -1;
            }
            else
            {
                //将读取到的文件内容写入文件
                int w = write(fd, buf1, ret);
                if (w < 0)
                {
                    perror("fail to write");
                    break;
                }
            }
        }
        printf("%s load success!\n", putfile);
        close(fd);
        return 0;
    }
    else if (buf[5] == FTP_BYE)
    {
        unsigned char resp[PACKET_SIZE] = {0};
        prepar_packet(NULL, resp, FTP_BYE, 0);
        // 不能调用exit(1);
        //将当前客户端和服务器断开，但不影响其他已连接的客户端
        int ret = send(sock, resp, PACKET_SIZE, 0);
        if (ret < 0)
        {
            perror("fail to send FTP_BYE packet\n");
            return -1;
        }
        pthread_exit(NULL);
    }
    else
    {
        unsigned char err[50] = {"Unknow command,place resend again!"};
        send(sock, err, sizeof(err), 0);
    }

    return 0;
}

/*
    线程函数，每当有一个客户端和我相连，我就去开启一个线程发送数据
*/
void *pthread_fun(void *arg)
{
    //将MSG 结构体解析出来
    MSG msg = *(MSG *)arg;
    //打印连接的客户端的信息
    printf("[ IP : %s ] [ PORT : %d ] CONNECT\n", inet_ntoa(msg.addr.sin_addr), ntohs(msg.addr.sin_port));
    //定义接收数据包
    unsigned char buf[PACKET_SIZE] = "";
    ssize_t bytes;
    //接收数据
    while (1)
    {
        //阻塞等待接收客户端消息
        if ((bytes = recv(msg.acceptfd, buf, PACKET_SIZE, 0)) == -1)
        {
            perror("fail to recv");
            exit(1);
        }

        //检查包是否错误
        if (buf[0] != 0xc0)
        {
            unsigned char err[50] = {"Unknow data,please check your packet!"};
            send(msg.acceptfd, err, sizeof(err), 0);
            continue;
        }
        else //包头正确，分析包头内容
        {
            Analyz_packet(msg.acceptfd, buf, msg.addr);
            printf("wait client to send massage.....\n");
        }
    }
}

/*
	创建服务器的套接字，根据IP地址字符串和端口号字符串创建套接字
		@返回值：成功返回套接字,失败返回-1
*/
int Create_Server_Socket(const char *ip, const char *port)
{
    //创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Create Socket Failed!");
        return -1;
    }

    //2.指定服务器(本机)的IP地址和端口号
    struct sockaddr_in serveraddr = {0}; //初始化为0

    socklen_t addrlen = sizeof(serveraddr);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(port));
    serveraddr.sin_addr.s_addr = inet_addr(ip);

    //设置端口重用要在socket和bind之间使用
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on)); //允许地址重用
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (void *)&on, sizeof(on)); //允许端口重用

    //3.将套接字和服务器的网络信息结构体进行绑定
    if (bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("faile to bind serveraddr");
        exit(1);
    }

    //4.将套接字设置为监听状态
    if (listen(sockfd, 10) < 0)
    {
        perror("fail to listen");
        exit(1);
    }

    return sockfd;
}

int main(int argc, char const *argv[])
{

    if (argc < 3)
    {
        fprintf(stderr, "Usage:%s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }
    //创建服务器的套接字
    int sockfd = Create_Server_Socket(argv[1], argv[2]);
    struct sockaddr_in clientaddr;
    int acceptfd;
    socklen_t addrlen = sizeof(clientaddr);

    printf("FEIYANG FTP SERVER LOGIN SUCCESSFUL!\n");
    while (1)
    {
        //5.阻塞等待客户端的连接请求
        if ((acceptfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen)) < 0)
        {
            perror("fail to accept");
            exit(1);
        }

        //一旦有客户端连接上，则开辟子线程与之连接,并将信息保存到结构体中作参数传给线程函数
        MSG msg;
        msg.addr = clientaddr;
        msg.acceptfd = acceptfd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, pthread_fun, (void *)&msg) != 0)
        {
            perror("fail to create pthread");
            exit(1);
        }

        //将每一个线程设置为分离态
        pthread_detach(thread);
    }

    return 0;
}
