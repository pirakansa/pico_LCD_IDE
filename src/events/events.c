#include <stdio.h>
#include "pico/stdlib.h"
#include "./events.h"
#include "pico/mutex.h"

#define QUEUE_SIZE 10

auto_init_mutex(events_pointer_mutex);

typedef enum {
    STACKEVENTS_ADD = 0,
    STACKEVENTS_SUB,
} pointer_action_dt;

stackevents_dt stackevents[QUEUE_SIZE] = {STACKEVENTS_NONE};
volatile int queue_head;
volatile int queue_cnt;

// Initialize the event queue
int initialize_ev_data(){
    queue_head = 0;
    queue_cnt = 0;
    return 0;
}

// Perform an action on the event queue (add or remove)
int events_action(pointer_action_dt act) {
    mutex_enter_blocking(&events_pointer_mutex);

    if (act == STACKEVENTS_ADD) {
        queue_cnt++;
    } else if (act == STACKEVENTS_SUB) {
        queue_head = (queue_head + 1) % QUEUE_SIZE;
        queue_cnt--;
    }

    mutex_exit(&events_pointer_mutex);
    return 0;
}


stackevents_dt enqueue(stackevents_dt enq_data) {
    if (QUEUE_SIZE <= queue_cnt) {
        printf("Error: Queue overflow.\n");
        return STACKEVENTS_NONE;
    }

    stackevents[(queue_head + queue_cnt) % QUEUE_SIZE] = enq_data;
    events_action(STACKEVENTS_ADD);
    return enq_data;
}

stackevents_dt dequeue() {
    if (queue_cnt <= 0) {
        printf("Error: Queue underflow.\n");
        return STACKEVENTS_FULL;
    }

    stackevents_dt deq_data = stackevents[queue_head];
    events_action(STACKEVENTS_SUB);
    return deq_data;
}

