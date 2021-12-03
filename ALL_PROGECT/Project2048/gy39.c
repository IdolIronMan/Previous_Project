#include "gy39.h"

int uart_init(const char *uart_name)
{
    /*设置串口
	  波特率:9600
	  数据位:8
	  校验位:不要
	  停止位:1
	  数据流控制:无
	  */

    int uart_fd = open(uart_name, O_RDWR); //打开串口1设备文件
    if (uart_fd == -1)
    {
        perror("open error:");
        return -1;
    }

    struct termios myserial;
    //清空结构体
    memset(&myserial, 0, sizeof(myserial));
    //O_RDWR
    myserial.c_cflag |= (CLOCAL | CREAD);
    //设置控制模式状态，本地连接，接受使能
    //设置 数据位
    myserial.c_cflag &= ~CSIZE;   //清空数据位
    myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
    myserial.c_cflag |= CS8;      //数据位:8

    myserial.c_cflag &= ~CSTOPB; //   //1位停止位
    myserial.c_cflag &= ~PARENB; //不要校验

    cfsetospeed(&myserial, B9600); //设置波特率,B9600是定义的宏
    cfsetispeed(&myserial, B9600);

    /* 刷新输出队列,清除正接受的数据 */
    tcflush(uart_fd, TCIFLUSH);

    /* 改变配置 */
    tcsetattr(uart_fd, TCSANOW, &myserial);
    return uart_fd;
}

int gy39_getLUX()
{
    int lux = 0;
    int i;

    unsigned char readbuf1[9] = {0};

    char cmd1[3] = {0xa5, 0x81, 0x26};

    int uart1_fd1 = uart_init("/dev/ttySAC2");

    while (1)
    {

        //发送获取光强的命令
        int wr = write(uart1_fd1, cmd1, 3);
        if (wr == -1)
        {
            perror("fail to write");
        }
        //将文件中的数据读到数组中
        int re = read(uart1_fd1, readbuf1, 9);
        if (re == -1)
        {
            perror("fail to read");
        }

        //处理数据
        lux = (readbuf1[4] << 24 | readbuf1[5] << 16 | readbuf1[6] << 8 | readbuf1[7]) / 100;

        printf("当前光照强度为:%d\n", lux);
        // if (lux > 0)
        // {
        //     return lux;
        // }
        sleep(1);
    }
    close(uart1_fd1);
}

void gy39_getHTP()
{
    int T, Hum, H, P;
    unsigned char readbuf2[15] = {0};
    char cmd2[3] = {0xa5, 0x82, 0x27};
    int i, n = 0;
    int uart1_fd2 = uart_init("/dev/ttySAC2");

    //发送获取光强的命令
    write(uart1_fd2, cmd2, 3);

    while (1)
    {

        //将文件中的数据读到数组中
        read(uart1_fd2, readbuf2, 15);
        //处理数据
        // for (i = 0; i < 15; i++)
        // {
        //     printf("%x\t", readbuf2[i]);
        // }
        // printf("\n");
        //温度
        T = (readbuf2[4] << 8 | readbuf2[5]) / 100;
        //气压
        P = (readbuf2[6] << 24 | readbuf2[7] << 16 | readbuf2[8] << 8 | readbuf2[9]) / 100;
        //湿度
        Hum = (readbuf2[10] << 8 | readbuf2[11]) / 100;
        //海拔
        H = (readbuf2[12] << 8 | readbuf2[13]);

        // if (T > 0 && P > 0 && Hum > 0 && H > 0)
        // {
        printf("*********************\n");
        printf("***当前温度：%d℃\n", T);
        printf("***当前气压：%dpa\n", P);
        printf("***当前湿度：%d%%\n", Hum);
        printf("***当前海拔：%dm\n", H);
        printf("*********************\n");

        // break;
        // }
        sleep(1);
    }
    close(uart1_fd2);
}