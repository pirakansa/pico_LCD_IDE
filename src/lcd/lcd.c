#include "GUI_Paint.h"
#include "LCD_1in3.h"
#include "DEV_Config.h"
#include "Infrared.h"
#include "gpios.h"
#include "DrawData.h"

#include "pico/mutex.h"
#include "lcd.h"

#define GPIO_KEY_UX_PUSH_WAIT 250

int initialize_liblcd();
int initialize_lcd_event();

UWORD *BlackImage;
volatile int select_menu_idx;
static lcd_callback_t lcd_event_callback = NULL;

auto_init_mutex(counter_mutex);



int initialize_liblcd(){
    if(DEV_Module_Init()!=0){
        return -1;
    }

    DEV_SET_PWM(50);
    LCD_1IN3_Init(HORIZONTAL);
    LCD_1IN3_Clear(WHITE);

    SET_Infrared_PIN(GPIO_KEY_A);
    SET_Infrared_PIN(GPIO_KEY_B);
    SET_Infrared_PIN(GPIO_KEY_X);
    SET_Infrared_PIN(GPIO_KEY_Y);
		 
	SET_Infrared_PIN(GPIO_KEY_UP);
    SET_Infrared_PIN(GPIO_KEY_DOWN);
    SET_Infrared_PIN(GPIO_KEY_LEFT);
    SET_Infrared_PIN(GPIO_KEY_RIGHT);
    SET_Infrared_PIN(GPIO_KEY_CTRL);

    return 0;
}

int initialize_lcd_module(){
    return initialize_liblcd();
}


void gpio_callback(uint gpio, uint32_t events) {
    uint32_t owner;
    if (!mutex_try_enter(&counter_mutex,&owner)) {
        printf("Mutex is already locked\n");
        return;
    };
    printf("GPIO EV %d %d\n", gpio, events);

    if( (GPIO_KEY_UP==gpio) && (GPIO_KEY_EVENTS_EDGE_RISE==events)){
        int menusCount = sizeof menu_lists / sizeof menu_lists[0];
        select_menu_idx = (select_menu_idx-1 < 0) ? menusCount-1 : select_menu_idx-1;
        draw_radio_menu_screen(BlackImage, select_menu_idx);
    } else if( (GPIO_KEY_DOWN==gpio) && (GPIO_KEY_EVENTS_EDGE_RISE==events)){
        int menusCount = sizeof menu_lists / sizeof menu_lists[0];
        select_menu_idx = (menusCount-1 < select_menu_idx+1) ? 0 : select_menu_idx+1;
        draw_radio_menu_screen(BlackImage, select_menu_idx);
    } else if( (GPIO_KEY_A==gpio) && (GPIO_KEY_EVENTS_EDGE_RISE==events)){
        if (lcd_event_callback) {
            lcd_event_callback(STACKEVENTS_BTN1);
        }
    } else if( (GPIO_KEY_B==gpio) && (GPIO_KEY_EVENTS_EDGE_RISE==events)){
        if (lcd_event_callback) {
            lcd_event_callback(STACKEVENTS_BTN2);
        }
    }
    mutex_exit(&counter_mutex);
}

int initialize_lcd_event(){
    // The IO IRQs are independent per-processor. This method affects only the processor that calls the function.
    // - https://www.raspberrypi.com/documentation/pico-sdk/hardware.html
    gpio_set_irq_enabled_with_callback(
        GPIO_KEY_UP,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true,
        &gpio_callback
    );
    gpio_set_irq_enabled(
        GPIO_KEY_DOWN,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_LEFT,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_RIGHT,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_A,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_B,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_X,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        GPIO_KEY_Y,
        GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE,
        true
    );

    return 0;
}

int initialize_lcd_draw(lcd_callback_t callback) {
    lcd_event_callback = callback;

    UDOUBLE Imagesize = LCD_1IN3_HEIGHT * LCD_1IN3_WIDTH * 2;
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);

    draw_splash_screen(BlackImage);
    DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);
    
    select_menu_idx = 0;
    draw_menu_screen(BlackImage);
    initialize_lcd_event();
    draw_radio_menu_screen(BlackImage, select_menu_idx);

    return 0;
}
