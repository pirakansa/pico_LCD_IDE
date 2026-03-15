#include <stdio.h>
#ifdef HOST_TEST
#include <stdbool.h>
#define bi_decl(x) ((void)0)
#define bi_program_description(x) (x)
#else
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#endif

#include "pico/pico.h"
#include "lcd/lcd.h"
#include "usb/usb.h"
#include "events/events.h"

typedef enum {
    BOARD_INIT_OK = 0,
    BOARD_INIT_EVENT_ERROR = -1,
    BOARD_INIT_PICO_ERROR = -2,
    BOARD_INIT_LCD_ERROR = -3,
    BOARD_INIT_USB_ERROR = -4,
} board_init_result_t;

board_init_result_t board_init_step_result(int event_rc, int pico_rc, int lcd_rc, int usb_rc) {
    if (event_rc != 0) {
        return BOARD_INIT_EVENT_ERROR;
    }

    if (pico_rc != 0) {
        return BOARD_INIT_PICO_ERROR;
    }

    if (lcd_rc != 0) {
        return BOARD_INIT_LCD_ERROR;
    }

    if (usb_rc != 0) {
        return BOARD_INIT_USB_ERROR;
    }

    return BOARD_INIT_OK;
}


// Function to initialize the board and declare program metadata
int initialize_board(){
    int event_rc;
    int pico_rc;
    int lcd_rc;
    int usb_rc;

    // Declare program description for binary info
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    event_rc = initialize_ev_data();
    if (event_rc != 0) {
        return board_init_step_result(event_rc, 0, 0, 0);
    }

    // Initialize Pico module and check for errors
    pico_rc = initialize_pico_module();
    if (pico_rc != 0) {
        return board_init_step_result(0, pico_rc, 0, 0);
    }

    lcd_rc = initialize_lcd_module();
    if (lcd_rc != 0) {
        return board_init_step_result(0, 0, lcd_rc, 0);
    }

    usb_rc = initialize_usb_module();
    if (usb_rc != 0) {
        return board_init_step_result(0, 0, 0, usb_rc);
    }

    return board_init_step_result(0, 0, 0, 0);
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
#ifndef HOST_TEST
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
#else
    return initialize_board();
#endif
}
