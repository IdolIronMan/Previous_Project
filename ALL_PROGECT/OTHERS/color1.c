#include "head.h"

int main()
{
    int fd;
    fd = open("/dev/fb0", O_RDWR);
    if (fd == -1)
    {
        return -1;
    }
    int color[480][800];
    int i, j;
    for (i = 0; i < 239; i++)
    {
        for (j = 0; j < 400; j++)
        {
            color[i][j] = 0x00ff0000;
        }
        for (j = 400; j < 800; j++)
        {
            color[i][j] = 0x0000ff00;
        }
    }
    for (i = 240; i < 480; i++)
    {
        for (j = 0; j < 400; j++)
        {
            color[i][j] = 0x000000ff;
        }
        for (j = 400; j < 800; j++)
        {
            color[i][j] = 0x00A020F0;
        }
    }
    write(fd, color, 480 * 800 * 4);

    close(fd);
}