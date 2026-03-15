#include "lcd_internal.h"

auto_init_mutex(counter_mutex);

void gpio_callback(uint gpio, uint32_t events) {
    uint32_t owner;

    if (!mutex_try_enter(&counter_mutex, &owner)) {
        printf("Mutex is already locked\n");
        return;
    }

    printf("GPIO EV %d %d\n", gpio, events);

    if ((GPIO_KEY_UP == gpio) && (GPIO_KEY_EVENTS_EDGE_RISE == events)) {
        int next_index = lcd_menu_next_index(
            lcd_current_menu_index(),
            -1,
            lcd_current_menu_count()
        );
        lcd_set_current_menu_index(next_index);
        draw_radio_menu_screen(lcd_current_image(), next_index);
    } else if ((GPIO_KEY_DOWN == gpio) && (GPIO_KEY_EVENTS_EDGE_RISE == events)) {
        int next_index = lcd_menu_next_index(
            lcd_current_menu_index(),
            1,
            lcd_current_menu_count()
        );
        lcd_set_current_menu_index(next_index);
        draw_radio_menu_screen(lcd_current_image(), next_index);
    } else if ((GPIO_KEY_A == gpio) && (GPIO_KEY_EVENTS_EDGE_RISE == events)) {
        if (lcd_current_callback()) {
            lcd_current_callback()(STACKEVENTS_BTN1);
        }
    } else if ((GPIO_KEY_B == gpio) && (GPIO_KEY_EVENTS_EDGE_RISE == events)) {
        if (lcd_current_callback()) {
            lcd_current_callback()(STACKEVENTS_BTN2);
        }
    } else if ((GPIO_KEY_X == gpio) && (GPIO_KEY_EVENTS_EDGE_RISE == events)) {
        if (lcd_current_callback()) {
            lcd_current_callback()(STACKEVENTS_BTN3);
        }
    } else if ((GPIO_KEY_Y == gpio) && (GPIO_KEY_EVENTS_EDGE_RISE == events)) {
        if (lcd_current_callback()) {
            lcd_current_callback()(STACKEVENTS_BTN4);
        }
    }

    mutex_exit(&counter_mutex);
}

int initialize_lcd_event(void) {
    gpio_set_irq_enabled_with_callback(
        GPIO_KEY_UP,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true,
        &gpio_callback
    );
    gpio_set_irq_enabled(
        GPIO_KEY_DOWN,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_LEFT,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_RIGHT,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_A,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_B,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_X,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_Y,
        GPIO_KEY_EVENTS_EDGE_FALL | GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );

    return 0;
}
