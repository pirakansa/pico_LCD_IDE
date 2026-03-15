#ifndef _PICO_LCD_SRC_LCD_LCD_
#define _PICO_LCD_SRC_LCD_LCD_

#include "../events/events.h"

typedef void (*lcd_callback_t)(stackevents_dt ev);

int initialize_lcd_module();
int initialize_lcd_draw(lcd_callback_t callback);
int lcd_menu_next_index(int current, int direction, int menu_count);

#ifdef HOST_TEST
void *lcd_test_image_buffer(void);
int lcd_test_selected_menu_index(void);
void lcd_test_set_selected_menu_index(int index);
void lcd_test_reset_runtime_state(void);
#endif

#endif
