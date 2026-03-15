#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lcd.h"
#include "../gpios.h"

typedef unsigned int uint;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;

typedef struct {
    UWORD WIDTH;
    UWORD HEIGHT;
} lcd_1in3_t;

extern UWORD *BlackImage;
extern volatile int select_menu_idx;

int initialize_lcd_draw(lcd_callback_t callback);
void gpio_callback(uint gpio, uint32_t events);

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

static void fail(const char *name, int expected, int actual) {
    fprintf(stderr, "%s failed: expected %d, got %d\n", name, expected, actual);
    exit(1);
}

static void assert_int(const char *name, int expected, int actual) {
    if (expected != actual) {
        fail(name, expected, actual);
    }
}

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
    select_menu_idx = 0;
    free(BlackImage);
    BlackImage = NULL;
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

static void test_initialize_lcd_draw_sets_up_host_state(void) {
    reset_state();

    assert_int("initialize_lcd_draw rc", 0, initialize_lcd_draw(capture_event));
    assert_int("splash draw", 1, splash_calls);
    assert_int("menu draw", 1, menu_calls);
    assert_int("radio draw", 1, draw_radio_calls);
    assert_int("selected reset", 0, select_menu_idx);
    assert_int("registered callback irqs", 1, irq_with_callback_calls);
    assert_int("registered secondary irqs", 7, irq_enabled_calls);
}

static void test_lcd_menu_next_index_wraps_and_ignores_invalid_counts(void) {
    assert_int("menu up wrap", 3, lcd_menu_next_index(0, -1, 4));
    assert_int("menu down wrap", 0, lcd_menu_next_index(3, 1, 4));
    assert_int("menu down advance", 2, lcd_menu_next_index(1, 1, 4));
    assert_int("menu unchanged invalid count", 0, lcd_menu_next_index(2, -1, 0));
}

static void test_initialize_lcd_draw_propagates_init_error(void) {
    reset_state();
    dev_module_init_rc = -1;

    assert_int("initialize_lcd_draw rc", -1, initialize_lcd_module());
}

static void test_initialize_lcd_module_configures_display(void) {
    reset_state();

    assert_int("initialize_lcd_module rc", 0, initialize_lcd_module());
    assert_int("pwm configured", 50, pwm_value);
}

static void test_gpio_up_wraps_selection(void) {
    reset_state();
    initialize_lcd_draw(capture_event);
    select_menu_idx = 0;

    gpio_callback(GPIO_KEY_UP, GPIO_KEY_EVENTS_EDGE_RISE);

    assert_int("selection wrapped", 3, select_menu_idx);
    assert_int("radio redraw selected", 3, draw_radio_selected);
}

static void test_gpio_down_wraps_selection(void) {
    reset_state();
    initialize_lcd_draw(capture_event);
    select_menu_idx = 3;

    gpio_callback(GPIO_KEY_DOWN, GPIO_KEY_EVENTS_EDGE_RISE);

    assert_int("selection wrapped", 0, select_menu_idx);
    assert_int("radio redraw selected", 0, draw_radio_selected);
}

static void test_gpio_buttons_enqueue_events(void) {
    reset_state();
    initialize_lcd_draw(capture_event);

    gpio_callback(GPIO_KEY_A, GPIO_KEY_EVENTS_EDGE_RISE);
    assert_int("button a", STACKEVENTS_BTN1, callback_event);

    gpio_callback(GPIO_KEY_B, GPIO_KEY_EVENTS_EDGE_RISE);
    assert_int("button b", STACKEVENTS_BTN2, callback_event);

    gpio_callback(GPIO_KEY_X, GPIO_KEY_EVENTS_EDGE_RISE);
    assert_int("button x", STACKEVENTS_BTN3, callback_event);

    gpio_callback(GPIO_KEY_Y, GPIO_KEY_EVENTS_EDGE_RISE);
    assert_int("button y", STACKEVENTS_BTN4, callback_event);
}

static void test_gpio_ignores_when_mutex_busy(void) {
    reset_state();
    initialize_lcd_draw(capture_event);
    mutex_try_enter_rc = 0;
    select_menu_idx = 1;

    gpio_callback(GPIO_KEY_DOWN, GPIO_KEY_EVENTS_EDGE_RISE);

    assert_int("selection unchanged", 1, select_menu_idx);
    assert_int("no redraw", 1, draw_radio_calls);
}

int main(void) {
    test_initialize_lcd_draw_sets_up_host_state();
    test_lcd_menu_next_index_wraps_and_ignores_invalid_counts();
    test_initialize_lcd_draw_propagates_init_error();
    test_initialize_lcd_module_configures_display();
    test_gpio_up_wraps_selection();
    test_gpio_down_wraps_selection();
    test_gpio_buttons_enqueue_events();
    test_gpio_ignores_when_mutex_busy();

    reset_state();
    printf("lcd tests passed\n");
    return 0;
}
