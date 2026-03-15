#include <stdint.h>

#include "../lcd.h"
#include "../gpios.h"
#include "../../tests/test_support.h"

typedef unsigned int uint;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;

typedef struct {
    UWORD WIDTH;
    UWORD HEIGHT;
} lcd_1in3_t;

int initialize_lcd_draw(lcd_callback_t callback);
void gpio_callback(uint gpio, uint32_t events);
void lcd_test_reset_input_state(void);

lcd_1in3_t LCD_1IN3 = {240, 240};
int menu_count = 4;

static int dev_module_init_rc;
static int mutex_try_enter_rc = 1;
static int callback_event = -1;
static int draw_radio_calls;
static int draw_radio_selected = -1;
static int irq_with_callback_calls;
static int irq_enabled_calls;
static int pwm_value = -1;
static int splash_calls;
static int menu_calls;
static uint32_t fake_time_us;

static void reset_state(void) {
    dev_module_init_rc = 0;
    mutex_try_enter_rc = 1;
    callback_event = -1;
    draw_radio_calls = 0;
    draw_radio_selected = -1;
    irq_with_callback_calls = 0;
    irq_enabled_calls = 0;
    pwm_value = -1;
    splash_calls = 0;
    menu_calls = 0;
    fake_time_us = 0;
    lcd_test_reset_runtime_state();
    lcd_test_reset_input_state();
}

int DEV_Module_Init(void) {
    return dev_module_init_rc;
}

void DEV_SET_PWM(int pwm) {
    pwm_value = pwm;
}

void LCD_1IN3_Init(int orientation) {
    (void)orientation;
}

void LCD_1IN3_Clear(UWORD color) {
    (void)color;
}

void SET_Infrared_PIN(int pin) {
    (void)pin;
}

void Paint_NewImage(UBYTE *image, UWORD width, UWORD height, int rotate, UWORD color) {
    (void)image;
    (void)width;
    (void)height;
    (void)rotate;
    (void)color;
}

void Paint_SetScale(int scale) {
    (void)scale;
}

void Paint_SetRotate(int rotate) {
    (void)rotate;
}

void Paint_Clear(UWORD color) {
    (void)color;
}

int draw_splash_screen(UWORD *image) {
    (void)image;
    splash_calls++;
    return 0;
}

int draw_menu_screen(UWORD *image) {
    (void)image;
    menu_calls++;
    return 0;
}

int draw_radio_menu_screen(UWORD *image, int selected) {
    (void)image;
    draw_radio_calls++;
    draw_radio_selected = selected;
    return 0;
}

void DEV_Delay_ms(int ms) {
    (void)ms;
}

uint32_t time_us_32(void) {
    return fake_time_us;
}

int mutex_try_enter(void *mutex, uint32_t *owner) {
    (void)mutex;
    if (owner) {
        *owner = 0;
    }
    return mutex_try_enter_rc;
}

void mutex_exit(void *mutex) {
    (void)mutex;
}

void gpio_set_irq_enabled_with_callback(uint gpio, uint32_t events, _Bool enabled, void (*callback)(uint, uint32_t)) {
    (void)gpio;
    (void)events;
    (void)enabled;
    (void)callback;
    irq_with_callback_calls++;
}

void gpio_set_irq_enabled(uint gpio, uint32_t events, _Bool enabled) {
    (void)gpio;
    (void)events;
    (void)enabled;
    irq_enabled_calls++;
}

static void capture_event(stackevents_dt ev) {
    callback_event = ev;
}

static void test_initialize_lcd_draw_sets_up_host_state(test_suite_t *suite) {
    reset_state();

    ASSERT_INT(suite, "initialize_lcd_draw rc", 0, initialize_lcd_draw(capture_event));
    ASSERT_INT(suite, "splash draw", 1, splash_calls);
    ASSERT_INT(suite, "menu draw", 1, menu_calls);
    ASSERT_INT(suite, "radio draw", 1, draw_radio_calls);
    ASSERT_INT(suite, "selected reset", 0, lcd_test_selected_menu_index());
    ASSERT_INT(suite, "registered callback irqs", 1, irq_with_callback_calls);
    ASSERT_INT(suite, "registered secondary irqs", 7, irq_enabled_calls);
    ASSERT_PTR(suite, "buffer allocated", (void *)1, lcd_test_image_buffer() ? (void *)1 : NULL);
}

static void test_lcd_menu_next_index_wraps_and_ignores_invalid_counts(test_suite_t *suite) {
    ASSERT_INT(suite, "menu up wrap", 3, lcd_menu_next_index(0, -1, 4));
    ASSERT_INT(suite, "menu down wrap", 0, lcd_menu_next_index(3, 1, 4));
    ASSERT_INT(suite, "menu down advance", 2, lcd_menu_next_index(1, 1, 4));
    ASSERT_INT(suite, "menu unchanged invalid count", 0, lcd_menu_next_index(2, -1, 0));
}

static void test_initialize_lcd_draw_propagates_init_error(test_suite_t *suite) {
    reset_state();
    dev_module_init_rc = -1;

    ASSERT_INT(suite, "initialize_lcd_draw rc", -1, initialize_lcd_module());
}

static void test_initialize_lcd_module_configures_display(test_suite_t *suite) {
    reset_state();

    ASSERT_INT(suite, "initialize_lcd_module rc", 0, initialize_lcd_module());
    ASSERT_INT(suite, "pwm configured", 50, pwm_value);
}

static void test_gpio_up_wraps_selection(test_suite_t *suite) {
    reset_state();
    initialize_lcd_draw(capture_event);
    lcd_test_set_selected_menu_index(0);

    gpio_callback(GPIO_KEY_UP, GPIO_KEY_EVENTS_EDGE_RISE);

    ASSERT_INT(suite, "selection wrapped", 3, lcd_test_selected_menu_index());
    ASSERT_INT(suite, "radio redraw selected", 3, draw_radio_selected);
}

static void test_gpio_down_wraps_selection(test_suite_t *suite) {
    reset_state();
    initialize_lcd_draw(capture_event);
    lcd_test_set_selected_menu_index(3);

    gpio_callback(GPIO_KEY_DOWN, GPIO_KEY_EVENTS_EDGE_RISE);

    ASSERT_INT(suite, "selection wrapped", 0, lcd_test_selected_menu_index());
    ASSERT_INT(suite, "radio redraw selected", 0, draw_radio_selected);
}

static void test_gpio_buttons_enqueue_events(test_suite_t *suite) {
    reset_state();
    initialize_lcd_draw(capture_event);

    gpio_callback(GPIO_KEY_A, GPIO_KEY_EVENTS_EDGE_RISE);
    ASSERT_INT(suite, "button a", STACKEVENTS_BTN1, callback_event);

    gpio_callback(GPIO_KEY_B, GPIO_KEY_EVENTS_EDGE_RISE);
    ASSERT_INT(suite, "button b", STACKEVENTS_BTN2, callback_event);

    gpio_callback(GPIO_KEY_X, GPIO_KEY_EVENTS_EDGE_RISE);
    ASSERT_INT(suite, "button x", STACKEVENTS_BTN3, callback_event);

    gpio_callback(GPIO_KEY_Y, GPIO_KEY_EVENTS_EDGE_RISE);
    ASSERT_INT(suite, "button y", STACKEVENTS_BTN4, callback_event);
}

static void test_gpio_button_debounces_duplicate_rise(test_suite_t *suite) {
    reset_state();
    initialize_lcd_draw(capture_event);

    fake_time_us = 1000;
    gpio_callback(GPIO_KEY_A, GPIO_KEY_EVENTS_EDGE_RISE);
    ASSERT_INT(suite, "first button a", STACKEVENTS_BTN1, callback_event);

    callback_event = -1;
    fake_time_us = 20000;
    gpio_callback(GPIO_KEY_A, GPIO_KEY_EVENTS_EDGE_RISE);
    ASSERT_INT(suite, "duplicate rise ignored", -1, callback_event);

    fake_time_us = 40000;
    gpio_callback(GPIO_KEY_A, GPIO_KEY_EVENTS_EDGE_RISE);
    ASSERT_INT(suite, "button a after debounce", STACKEVENTS_BTN1, callback_event);
}

static void test_gpio_ignores_when_mutex_busy(test_suite_t *suite) {
    reset_state();
    initialize_lcd_draw(capture_event);
    mutex_try_enter_rc = 0;
    lcd_test_set_selected_menu_index(1);

    gpio_callback(GPIO_KEY_DOWN, GPIO_KEY_EVENTS_EDGE_RISE);

    ASSERT_INT(suite, "selection unchanged", 1, lcd_test_selected_menu_index());
    ASSERT_INT(suite, "no redraw", 1, draw_radio_calls);
}

int main(void) {
    test_suite_t suite;

    test_suite_begin(&suite, "lcd");
    RUN_TEST(&suite, test_initialize_lcd_draw_sets_up_host_state);
    RUN_TEST(&suite, test_lcd_menu_next_index_wraps_and_ignores_invalid_counts);
    RUN_TEST(&suite, test_initialize_lcd_draw_propagates_init_error);
    RUN_TEST(&suite, test_initialize_lcd_module_configures_display);
    RUN_TEST(&suite, test_gpio_up_wraps_selection);
    RUN_TEST(&suite, test_gpio_down_wraps_selection);
    RUN_TEST(&suite, test_gpio_buttons_enqueue_events);
    RUN_TEST(&suite, test_gpio_button_debounces_duplicate_rise);
    RUN_TEST(&suite, test_gpio_ignores_when_mutex_busy);

    reset_state();
    test_suite_end(&suite);
    return 0;
}
