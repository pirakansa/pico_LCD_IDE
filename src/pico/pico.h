#ifndef _PICO_LCD_SRC_PICO_PICO_
#define _PICO_LCD_SRC_PICO_PICO_

int initialize_pico_module();
void pico_set_led(bool);
bool __no_inline_not_in_flash_func(get_bootsel_button)();

#endif


