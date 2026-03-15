#ifndef _PICO_LCD_SRC_LCD_LCD_
#define _PICO_LCD_SRC_LCD_LCD_

#include "../events/events.h"

typedef void (*lcd_callback_t)(stackevents_dt ev);

int initialize_lcd_module();
int initialize_lcd_draw(lcd_callback_t callback);
int lcd_menu_next_index(int current, int direction, int menu_count);

#endif

