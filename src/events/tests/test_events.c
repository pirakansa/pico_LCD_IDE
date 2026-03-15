#include <stdio.h>
#include <stdlib.h>

#include "../events.h"

static void assert_event(const char *name, stackevents_dt actual, stackevents_dt expected) {
    if (actual != expected) {
        fprintf(stderr, "%s failed: expected %d, got %d\n", name, expected, actual);
        exit(1);
    }
}

static void test_initialize_and_underflow(void) {
    assert_event("initialize_ev_data", initialize_ev_data(), 0);
    assert_event("dequeue empty", dequeue(), STACKEVENTS_NONE);
}

static void test_fifo_order(void) {
    initialize_ev_data();

    assert_event("enqueue btn1", enqueue(STACKEVENTS_BTN1), STACKEVENTS_BTN1);
    assert_event("enqueue btn2", enqueue(STACKEVENTS_BTN2), STACKEVENTS_BTN2);
    assert_event("dequeue btn1", dequeue(), STACKEVENTS_BTN1);
    assert_event("dequeue btn2", dequeue(), STACKEVENTS_BTN2);
}

static void test_wraparound(void) {
    int i;

    initialize_ev_data();

    for (i = 0; i < 9; i++) {
        assert_event("prime queue", enqueue(STACKEVENTS_BTN1), STACKEVENTS_BTN1);
    }

    for (i = 0; i < 5; i++) {
        assert_event("drain primed queue", dequeue(), STACKEVENTS_BTN1);
    }

    for (i = 0; i < 5; i++) {
        assert_event("wrap enqueue", enqueue(STACKEVENTS_BTN2), STACKEVENTS_BTN2);
    }

    for (i = 0; i < 4; i++) {
        assert_event("remaining btn1", dequeue(), STACKEVENTS_BTN1);
    }

    for (i = 0; i < 5; i++) {
        assert_event("wrapped btn2", dequeue(), STACKEVENTS_BTN2);
    }
}

static void test_overflow(void) {
    int i;

    initialize_ev_data();

    for (i = 0; i < 10; i++) {
        assert_event("fill queue", enqueue(STACKEVENTS_BTN3), STACKEVENTS_BTN3);
    }

    assert_event("overflow", enqueue(STACKEVENTS_BTN4), STACKEVENTS_FULL);
}

int main(void) {
    test_initialize_and_underflow();
    test_fifo_order();
    test_wraparound();
    test_overflow();

    printf("events tests passed\n");
    return 0;
}
