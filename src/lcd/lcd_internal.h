#ifndef _PICO_LCD_SRC_LCD_INTERNAL_
#define _PICO_LCD_SRC_LCD_INTERNAL_

#include "lcd_platform.h"

int initialize_lcd_event(void);
int draw_splash_screen(UWORD *image);
int draw_menu_screen(UWORD *image);
int draw_radio_menu_screen(UWORD *image, int selected);
UWORD *lcd_current_image(void);
int lcd_current_menu_index(void);
void lcd_set_current_menu_index(int index);
int lcd_current_menu_count(void);
lcd_callback_t lcd_current_callback(void);

#endif
