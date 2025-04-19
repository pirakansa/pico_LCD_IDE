#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "pico/pico.h"
#include "lcd/lcd.h"
#include "usb/usb.h"


// Function to initialize the board and declare program metadata
int initialize_board(){
    // Declare program description for binary info
    bi_decl(bi_program_description("https://github.com/pirakansa"));

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


int main ()
{
     // Initialize the board and exit if initialization fails
     if(initialize_board()!=0){
        return 0;
     }

    // Initialize standard I/O for debugging and communication
    // stdio_init_all();
    set_started_led_signal();

    if(initialize_lcd_draw()!=0){
        set_err_led_signal(2);
    }

    while (true) {
        usb_device_task();
        usb_hid_task(get_bootsel_button_state());

        // if(tud_hid_ready()) {
        //     // char buf[] = {0x41};
        //     // tud_hid_n_report(0, 0, buf, 1);
        //     // multiple reports
        //     // char buf[] = {0x48, 0x45, 0x4c, 0x4c, 0x4f};    // HELLO
        //     // 
        //     // tud_hid_n_report(0, 0, buf, sizeof(buf));
        //     // uint8_t keycode[6] = {0};
        //     // keycode[0] = 0x04; // HID keycode for 'A'
        //     // tud_hid_keyboard_report(0, 0, keycode);
        //     // sleep_ms(1000);
        //     // keycode[0] = 0;
        //     // tud_hid_keyboard_report(0, 0, keycode);
        //     // sleep_ms(1000);
        // }

        // printf("%d\n", get_bootsel_button_state());
        // sleep_ms(1000); // Wait for 1 second
    }

    return 0;
}





