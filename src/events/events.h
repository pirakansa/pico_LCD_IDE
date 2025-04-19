#ifndef _PICO_LCD_SRC_ENVENS_ENVENS_
#define _PICO_LCD_SRC_ENVENS_ENVENS_

// Define event types for the event queue
typedef enum {
    STACKEVENTS_NONE = 0,       // No event
    STACKEVENTS_FULL,           // Queue is full
    STACKEVENTS_INTERRUPT,      // Interrupt event
    STACKEVENTS_BTN1,           // Button 1 pressed
    STACKEVENTS_BTN2,           // Button 2 pressed
    // Add new events here
} stackevents_dt;

// Initialize the event queue
int initialize_ev_data();

// Add an event to the queue
stackevents_dt enqueue(stackevents_dt enq_data);

// Remove an event from the queue
stackevents_dt dequeue();

#endif


