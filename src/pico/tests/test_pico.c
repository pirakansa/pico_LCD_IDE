#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../pico.h"

int initialize_libpico(void);
int pico_led_init(void);
void pico_set_led(bool led_on);

static int gpio_init_calls;
static int gpio_set_dir_calls;
static int gpio_put_calls;
static int sleep_ms_calls;
static int last_gpio_init_pin = -1;
static int last_gpio_dir_pin = -1;
static int last_gpio_dir = -1;
static int last_gpio_put_pin = -1;
static int last_gpio_put_value = -1;
static int last_sleep_ms = -1;

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
    gpio_init_calls = 0;
    gpio_set_dir_calls = 0;
    gpio_put_calls = 0;
    sleep_ms_calls = 0;
    last_gpio_init_pin = -1;
    last_gpio_dir_pin = -1;
    last_gpio_dir = -1;
    last_gpio_put_pin = -1;
    last_gpio_put_value = -1;
    last_sleep_ms = -1;
    host_pico_led_init_rc = 0;
    host_bootsel_button_state = false;
    host_last_err_led_count = -1;
}

void gpio_init(int pin) {
    gpio_init_calls++;
    last_gpio_init_pin = pin;
}

void gpio_set_dir(int pin, int dir) {
    gpio_set_dir_calls++;
    last_gpio_dir_pin = pin;
    last_gpio_dir = dir;
}

void gpio_put(int pin, bool value) {
    gpio_put_calls++;
    last_gpio_put_pin = pin;
    last_gpio_put_value = value ? 1 : 0;
}

void sleep_ms(int ms) {
    sleep_ms_calls++;
    last_sleep_ms = ms;
}

static void test_pico_led_init_sets_up_gpio(void) {
    reset_state();

    assert_int("pico_led_init rc", 0, pico_led_init());
    assert_int("gpio_init calls", 1, gpio_init_calls);
    assert_int("gpio_set_dir calls", 1, gpio_set_dir_calls);
}

static void test_initialize_libpico_turns_led_off(void) {
    reset_state();

    assert_int("initialize_libpico rc", 0, initialize_libpico());
    assert_int("gpio_put calls", 1, gpio_put_calls);
    assert_int("led off", 0, last_gpio_put_value);
}

static void test_initialize_pico_module_propagates_led_init_result(void) {
    reset_state();
    host_pico_led_init_rc = -1;

    assert_int("initialize_pico_module rc", -1, initialize_pico_module());
    assert_int("gpio_put skipped on failed init", 0, gpio_put_calls);
}

static void test_set_started_led_signal_turns_led_on(void) {
    reset_state();

    set_started_led_signal();

    assert_int("gpio_put calls", 1, gpio_put_calls);
    assert_int("led on", 1, last_gpio_put_value);
}

static void test_set_err_led_signal_records_count_and_blinks(void) {
    reset_state();

    set_err_led_signal(3);

    assert_int("err count", 3, host_last_err_led_count);
    assert_int("gpio_put blink count", 6, gpio_put_calls);
    assert_int("sleep count", 7, sleep_ms_calls);
    assert_int("last sleep", 2000, last_sleep_ms);
}

static void test_get_bootsel_button_state_uses_host_value(void) {
    reset_state();
    host_bootsel_button_state = true;
    assert_int("bootsel true", 1, get_bootsel_button_state());

    host_bootsel_button_state = false;
    assert_int("bootsel false", 0, get_bootsel_button_state());
}

int main(void) {
    test_pico_led_init_sets_up_gpio();
    test_initialize_libpico_turns_led_off();
    test_initialize_pico_module_propagates_led_init_result();
    test_set_started_led_signal_turns_led_on();
    test_set_err_led_signal_records_count_and_blinks();
    test_get_bootsel_button_state_uses_host_value();

    printf("pico tests passed\n");
    return 0;
}
