#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "pico/pico.h"
#include "lcd/lcd.h"
#include "usb/usb.h"
#include "events/events.h"


// Function to initialize the board and declare program metadata
int initialize_board(){
    // Declare program description for binary info
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    if(initialize_ev_data()!=0){
        return -1;
    }

    // Initialize Pico module and check for errors
    if(initialize_pico_module()!=0){
        return -1;
    }

    if(initialize_lcd_module()!=0){
        return -1;
    }

    if(initialize_usb_module()!=0){
        return -1;
    }

    return 0;
}


// Callback function to enqueue events into the event queue
void enqueue_events_callback(stackevents_dt ev){
    if(enqueue(ev) == STACKEVENTS_FULL){
        printf("Error: Event queue is full.\n");
        set_err_led_signal(10);
    }
}

// Callback function to dequeue events or handle interrupt events
stackevents_dt dequeue_events_callback(void){
    return (get_bootsel_button_state()) ? STACKEVENTS_INTERRUPT : dequeue();
}

int main ()
{
     // Initialize the board and exit if initialization fails
     if(initialize_board()!=0){
        return 0;
     }

    // Initialize standard I/O for debugging and communication
    stdio_init_all();
    set_started_led_signal();

    if(initialize_lcd_draw(enqueue_events_callback)!=0){
        set_err_led_signal(2);
    }

    while (true) {
        usb_device_task();
        usb_hid_task(dequeue_events_callback);
    }

    return 0;
}


