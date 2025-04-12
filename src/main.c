#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "pico/pico.h"

// Function to initialize the board and declare program metadata
int initialize_board(){
    // Declare program description for binary info
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    // Initialize Pico module and check for errors
    if(initialize_pico_module()!=0){
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
    stdio_init_all();
    printf("Hello, world!\n");

    bool hoge=false;
    
    while(true){
        // Set the LED state based on the BOOTSEL button status
        pico_set_led(get_bootsel_button());
        // hoge = !hoge; // Uncomment to toggle the state of 'hoge'
        sleep_ms(1000); // Wait for 1 second
    }

    return 0;
}
