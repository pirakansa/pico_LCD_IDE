#ifndef _LCD_RES_DRAWDATA_H_
#define _LCD_RES_DRAWDATA_H_

#include "GUI_Paint.h"
#include "LCD_1in3.h"

#include "res/Lenna.c"

int draw_splash_screen(UWORD *Image);
int draw_radio_menu_screen(UWORD *Image,int selected);
int draw_text_menu_screen(UWORD *Image);
int draw_menu_screen(UWORD *Image);

int draw_splash_screen(UWORD *Image){
    Paint_DrawImage(gimp_image.pixel_data, 0, 0, (uint16_t)gimp_image.width, (uint16_t)gimp_image.height);
    LCD_1IN3_Display(Image);

    return 0;
}

static const char *menu_lists[] = {
	"menu1",
    "menu2",
	"menu3",
    "menu4"
};

#define MENU_CIRCLE_SIZE 15
#define MENU_CIRCLE_MARGIN_SIZE 5
#define LCD_HEIGHT 240

int draw_radio_menu_screen(UWORD *Image, int selected){
    int menusCount = sizeof menu_lists / sizeof menu_lists[0];
    for(int i = 0; i < menusCount; i++) {
        DRAW_FILL stat = (selected == i) ? DRAW_FILL_EMPTY : DRAW_FILL_FULL;
        Paint_DrawCircle(
            (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE),
            (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE) + (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE)*2*i,
            MENU_CIRCLE_SIZE,
            BROWN, DOT_PIXEL_2X2, DRAW_FILL_FULL);
        Paint_DrawCircle(
            (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE),
            (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE) + (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE)*2*i,
            MENU_CIRCLE_SIZE,
            CYAN, DOT_PIXEL_2X2, stat);
    }
    LCD_1IN3_DisplayWindows(
        0, 0,
        (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE)*2, LCD_HEIGHT,
        Image);

    return 0;
}

int draw_text_menu_screen(UWORD *Image){
    int menusCount = sizeof menu_lists / sizeof menu_lists[0];
    for(int i = 0; i < menusCount; i++) {
        Paint_DrawString_EN(
            (MENU_CIRCLE_SIZE + MENU_CIRCLE_MARGIN_SIZE)*2,
            (MENU_CIRCLE_MARGIN_SIZE + 5) + (MENU_CIRCLE_SIZE*2 + MENU_CIRCLE_MARGIN_SIZE*2)*i,
            menu_lists[i], &Font20, WHITE, GREEN); // BRED, WHITE); 
    }
    LCD_1IN3_Display(Image);

    return 0;
}

int draw_menu_screen(UWORD *Image){
    draw_radio_menu_screen(Image, -1);
    draw_text_menu_screen(Image);

    return 0;
}



#endif


