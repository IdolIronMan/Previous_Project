#include "head.h"

/*
	项目：实现一个简单的网络文件传输工具(FTP服务器)
		功能：通过网络传输，实现文件的跨设备传输，包含服务器和客户端。
		
		客户端功能：
			负责链接服务器后，向服务器发送请求，并等待服务器的回应，同时处理服务回应的数据。

            命令：
			1.	ls
				用来获取服务器目录下的文件信息(文件名)
				ls -l 
				
			2.	get file
				用来从服务器上获取file指定的文件
				如果服务器存在该文件，则服务器回复文件内容
				如果服务器不存在该文件，则回复错误码
				get file1 file2
				
			3.	put file
				用来往服务器上上传file指定的文件
				如果服务器愿意接收该文件，则后续发送文件内容
				如果服务器不愿意接收该文件，则不发送

			4.	bye
				用来告诉服务器，客户端即将断开连接
				服务器收到bye之后，关闭与客户端的连接套接字，并结束该客户端处理分支
    
*/
#define MAX_FILENAMESIZE 4096
#define PACKET_SIZE 8192

#define MAX_LEN 1024
typedef enum ftp_cmd_no
{
    FTP_LS = 0,
    FTP_GET, //发送get指令 1
    FTP_PUT, // 2
    FTP_BYE, //断开连接 3
    UNKOWN_CMD = 99
} FTP_CMD;

//  查找字符串s1中是否有s2字符串  返回flag 成功找到返回1，未找到返回0
int find(unsigned char *s1, unsigned char *s2, int flag)
{
    char *p, *q;              //两个指针分别指向两个字符串
    for (; *s1 != '\0'; s1++) //遍历字符串1
    {
        //先找到带匹配的字符的首地址
        if (*s2 == *s1)
        { /*判断字符串中是否有和要判断的字串首字符相同的字符*/
            flag = 1;
            p = s1; /* s1 p为第一个相同字符的地址*/
            q = s2;
            for (; *q != '\0';)
            { /*如果有则判断接下去的几个字符是否相同*/
                if (*q++ != *p++)
                {
                    flag = 0;
                    break;
                }
            }
        }
        if (flag == 1)
            break;
    }
    return (flag);
}

//获取当前目录下的所有文件名
int Show_Filename(unsigned char *all_filename, const char *pathname, unsigned char *buf)
{

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

    printf("all_filename=%s\n", all_filename); //测试

    closedir(dir);
    return 0;
}

/*
        发送ftp数据包给服务器。
    包格式  ：0xC0 pkg_len   cmd_no  arg_1  arg_2  0xC0
    	              4        4      4+2    4+2
  
*/
void prepar_packet(unsigned char *cmd, int cmdtype, unsigned char *filenames)
{

    int i = 0; //记录cmd的下标
    //printf("filenames=%s\n", filenames); //测试

    FTP_CMD cmd_no = cmdtype;
    //printf("cmdtype=%d\n", cmdtype);
    int file_len = 0; //文件大小
    int pkg_len = 0;  //数据包的总长度
    switch (cmdtype)  //命令号   FTP_LS = 0   FTP_GET = 1
    {
    case FTP_LS:
        pkg_len = 8;
        cmd[i++] = 0xC0; //baoto
        cmd[i++] = pkg_len & 0xFF;
        cmd[i++] = (pkg_len >> 8) & 0xFF;
        cmd[i++] = (pkg_len >> 16) & 0xFF;
        cmd[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        cmd[i++] = cmd_no & 0xFF;          //命令号  0
        cmd[i++] = (cmd_no >> 8) & 0xFF;
        cmd[i++] = (cmd_no >> 16) & 0xFF;
        cmd[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        cmd[i] = 0xC0; //包尾

        break;
        //get file.c 实现
    case FTP_GET:

        file_len = (filenames ? strlen(filenames) : 0); //数据包的总长度
        //printf("file_len=%d\n", file_len);

        cmd[i++] = 0xC0; //baoto
        //以下为8字节
        cmd[i++] = file_len & 0xFF; //第二个字节保存文件名的长度
        cmd[i++] = (file_len >> 8) & 0xFF;
        cmd[i++] = (file_len >> 16) & 0xFF;
        cmd[i++] = (file_len >> 24) & 0xFF; //包长度以小端模式存储
        cmd[i++] = cmd_no & 0xFF;           //命令号  1  第6字节
        cmd[i++] = (cmd_no >> 8) & 0xFF;
        cmd[i++] = (cmd_no >> 16) & 0xFF;
        cmd[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        if (filenames != NULL) //如果file不为空，则把文件名拷贝到包里去
        {                      //第10字节开始是文件名
            strcpy(cmd + i, filenames);
        }
        cmd[(i++) + file_len] = 0xC0; //包尾
        //printf("filenames=%s\n", cmd + i - 1); //-1 为了将包尾加上的长度减去

        break;

    case FTP_PUT:

        file_len = 0; //数据包的总长度
        //printf("file_len=%d\n", file_len);
        //ftp客户端的当前目录下的所有文件名
        unsigned char all_filename[MAX_FILENAMESIZE] = "";

        Show_Filename(all_filename, ".", cmd);
        //将要上传的文件 和当前 目录下文件进行比较，如果有则发送文件名长度，没有则文件名长度为0

        //strstr(str1,str2)用于判断str2是否为str1的子串
        //如果是则该函数返回str2在str1中首次出现的地址,否则返回NULL
        //用这个的话 如果你输入了一半文件名也会认为找到了

        if (1 == find(all_filename, filenames, 0))
        {
            //当前文件目录下有这个文件，设置文件名长度
            file_len = strlen(filenames);
        }

        printf("file_len=%d\n", file_len);
        cmd[i++] = 0xC0; //包头
        //以下为8字节
        cmd[i++] = file_len & 0xFF; //第二个字节保存文件名的长度
        cmd[i++] = (file_len >> 8) & 0xFF;
        cmd[i++] = (file_len >> 16) & 0xFF;
        cmd[i++] = (file_len >> 24) & 0xFF; //包长度以小端模式存储
        cmd[i++] = cmd_no & 0xFF;           //命令号  1  第6字节
        cmd[i++] = (cmd_no >> 8) & 0xFF;
        cmd[i++] = (cmd_no >> 16) & 0xFF;
        cmd[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        if (filenames != NULL) //如果file不为空，则把文件名拷贝到包里去
        {                      //第10字节开始是文件名
            strcpy(cmd + i, filenames);
        }
        cmd[(i++) + strlen(filenames)] = 0xC0; //包尾
        //printf("filenames=%s\n", cmd + i - 1); //-1 为了将包尾加上的长度减去

        break;
        //发送断开连接请求
    case FTP_BYE: //命令号  0
                  //判断这个文件是否存在

        pkg_len = 8;
        cmd[i++] = 0xC0; //包头
        cmd[i++] = pkg_len & 0xFF;
        cmd[i++] = (pkg_len >> 8) & 0xFF;
        cmd[i++] = (pkg_len >> 16) & 0xFF;
        cmd[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        cmd[i++] = cmd_no & 0xFF;          //第6字节
        cmd[i++] = (cmd_no >> 8) & 0xFF;
        cmd[i++] = (cmd_no >> 16) & 0xFF;
        cmd[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        cmd[i] = 0xC0; //包尾

        break;
    default:
        pkg_len = 8;
        cmd[i++] = 0xC0; //包头
        cmd[i++] = pkg_len & 0xFF;
        cmd[i++] = (pkg_len >> 8) & 0xFF;
        cmd[i++] = (pkg_len >> 16) & 0xFF;
        cmd[i++] = (pkg_len >> 24) & 0xFF; //包长度以小端模式存储
        cmd[i++] = cmd_no & 0xFF;          //第6字节  UNKNOEN
        cmd[i++] = (cmd_no >> 8) & 0xFF;
        cmd[i++] = (cmd_no >> 16) & 0xFF;
        cmd[i++] = (cmd_no >> 24) & 0xFF; //命令号以小端模式存储

        cmd[i] = 0xC0; //包尾

        break;
    }
}

//将客户端输入的指令转化为int类型
int get_instype(char *comd)
{
    //输入的是ls
    if (strncmp(comd, "ls", 2) == 0)
    {

        return FTP_LS;
    }
    else if (strncmp(comd, "bye", 3) == 0 || strncmp(comd, "Bye", 3) == 0)
    {
        return FTP_BYE;
    }
    else if (strncmp(comd, "get", 3) == 0)
    {
        return FTP_GET;
    }
    else if (strncmp(comd, "put", 3) == 0)
    {
        return FTP_PUT;
    }
    else
    {
        return UNKOWN_CMD;
    }
}

int Create_Client_Socket(const char *ip, const char *port)
{
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Create Socket Failed!");
        return -1;
    }

    //根据服务器IP和服务器的PORT与指定的服务器进行连接
    struct sockaddr_in addr = {0}; //初始化为0
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ip);

    //连接服务器
    if (connect(sock, (const struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        perror("connect error");
        exit(-1);
    }
    return sock;
}

//用来将收到的包解析出来
int Analyz_packet(int sock, unsigned char *packet)
{

    if (packet[5] == FTP_LS)
    {
        //提取数据包里的内容到buf数组里
        unsigned char buf[MAX_FILENAMESIZE] = {0};
        //将数据包中的数据内容拷贝到buf里
        strncpy(buf, packet + 9, MAX_FILENAMESIZE);
        printf("%s\n", buf); //将从服务器接收到的所有文件名打印出来
        return 0;
    }
    else if (packet[5] == FTP_GET)
    {
        //printf("packet[9]=%d\n", (unsigned int)(packet[9] | packet[10] << 8));
        if ((unsigned int)(packet[9] | packet[10] << 8) == 0) //87654321 -> 12345678
        {
            printf("Server don't have this file\n");
            return -1;
        }

        //获取到文件的大小和名字
        unsigned char filename[100] = "";
        strncpy(filename, packet + 13, packet[9]);
        printf("%s的大小为%d\n", filename, (unsigned int)(packet[9] | packet[10] << 8));
        //传输数据保存在数组里
        unsigned char buf[MAX_LEN] = "";
        //继续接受传输file
        int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0664); //在当前目录下想创建接收到的文件
        if (fd < 0)
        {
            perror("fail to open\n");
            return -1;
        }
        while (1) //接收数据
        {
            int ret = read(sock, buf, sizeof(buf));
            if (ret < 1024) //最后一次传输，传完就跳出循环
            {
                write(fd, buf, ret);
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
                int w = write(fd, buf, ret);
                if (w < 0)
                {
                    perror("fail to write");
                    break;
                }
            }
        }
        return 0;
    }
    else if (packet[5] == FTP_PUT)
    {
        // printf("第%d行\n", __LINE__);
        unsigned char putfile[512] = "";
        strncpy(putfile, packet + 13, packet[9]);
        //printf("putfile=%s,文件名长度为:%d\n", putfile, packet[9]);
        //客户端开始传输文件给服务端
        // unsigned char pathname[512] = "";
        // //获取到当前文件所在目录
        // printf("请输入%s文件所在目录：", putfile);
        // fgets(pathname, sizeof(pathname), stdin);
        // pathname[strlen(pathname) - 1] = '\0';
        // //获取到这个文件目录项的完整的带路径名
        // unsigned char filename[512] = {0};
        // sprintf(filename, "%s/%s", pathname, putfile);
        //先open
        // printf("filename=%s\n", filename);

        // int fd = open(filename, O_RDONLY);

        struct stat st;
        //获取此目录项的属性
        int ret = lstat(putfile, &st);
        if (ret == -1)
        {
            perror("Stat Failed");
        }
        int filesize = st.st_size; //获取到文件的大小
        if (filesize == 0)
        {
            printf("pleace check whethre the file exists!\n");
            return -1;
        }
        //打开待传输文件
        printf("filesize=%d\n", filesize);
        int fd = open(putfile, O_RDONLY);
        if (fd == -1)
        {
            fprintf(stderr, "Open %s filed\n", putfile);
            return -1;
        }
        //printf("fd=%d\n", fd);
        //开始将文件下载到服务器端
        //将文件内容分多次发送给服务器端
        unsigned char buf[MAX_LEN] = {0};
        while (1)
        {
            int ret = read(fd, buf, sizeof(buf));
            if (ret == 0)
            {
                printf("%s has to send successfully\n", putfile); //跳出while
                break;
            }
            else if (ret < 0)
            {
                perror("Read File Failed");
                return -1;
            }
            else
            {
                // printf("ret=%d\n", ret);
                //将读取到的文件内容发送给服务端
                ret = write(sock, buf, ret);
                if (ret < 0)
                {
                    perror("Write File Failed");
                    return -1;
                }
            }
        }
        //执行完拷贝,将打开的文件关闭
        close(fd);

        return 0;
    }
    else if (packet[5] == FTP_BYE)
    {
        sleep(1);
        printf("recv from server: byebye!\n");
        //退出客户端
        exit(0);
    }
    else if (packet[5] == UNKOWN_CMD)
    {
        printf("Server don't know what you want to do，please input again!\n");
    }
    else
    {
        //printf("buf[5]=%d\n", packet[5]);
        printf("The message has mistake!\n");
    }
    return 0;
}

/*
	客户端主函数
*/
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage : %s <IP> <PROT>\n", argv[0]);
        return -1;
    }
    //创建sock套接字
    int sock = Create_Client_Socket(argv[1], argv[2]);
    //ftp数据包
    unsigned char cmd[PACKET_SIZE] = {0};
    unsigned char ins[50] = ""; //命令数据
    //给服务器发送数据包
    while (1)
    {
        fprintf(stderr, "ftp>");        //以标准出错流的形式打印出ftp>
        fgets(ins, sizeof(ins), stdin); //get aabldd.c
        ins[strlen(ins) - 1] = '\0';    //将用户输入的‘\n’消化掉
        //将数据包准备好
        prepar_packet(cmd, get_instype(ins), ins + 4);
        //向服务器发送数据
        int ret = send(sock, cmd, sizeof(cmd), 0);
        if (ret < 0)
        {
            perror("fail to send");
            return -1;
        }

        //接收数据 -->
        unsigned char buf[PACKET_SIZE] = {0};
        ret = recv(sock, buf, sizeof(buf), 0);
        if (ret < 0)
        {
            perror("fail to recv");
            return -1;
        }

        Analyz_packet(sock, buf);
    }
}
