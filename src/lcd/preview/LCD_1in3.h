#ifndef HOST_PREVIEW_LCD_1IN3_H
#define HOST_PREVIEW_LCD_1IN3_H

#include "GUI_Paint.h"

#define LCD_1IN3_HEIGHT 240
#define LCD_1IN3_WIDTH 240

#define HORIZONTAL 0

typedef struct {
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
} LCD_1IN3_ATTRIBUTES;

extern LCD_1IN3_ATTRIBUTES LCD_1IN3;

void LCD_1IN3_Display(UWORD *image);
void LCD_1IN3_DisplayWindows(UWORD xstart, UWORD ystart, UWORD xend, UWORD yend, UWORD *image);

#endif
