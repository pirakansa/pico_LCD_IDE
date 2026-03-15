#ifndef _PICO_LCD_SRC_PICO_PICO_
#define _PICO_LCD_SRC_PICO_PICO_

#include <stdbool.h>

#ifndef __no_inline_not_in_flash_func
#define __no_inline_not_in_flash_func(func_name) func_name
#endif

int initialize_pico_module();
void set_started_led_signal();
void set_err_led_signal(int count);
bool __no_inline_not_in_flash_func(get_bootsel_button_state)();

#ifdef HOST_TEST
extern int host_pico_led_init_rc;
extern bool host_bootsel_button_state;
extern int host_last_err_led_count;
void gpio_init(int pin);
void gpio_set_dir(int pin, int dir);
void gpio_put(int pin, bool value);
void sleep_ms(int ms);
#endif

#endif
