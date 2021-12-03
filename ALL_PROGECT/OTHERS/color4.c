#include "head.h"

void draw_point(int x, int y, int color)
{
    int *p;
    if (x >= 0 && x < 800 && y >= 0 && y < 480)
        *(p + 800 * y + x) = color;
}
