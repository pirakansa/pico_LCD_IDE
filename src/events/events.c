#include <stdio.h>
#include "pico/stdlib.h"
#include "./events.h"
#include "pico/mutex.h"

#define QUEUE_SIZE 10

auto_init_mutex(events_queue_mutex);

stackevents_dt stackevents[QUEUE_SIZE] = {STACKEVENTS_NONE};
volatile int queue_head;
volatile int queue_tail;
volatile int queue_cnt;

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
    
    stackevents[(queue_head + queue_cnt) % QUEUE_SIZE] = enq_data;
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
    queue_cnt--;

    mutex_exit(&events_queue_mutex);
    return deq_data;
}

