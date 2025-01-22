#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "pico/pico.h"


int initialize_board(){
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    if(initialize_pico_module()!=0){
        return -1;
    }

    return 0;
}



int main ()
{
     if(initialize_board()!=0){
        return 0;
     }

    stdio_init_all();
    printf("Hello, world!\n");
    
    while(true){}

    return 0;
}
