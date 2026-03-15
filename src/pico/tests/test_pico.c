#include "../pico.h"
#include "../../tests/test_support.h"

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

static void test_pico_led_init_sets_up_gpio(test_suite_t *suite) {
    reset_state();

    ASSERT_INT(suite, "pico_led_init rc", 0, pico_led_init());
    ASSERT_INT(suite, "gpio_init calls", 1, gpio_init_calls);
    ASSERT_INT(suite, "gpio_set_dir calls", 1, gpio_set_dir_calls);
}

static void test_initialize_libpico_turns_led_off(test_suite_t *suite) {
    reset_state();

    ASSERT_INT(suite, "initialize_libpico rc", 0, initialize_libpico());
    ASSERT_INT(suite, "gpio_put calls", 1, gpio_put_calls);
    ASSERT_INT(suite, "led off", 0, last_gpio_put_value);
}

static void test_initialize_pico_module_propagates_led_init_result(test_suite_t *suite) {
    reset_state();
    host_pico_led_init_rc = -1;

    ASSERT_INT(suite, "initialize_pico_module rc", -1, initialize_pico_module());
    ASSERT_INT(suite, "gpio_put skipped on failed init", 0, gpio_put_calls);
}

static void test_set_started_led_signal_turns_led_on(test_suite_t *suite) {
    reset_state();

    set_started_led_signal();

    ASSERT_INT(suite, "gpio_put calls", 1, gpio_put_calls);
    ASSERT_INT(suite, "led on", 1, last_gpio_put_value);
}

static void test_set_err_led_signal_records_count_and_blinks(test_suite_t *suite) {
    reset_state();

    set_err_led_signal(3);

    ASSERT_INT(suite, "err count", 3, host_last_err_led_count);
    ASSERT_INT(suite, "gpio_put blink count", 6, gpio_put_calls);
    ASSERT_INT(suite, "sleep count", 7, sleep_ms_calls);
    ASSERT_INT(suite, "last sleep", 2000, last_sleep_ms);
}

static void test_get_bootsel_button_state_uses_host_value(test_suite_t *suite) {
    reset_state();
    host_bootsel_button_state = true;
    ASSERT_INT(suite, "bootsel true", 1, get_bootsel_button_state());

    host_bootsel_button_state = false;
    ASSERT_INT(suite, "bootsel false", 0, get_bootsel_button_state());
}

int main(void) {
    test_suite_t suite;

    test_suite_begin(&suite, "pico");
    RUN_TEST(&suite, test_pico_led_init_sets_up_gpio);
    RUN_TEST(&suite, test_initialize_libpico_turns_led_off);
    RUN_TEST(&suite, test_initialize_pico_module_propagates_led_init_result);
    RUN_TEST(&suite, test_set_started_led_signal_turns_led_on);
    RUN_TEST(&suite, test_set_err_led_signal_records_count_and_blinks);
    RUN_TEST(&suite, test_get_bootsel_button_state_uses_host_value);
    test_suite_end(&suite);
    return 0;
}
