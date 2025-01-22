#include <stdio.h>

#include "pico/binary_info.h"

#include "lcd/DrawData.h"
#include "lcd/lcd.h"
#include "pico/pico.h"


#include "hardware/vreg.h"
#include "hardware/clocks.h"


int initialize_board(){
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    if(initialize_pico_module()!=0){
        return -1;
    }
    if(initialize_lcd_module()!=0){
        return -1;
    }

    return 0;
}



int main ()
{
     if(initialize_board()!=0){
        return 0;
     }

    initialize_lcd_draw();

    while(1){
        // if(DEV_Digital_Read(GPIO_KEY_UP) == GPIO_KEY_EV_PUSH){
        //     int menusCount = sizeof menu_lists / sizeof menu_lists[0];
        //     select_menu_idx = (select_menu_idx-1 < 0) ? menusCount-1 : select_menu_idx-1;
        //     draw_radio_menu_screen(BlackImage, select_menu_idx);
        //     printf("gpio =%d\r\n",GPIO_KEY_UP);
        //     DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);
        // }
        // if(DEV_Digital_Read(GPIO_KEY_DOWN) == GPIO_KEY_EV_PUSH){
        //     int menusCount = sizeof menu_lists / sizeof menu_lists[0];
        //     select_menu_idx = (menusCount-1 < select_menu_idx+1) ? 0 : select_menu_idx+1;
        //     draw_radio_menu_screen(BlackImage, select_menu_idx);
        //     printf("gpio =%d\r\n",GPIO_KEY_DOWN);
        //     DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);
        // }
        // if(select_menu_idx == 0){
        //     led_stat = (led_stat+1) % 10;
        //     gpio_put(PICO_DEFAULT_LED_PIN, (led_stat < 5));
        //     DEV_Delay_ms(100);
        // }
        // if(select_menu_idx == 1){
        //     sleep_ms(1000);
        // } else {

        // }

    }

    // free(BlackImage);
    // BlackImage = NULL;
    
    // DEV_Module_Exit();
    return 0;
}
