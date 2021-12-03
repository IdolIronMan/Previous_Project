#include "main.h"

int main()
{

    LCD_Init();
    //printf("****************\n");
    lcd_draw_orth(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x00);
    //printf("&&&&&&&&&&&&&&&&\n");
    //lcd_draw_cricl(399, 239, 100, 0x009932CC);
    //lcd_draw_tai();
    Lcd_Draw_rainbow();
    lcd_uinit();
}
