#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../usb.h"

int initialize_usb_module(void);
void send_hid_report(uint8_t report_id, stackevents_dt ev);
extern uint32_t usb_hid_task_start_ms;

enum {
    REPORT_ID_KEYBOARD = 1,
};

static bool tud_init_rc = true;
static bool hid_ready = true;
static bool suspended;
static uint32_t fake_millis;
static int remote_wakeup_calls;
static int tud_task_calls;
static int keyboard_report_calls;
static int last_modifier = -1;
static int last_keycode = -1;
static int event_source_calls;
static stackevents_dt next_event = STACKEVENTS_NONE;

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
    tud_init_rc = true;
    hid_ready = true;
    suspended = false;
    fake_millis = 0;
    remote_wakeup_calls = 0;
    tud_task_calls = 0;
    keyboard_report_calls = 0;
    last_modifier = -1;
    last_keycode = -1;
    event_source_calls = 0;
    next_event = STACKEVENTS_NONE;
    usb_hid_task_start_ms = 0;
}

bool tud_init(uint8_t rhport) {
    (void)rhport;
    return tud_init_rc;
}

void tud_task(void) {
    tud_task_calls++;
}

bool tud_hid_ready(void) {
    return hid_ready;
}

bool tud_suspended(void) {
    return suspended;
}

void tud_remote_wakeup(void) {
    remote_wakeup_calls++;
}

void tud_hid_keyboard_report(uint8_t report_id, uint8_t modifier, uint8_t keycode[6]) {
    (void)report_id;
    keyboard_report_calls++;
    last_modifier = modifier;
    last_keycode = keycode ? keycode[0] : -1;
}

uint32_t board_millis(void) {
    return fake_millis;
}

void sleep_ms(uint32_t ms) {
    (void)ms;
}

static stackevents_dt get_event(void) {
    event_source_calls++;
    return next_event;
}

static void test_initialize_usb_module_reports_tud_init_result(void) {
    reset_state();
    tud_init_rc = true;
    assert_int("initialize success", 0, initialize_usb_module());

    tud_init_rc = false;
    assert_int("initialize failure", -1, initialize_usb_module());
}

static void test_usb_event_text_maps_supported_events(void) {
    const char *btn1 = usb_event_text(STACKEVENTS_BTN1);
    const char *btn2 = usb_event_text(STACKEVENTS_BTN2);
    const char *btn3 = usb_event_text(STACKEVENTS_BTN3);
    const char *btn4 = usb_event_text(STACKEVENTS_BTN4);
    const char *none = usb_event_text(STACKEVENTS_NONE);

    assert_int("btn1 first char", 'm', btn1[0]);
    assert_int("btn2 first char", 'b', btn2[0]);
    assert_int("btn3 suffix", '3', btn3[3]);
    assert_int("btn4 suffix", '4', btn4[3]);
    assert_int("none is null", 1, none == NULL);
}

static void test_usb_event_text_returns_null_for_non_text_events(void) {
    assert_int("interrupt is null", 1, usb_event_text(STACKEVENTS_INTERRUPT) == NULL);
    assert_int("full is null", 1, usb_event_text(STACKEVENTS_FULL) == NULL);
}

static void test_usb_hid_task_waits_for_interval(void) {
    reset_state();
    fake_millis = 5;

    usb_hid_task(get_event);

    assert_int("event source calls", 0, event_source_calls);
    assert_int("no report before interval", 0, keyboard_report_calls);
}

static void test_usb_hid_task_wakes_host_on_interrupt(void) {
    reset_state();
    fake_millis = 10;
    suspended = true;
    next_event = STACKEVENTS_INTERRUPT;

    usb_hid_task(get_event);

    assert_int("event source calls", 1, event_source_calls);
    assert_int("remote wakeup", 1, remote_wakeup_calls);
    assert_int("no keyboard report", 0, keyboard_report_calls);
}

static void test_usb_hid_task_sends_keyboard_report(void) {
    reset_state();
    fake_millis = 10;
    next_event = STACKEVENTS_BTN2;

    usb_hid_task(get_event);

    assert_int("event source calls", 1, event_source_calls);
    assert_int("reports sent", 22, keyboard_report_calls);
}

static void test_send_hid_report_skips_when_not_ready(void) {
    reset_state();
    hid_ready = false;

    send_hid_report(REPORT_ID_KEYBOARD, STACKEVENTS_BTN1);

    assert_int("reports skipped", 0, keyboard_report_calls);
}

int main(void) {
    test_initialize_usb_module_reports_tud_init_result();
    test_usb_event_text_maps_supported_events();
    test_usb_event_text_returns_null_for_non_text_events();
    test_usb_hid_task_waits_for_interval();
    test_usb_hid_task_wakes_host_on_interrupt();
    test_usb_hid_task_sends_keyboard_report();
    test_send_hid_report_skips_when_not_ready();

    printf("usb tests passed\n");
    return 0;
}
