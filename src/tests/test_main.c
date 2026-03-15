#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../events/events.h"
#include "../lcd/lcd.h"
#include "../usb/usb.h"

int initialize_board(void);
void enqueue_events_callback(stackevents_dt ev);
stackevents_dt dequeue_events_callback(void);

static int event_init_rc;
static int pico_init_rc;
static int lcd_init_rc;
static int usb_init_rc;
static bool bootsel_state;
static stackevents_dt enqueue_result;
static stackevents_dt dequeue_result;
static int err_led_count;
static int event_init_calls;
static int pico_init_calls;
static int lcd_init_calls;
static int usb_init_calls;
static int enqueue_calls;
static int dequeue_calls;
static int call_order[8];
static int call_order_len;

static void reset_state(void) {
    event_init_rc = 0;
    pico_init_rc = 0;
    lcd_init_rc = 0;
    usb_init_rc = 0;
    bootsel_state = false;
    enqueue_result = STACKEVENTS_NONE;
    dequeue_result = STACKEVENTS_NONE;
    err_led_count = -1;
    event_init_calls = 0;
    pico_init_calls = 0;
    lcd_init_calls = 0;
    usb_init_calls = 0;
    enqueue_calls = 0;
    dequeue_calls = 0;
    call_order_len = 0;
}

static void fail(const char *name, int expected, int actual) {
    fprintf(stderr, "%s failed: expected %d, got %d\n", name, expected, actual);
    exit(1);
}

static void assert_int(const char *name, int expected, int actual) {
    if (expected != actual) {
        fail(name, expected, actual);
    }
}

static void assert_event(const char *name, stackevents_dt expected, stackevents_dt actual) {
    if (expected != actual) {
        fail(name, expected, actual);
    }
}

int initialize_ev_data(void) {
    event_init_calls++;
    call_order[call_order_len++] = 1;
    return event_init_rc;
}

int initialize_pico_module(void) {
    pico_init_calls++;
    call_order[call_order_len++] = 2;
    return pico_init_rc;
}

int initialize_lcd_module(void) {
    lcd_init_calls++;
    call_order[call_order_len++] = 3;
    return lcd_init_rc;
}

int initialize_usb_module(void) {
    usb_init_calls++;
    call_order[call_order_len++] = 4;
    return usb_init_rc;
}

int initialize_lcd_draw(lcd_callback_t callback) {
    (void)callback;
    return 0;
}

void usb_device_task(void) {
}

void usb_hid_task(get_new_event_t func) {
    (void)func;
}

void stdio_init_all(void) {
}

void set_started_led_signal(void) {
}

void set_err_led_signal(int count) {
    err_led_count = count;
}

stackevents_dt enqueue(stackevents_dt ev) {
    enqueue_calls++;
    (void)ev;
    return enqueue_result;
}

stackevents_dt dequeue(void) {
    dequeue_calls++;
    return dequeue_result;
}

bool get_bootsel_button_state(void) {
    return bootsel_state;
}

static void test_initialize_board_success(void) {
    reset_state();

    assert_int("initialize_board success", 0, initialize_board());
    assert_int("event init calls", 1, event_init_calls);
    assert_int("pico init calls", 1, pico_init_calls);
    assert_int("lcd init calls", 1, lcd_init_calls);
    assert_int("usb init calls", 1, usb_init_calls);
    assert_int("call count", 4, call_order_len);
    assert_int("call order 1", 1, call_order[0]);
    assert_int("call order 2", 2, call_order[1]);
    assert_int("call order 3", 3, call_order[2]);
    assert_int("call order 4", 4, call_order[3]);
}

static void test_initialize_board_stops_on_error(void) {
    reset_state();
    lcd_init_rc = -1;

    assert_int("initialize_board failure", -1, initialize_board());
    assert_int("event init calls", 1, event_init_calls);
    assert_int("pico init calls", 1, pico_init_calls);
    assert_int("lcd init calls", 1, lcd_init_calls);
    assert_int("usb init calls", 0, usb_init_calls);
}

static void test_enqueue_callback_error_signal(void) {
    reset_state();
    enqueue_result = STACKEVENTS_FULL;

    enqueue_events_callback(STACKEVENTS_BTN1);

    assert_int("enqueue calls", 1, enqueue_calls);
    assert_int("error led count", 10, err_led_count);
}

static void test_enqueue_callback_no_error(void) {
    reset_state();
    enqueue_result = STACKEVENTS_BTN1;

    enqueue_events_callback(STACKEVENTS_BTN1);

    assert_int("enqueue calls", 1, enqueue_calls);
    assert_int("error led untouched", -1, err_led_count);
}

static void test_dequeue_callback_interrupt_priority(void) {
    reset_state();
    bootsel_state = true;
    dequeue_result = STACKEVENTS_BTN2;

    assert_event("interrupt result", STACKEVENTS_INTERRUPT, dequeue_events_callback());
    assert_int("dequeue bypassed", 0, dequeue_calls);
}

static void test_dequeue_callback_queue_fallback(void) {
    reset_state();
    bootsel_state = false;
    dequeue_result = STACKEVENTS_BTN3;

    assert_event("queue result", STACKEVENTS_BTN3, dequeue_events_callback());
    assert_int("dequeue calls", 1, dequeue_calls);
}

int main(void) {
    test_initialize_board_success();
    test_initialize_board_stops_on_error();
    test_enqueue_callback_error_signal();
    test_enqueue_callback_no_error();
    test_dequeue_callback_interrupt_priority();
    test_dequeue_callback_queue_fallback();

    printf("main tests passed\n");
    return 0;
}
