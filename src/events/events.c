#include <stdio.h>
#include "./events.h"

#ifdef HOST_TEST
typedef struct {
    int unused;
} mutex_t;

static mutex_t events_queue_mutex;

static void mutex_enter_blocking(mutex_t *mutex) {
    (void)mutex;
}

static void mutex_exit(mutex_t *mutex) {
    (void)mutex;
}
#else
#include "pico/stdlib.h"
#include "pico/mutex.h"

auto_init_mutex(events_queue_mutex);
#endif

#define QUEUE_SIZE 10

static stackevents_dt stackevents[QUEUE_SIZE] = {STACKEVENTS_NONE};
static volatile int queue_head;
static volatile int queue_tail;
static volatile int queue_cnt;

// Initialize the event queue
int initialize_ev_data(){
    mutex_enter_blocking(&events_queue_mutex);
    queue_head = 0;
    queue_tail = 0;
    queue_cnt = 0;

    mutex_exit(&events_queue_mutex);
    return 0;
}

stackevents_dt enqueue(stackevents_dt enq_data) {
    mutex_enter_blocking(&events_queue_mutex);

    if (QUEUE_SIZE <= queue_cnt) {
        mutex_exit(&events_queue_mutex);
        printf("Error: Queue overflow.\n");
        return STACKEVENTS_FULL;
    }

    stackevents[queue_tail] = enq_data;
    queue_tail = (queue_tail + 1) % QUEUE_SIZE;
    queue_cnt++;

    mutex_exit(&events_queue_mutex);
    return enq_data;
}

stackevents_dt dequeue() {
    mutex_enter_blocking(&events_queue_mutex);

    if (queue_cnt <= 0) {
        printf("Error: Queue underflow.\n");
        mutex_exit(&events_queue_mutex);
        return STACKEVENTS_NONE;
    }

    stackevents_dt deq_data = stackevents[queue_head];
    stackevents[queue_head] = STACKEVENTS_NONE;
    queue_head = (queue_head + 1) % QUEUE_SIZE;
    queue_cnt--;

    mutex_exit(&events_queue_mutex);
    return deq_data;
}

#ifdef HOST_TEST
int events_debug_head(void) {
    return queue_head;
}

int events_debug_tail(void) {
    return queue_tail;
}

int events_debug_count(void) {
    return queue_cnt;
}

stackevents_dt events_debug_slot(int index) {
    if (index < 0 || index >= QUEUE_SIZE) {
        return STACKEVENTS_NONE;
    }

    return stackevents[index];
}
#endif
