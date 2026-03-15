#include <stdio.h>
#include <stdlib.h>

#include "../events.h"

extern stackevents_dt stackevents[];
extern volatile int queue_head;
extern volatile int queue_tail;
extern volatile int queue_cnt;

static void assert_event(const char *name, stackevents_dt actual, stackevents_dt expected) {
    if (actual != expected) {
        fprintf(stderr, "%s failed: expected %d, got %d\n", name, expected, actual);
        exit(1);
    }
}

static void assert_int(const char *name, int actual, int expected) {
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

static void test_initialize_clears_queue_state(void) {
    initialize_ev_data();
    enqueue(STACKEVENTS_BTN1);
    enqueue(STACKEVENTS_BTN2);
    dequeue();

    assert_event("re-initialize", initialize_ev_data(), 0);
    assert_int("head reset", queue_head, 0);
    assert_int("tail reset", queue_tail, 0);
    assert_int("count reset", queue_cnt, 0);
    assert_event("queue empty after reinitialize", dequeue(), STACKEVENTS_NONE);
}

static void test_dequeue_clears_slot(void) {
    initialize_ev_data();
    enqueue(STACKEVENTS_BTN4);

    assert_event("dequeue btn4", dequeue(), STACKEVENTS_BTN4);
    assert_event("slot cleared", stackevents[0], STACKEVENTS_NONE);
}

static void test_overflow_does_not_mutate_indices(void) {
    int i;
    int head_before;
    int tail_before;
    int count_before;

    initialize_ev_data();
    for (i = 0; i < 10; i++) {
        enqueue(STACKEVENTS_BTN1);
    }

    head_before = queue_head;
    tail_before = queue_tail;
    count_before = queue_cnt;

    assert_event("overflow no mutate", enqueue(STACKEVENTS_BTN2), STACKEVENTS_FULL);
    assert_int("head unchanged", queue_head, head_before);
    assert_int("tail unchanged", queue_tail, tail_before);
    assert_int("count unchanged", queue_cnt, count_before);
}

int main(void) {
    test_initialize_and_underflow();
    test_fifo_order();
    test_wraparound();
    test_overflow();
    test_initialize_clears_queue_state();
    test_dequeue_clears_slot();
    test_overflow_does_not_mutate_indices();

    printf("events tests passed\n");
    return 0;
}
