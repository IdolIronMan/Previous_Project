#include "led.h"

int fd_ledall;
int fd_led10;
int fd_led9;
int fd_led8;
int fd_led7;
int fd_no;

int control1 = 0;
int control2 = 0;
int control3 = 0;
int control4 = 0;
int control5 = 0;
//给每个灯都创建文件描述符
int led_init()
{
    fd_ledall = open("/sys/kernel/gec_ctrl/led_all", O_RDWR);

    if (fd_ledall == -1)
    {
        perror("open led_all failed");
        return -1;
    }
    fd_led10 = open("/sys/kernel/gec_ctrl/led_d10", O_RDWR);

    if (fd_led10 == -1)
    {
        perror("open led_d10 failed");
        return -1;
    }
    fd_led9 = open("/sys/kernel/gec_ctrl/led_d9", O_RDWR);

    if (fd_led9 == -1)
    {
        perror("open led_d9 failed");
        return -1;
    }
    fd_led8 = open("/sys/kernel/gec_ctrl/led_d8", O_RDWR);

    if (fd_led8 == -1)
    {
        perror("open led_d8 failed");
        return -1;
    }
    fd_led7 = open("/sys/kernel/gec_ctrl/led_d7", O_RDWR);

    if (fd_led7 == -1)
    {
        perror("open led_d7 failed");
        return -1;
    }
    return 0;
}

//控制每盏灯的亮灭,亮返回1，灭返回0
int led_control(int fd_led, int a)
{
    //亮
    if (a == 1)
    {
        write(fd_led, &a, 4);
        return 1;
    }
    else //灭
    {
        write(fd_led, &a, 4);
        return 0;
    }
}

//点击灯开或灭

int led_touch()
{

    int x = get_x();
    int y = get_y();
    //卧室灯
    if (x > 628 && x < 790 && y > 13 && y < 100)
    {

        fd_no = 1;
        if (control1 == 0)
        {
            control1 = 1;
        }
        else
        {
            control1 = 0;
        }
    }
    //客厅灯
    else if (x > 805 && x < 970 && y > 13 && y < 100)
    {
        fd_no = 2;
        if (control2 == 0)
        {
            control2 = 1;
        }
        else
        {
            control2 = 0;
        }
    }

    //阳台灯
    else if (x > 628 && x < 790 && y > 125 && y < 210)
    {
        fd_no = 3;
        if (control3 == 0)
        {
            control3 = 1;
        }
        else
        {
            control3 = 0;
        }
    }

    //厕所灯
    else if (x > 805 && x < 970 && y > 125 && y < 210)
    {
        fd_no = 4;
        if (control4 == 0)
        {
            control4 = 1;
        }
        else
        {
            control4 = 0;
        }
    }
    //所有灯
    else if (x > 628 && x < 790 && y > 250 && y < 340)
    {
        fd_no = 5;
        if (control5 == 0)
        {
            control5 = 1;
        }
        else
        {
            control5 = 0;
        }
    }

    return fd_no;
}

//由触碰屏幕相应按钮来控制灯的亮灭
int led_make()
{

    led_init();
    int fd_number;

    fd_number = led_touch();
    //要控制的灯的编号

    switch (fd_number)
    {
    case 1:
        led_control(fd_led7, control1);
        close(fd_led7);
        break;
    case 2:
        led_control(fd_led8, control2);
        close(fd_led8);
        break;
    case 3:
        led_control(fd_led9, control3);
        close(fd_led9);
        break;
    case 4:
        led_control(fd_led10, control4);
        close(fd_led10);
        break;
    case 5:
        led_control(fd_ledall, control5);
        close(fd_ledall);
        break;

    default:
        //printf("没按灯啊\n");
        break;
    }
}
