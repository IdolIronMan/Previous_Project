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
    int x, y, a, r;
    r = 230;
    x = 239;
    y = 399;

    for (i = 0; i < 480; i++)
    {
        for (j = 0; j < 800; j++)
        {
            if ((i - x) * (i - x) + (j - y) * (j - y) < r * r)
            {
                color[i][j] = 0x00ff0000;
            }

            else
            {
                color[i][j] = 0x00ffffff;
            }
        }
    }

    write(fd, color, 480 * 800 * 4);

    close(fd);
}