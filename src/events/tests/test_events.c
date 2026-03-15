#include "../events.h"
#include "../../tests/test_support.h"

static void test_initialize_and_underflow(test_suite_t *suite) {
    ASSERT_INT(suite, "initialize_ev_data", 0, initialize_ev_data());
    ASSERT_INT(suite, "dequeue empty", STACKEVENTS_NONE, dequeue());
}

static void test_fifo_order(test_suite_t *suite) {
    initialize_ev_data();

    ASSERT_INT(suite, "enqueue btn1", STACKEVENTS_BTN1, enqueue(STACKEVENTS_BTN1));
    ASSERT_INT(suite, "enqueue btn2", STACKEVENTS_BTN2, enqueue(STACKEVENTS_BTN2));
    ASSERT_INT(suite, "dequeue btn1", STACKEVENTS_BTN1, dequeue());
    ASSERT_INT(suite, "dequeue btn2", STACKEVENTS_BTN2, dequeue());
}

static void test_wraparound(test_suite_t *suite) {
    int i;

    initialize_ev_data();

    for (i = 0; i < 9; i++) {
        ASSERT_INT(suite, "prime queue", STACKEVENTS_BTN1, enqueue(STACKEVENTS_BTN1));
    }

    for (i = 0; i < 5; i++) {
        ASSERT_INT(suite, "drain primed queue", STACKEVENTS_BTN1, dequeue());
    }

    for (i = 0; i < 5; i++) {
        ASSERT_INT(suite, "wrap enqueue", STACKEVENTS_BTN2, enqueue(STACKEVENTS_BTN2));
    }

    for (i = 0; i < 4; i++) {
        ASSERT_INT(suite, "remaining btn1", STACKEVENTS_BTN1, dequeue());
    }

    for (i = 0; i < 5; i++) {
        ASSERT_INT(suite, "wrapped btn2", STACKEVENTS_BTN2, dequeue());
    }
}

static void test_overflow(test_suite_t *suite) {
    int i;

    initialize_ev_data();

    for (i = 0; i < 10; i++) {
        ASSERT_INT(suite, "fill queue", STACKEVENTS_BTN3, enqueue(STACKEVENTS_BTN3));
    }

    ASSERT_INT(suite, "overflow", STACKEVENTS_FULL, enqueue(STACKEVENTS_BTN4));
}

static void test_initialize_clears_queue_state(test_suite_t *suite) {
    initialize_ev_data();
    enqueue(STACKEVENTS_BTN1);
    enqueue(STACKEVENTS_BTN2);
    dequeue();

    ASSERT_INT(suite, "re-initialize", 0, initialize_ev_data());
    ASSERT_INT(suite, "head reset", 0, events_debug_head());
    ASSERT_INT(suite, "tail reset", 0, events_debug_tail());
    ASSERT_INT(suite, "count reset", 0, events_debug_count());
    ASSERT_INT(suite, "queue empty after reinitialize", STACKEVENTS_NONE, dequeue());
}

static void test_dequeue_clears_slot(test_suite_t *suite) {
    initialize_ev_data();
    enqueue(STACKEVENTS_BTN4);

    ASSERT_INT(suite, "dequeue btn4", STACKEVENTS_BTN4, dequeue());
    ASSERT_INT(suite, "slot cleared", STACKEVENTS_NONE, events_debug_slot(0));
}

static void test_overflow_does_not_mutate_indices(test_suite_t *suite) {
    int i;
    int head_before;
    int tail_before;
    int count_before;

    initialize_ev_data();
    for (i = 0; i < 10; i++) {
        enqueue(STACKEVENTS_BTN1);
    }

    head_before = events_debug_head();
    tail_before = events_debug_tail();
    count_before = events_debug_count();

    ASSERT_INT(suite, "overflow no mutate", STACKEVENTS_FULL, enqueue(STACKEVENTS_BTN2));
    ASSERT_INT(suite, "head unchanged", head_before, events_debug_head());
    ASSERT_INT(suite, "tail unchanged", tail_before, events_debug_tail());
    ASSERT_INT(suite, "count unchanged", count_before, events_debug_count());
}

int main(void) {
    test_suite_t suite;

    test_suite_begin(&suite, "events");
    RUN_TEST(&suite, test_initialize_and_underflow);
    RUN_TEST(&suite, test_fifo_order);
    RUN_TEST(&suite, test_wraparound);
    RUN_TEST(&suite, test_overflow);
    RUN_TEST(&suite, test_initialize_clears_queue_state);
    RUN_TEST(&suite, test_dequeue_clears_slot);
    RUN_TEST(&suite, test_overflow_does_not_mutate_indices);
    test_suite_end(&suite);
    return 0;
}
