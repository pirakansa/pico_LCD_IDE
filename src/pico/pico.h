#ifndef _PICO_LCD_SRC_PICO_PICO_
#define _PICO_LCD_SRC_PICO_PICO_

int initialize_pico_module();
void set_started_led_signal();
void set_err_led_signal(int count);
bool __no_inline_not_in_flash_func(get_bootsel_button_state)();

#endif


