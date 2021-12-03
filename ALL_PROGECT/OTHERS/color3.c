#include "head.h"
#include <math.h>

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
    int x, y, a, r, s, n, s2;
    r = 400 * sqrt(2);
    x = 880;
    y = 399;
    s = (i - x) * (i - x) + (j - x) * (j - y);

    for (i = 0; i < 480; i++)
    {
        for (j = 0; j < 800; j++)
        {

            if (s < r * r)
            {
                color[i][j] = 0x00ff0000;
            }
            n = 6;
            while (n--)
            {
                x = x - 50;
                s2 = (i - x) * (i - x) + (j - x) * (j - y);
                if (s < s2 < r * r)
                {
                    color[i][j] = 0x0000ff00;
                }
            }
        }
    }

    write(fd, color, 480 * 800 * 4);

    close(fd);
}