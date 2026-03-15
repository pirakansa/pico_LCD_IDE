#include "lcd_internal.h"

auto_init_mutex(counter_mutex);

#define GPIO_DEBOUNCE_US 30000u
#define GPIO_TRACKED_MAX 32u

static uint32_t last_rise_event_us[GPIO_TRACKED_MAX];

static bool should_accept_rise_event(uint gpio, uint32_t events) {
    if (events != GPIO_KEY_EVENTS_EDGE_RISE) {
        return false;
    }

    if (gpio >= GPIO_TRACKED_MAX) {
        return true;
    }

    uint32_t now = time_us_32();
    uint32_t last = last_rise_event_us[gpio];

    if ((last != 0u) && ((uint32_t)(now - last) < GPIO_DEBOUNCE_US)) {
        return false;
    }

    last_rise_event_us[gpio] = now;
    return true;
}

void gpio_callback(uint gpio, uint32_t events) {
    uint32_t owner;

    if (!mutex_try_enter(&counter_mutex, &owner)) {
        printf("Mutex is already locked\n");
        return;
    }

    printf("GPIO EV %d %d\n", gpio, events);

    if ((GPIO_KEY_UP == gpio) && should_accept_rise_event(gpio, events)) {
        int next_index = lcd_menu_next_index(
            lcd_current_menu_index(),
            -1,
            lcd_current_menu_count()
        );
        lcd_set_current_menu_index(next_index);
        draw_radio_menu_screen(lcd_current_image(), next_index);
    } else if ((GPIO_KEY_DOWN == gpio) && should_accept_rise_event(gpio, events)) {
        int next_index = lcd_menu_next_index(
            lcd_current_menu_index(),
            1,
            lcd_current_menu_count()
        );
        lcd_set_current_menu_index(next_index);
        draw_radio_menu_screen(lcd_current_image(), next_index);
    } else if ((GPIO_KEY_A == gpio) && should_accept_rise_event(gpio, events)) {
        if (lcd_current_callback()) {
            lcd_current_callback()(STACKEVENTS_BTN1);
        }
    } else if ((GPIO_KEY_B == gpio) && should_accept_rise_event(gpio, events)) {
        if (lcd_current_callback()) {
            lcd_current_callback()(STACKEVENTS_BTN2);
        }
    } else if ((GPIO_KEY_X == gpio) && should_accept_rise_event(gpio, events)) {
        if (lcd_current_callback()) {
            lcd_current_callback()(STACKEVENTS_BTN3);
        }
    } else if ((GPIO_KEY_Y == gpio) && should_accept_rise_event(gpio, events)) {
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

#ifdef HOST_TEST
void lcd_test_reset_input_state(void) {
    for (uint i = 0; i < GPIO_TRACKED_MAX; ++i) {
        last_rise_event_us[i] = 0;
    }
}
#endif
