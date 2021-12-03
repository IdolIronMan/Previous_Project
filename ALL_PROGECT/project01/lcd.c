#include "lcd.h"

static int fd_lcd = -1;
static int *plcd = NULL;
//屏幕初始化
int LCD_Init(void)
{

    fd_lcd = open(LCD_PATH, O_RDWR);
    if (fd_lcd == -1)
    {
        perror("open failed");
        return -1;
    }
    //映射   每个像素点是一个int类型
    plcd = mmap(NULL, LCD_HEIGHT * LCD_WIDTH * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);
    if (plcd == MAP_FAILED)
    {
        perror("mmap failed");
        return -1;
    }

    return 0;
}

//lcd的解初始化函数
void lcd_uinit(void)
{
    munmap(plcd, LCD_WIDTH * LCD_HEIGHT * 4);
    close(fd_lcd);
}

//画点函数
void lcd_draw_point(int x, int y, int color)
{
    if (x >= 0 && x < LCD_WIDTH && y >= 0 && y < LCD_HEIGHT)
    {

        *(plcd + LCD_WIDTH * y + x) = color;
    }
}

//以x0,y0为起始点画
void lcd_draw_orth(int x0, int y0, int w, int h, int color)
{
    int x, y;
    for (y = y0; y <= y0 + h; y++)
    {

        for (x = x0; x <= x0 + w; x++)
        {

            lcd_draw_point(x, y, color);
        }
    }
}

void lcd_draw_cricl(int x0, int y0, int r, int color1)
{
    int x, y;
    for (y = 0; y < LCD_HEIGHT; y++)
    {
        for (x = 0; x < LCD_WIDTH; x++)
        {
            if ((x - x0) * (x - x0) + (y - y0) * (y - y0) < r * r)
            {
                lcd_draw_point(x, y, color1);
            }
        }
    }
}

void lcd_draw_tai()
{
    int x, y;
    int i = 239;
    int j = 399;
    int r = 220;
    for (y = 0; y < LCD_HEIGHT; y++)
    {
        for (x = 0; x < LCD_WIDTH; x++)
        {
            if (x < 399 && (x - j) * (x - j) + (y - i) * (y - i) < r * r)
            {
                lcd_draw_point(x, y, 0x00ffffff);
            }
            if (x >= 399 && (x - j) * (x - j) + (y - i) * (y - i) < r * r)
            {
                lcd_draw_point(x, y, 0x00000000);
            }
        }
    }
    lcd_draw_cricl(400, 130, 110, 0x00ffffff);
    lcd_draw_cricl(400, 350, 110, 0x00000000);
    lcd_draw_cricl(400, 130, 20, 0x00000000);
    lcd_draw_cricl(400, 350, 20, 0x00ffffff);
}

void Lcd_Draw_rainbow()
{
    int x1 = 399, y1 = 539;
    int r = 200;
    int x, y;
    for (y = 0; y < 480; y++)
    {
        for (x = 0; x < 800; x++)
        {
            if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < r * r)
            {
                lcd_draw_point(x, y, 0x00FFE1FF);
            }
            else if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < (r + 30) * (r + 30))
            {
                lcd_draw_point(x, y, 0x000000FF);
            }
            else if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < (r + 60) * (r + 60))
            {
                lcd_draw_point(x, y, 0x00FF00FF);
            }
            else if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < (r + 90) * (r + 90))
            {
                lcd_draw_point(x, y, 0x00FFFF00);
            }
            else if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < (r + 120) * (r + 120))
            {
                lcd_draw_point(x, y, 0x00FF0000);
            }
            else if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < (r + 150) * (r + 150))
            {
                lcd_draw_point(x, y, 0x0000FF00);
            }
            else if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < (r + 180) * (r + 180))
            {
                lcd_draw_point(x, y, 0x00CD6600);
            }
            else if ((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y) < (r + 210) * (r + 210))
            {
                lcd_draw_point(x, y, 0x0098FB98);
            }
        }
    }
}
