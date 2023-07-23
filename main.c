#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "LCD_1in3.h"
#include "Infrared.h"
#include "DrawData.h"

#include "./wifi.h"

#include "pico/cyw43_arch.h"

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}

#include "hardware/vreg.h"
#include "hardware/clocks.h"


#define GPIO_KEY_A 15
#define GPIO_KEY_B 17
#define GPIO_KEY_X 19
#define GPIO_KEY_Y 21

#define GPIO_KEY_UP 2
#define GPIO_KEY_DOWN 18
#define GPIO_KEY_LEFT 16
#define GPIO_KEY_RIGHT 20
#define GPIO_KEY_CTRL 3

#define GPIO_KEY_EV_PUSH 0
#define GPIO_KEY_UX_PUSH_WAIT 250

UWORD *BlackImage;
volatile int select_menu_idx = 0;

#define GPIO_KEY_EVENTS_LEVEL_LOW 0x1
#define GPIO_KEY_EVENTS_LEVEL_HIGH 0x2
#define GPIO_KEY_EVENTS_EDGE_FALL 0x4
#define GPIO_KEY_EVENTS_EDGE_RISE 0x8



void gpio_callback(uint gpio, uint32_t events) {
    printf("GPIO EV %d %d\n", gpio, events);

    // if( (GPIO_KEY_UP==gpio) && (GPIO_KEY_EVENTS_EDGE_FALL==events)){
    //     int menusCount = sizeof menu_lists / sizeof menu_lists[0];
    //     select_menu_idx = (select_menu_idx-1 < 0) ? menusCount-1 : select_menu_idx-1;
    //     draw_radio_menu_screen(BlackImage, select_menu_idx);
    //     // DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);
    // }
    // if( (GPIO_KEY_DOWN==gpio) && (GPIO_KEY_EVENTS_EDGE_FALL==events)){
    //     int menusCount = sizeof menu_lists / sizeof menu_lists[0];
    //     select_menu_idx = (menusCount-1 < select_menu_idx+1) ? 0 : select_menu_idx+1;
    //     draw_radio_menu_screen(BlackImage, select_menu_idx);
    //     // DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);
    // }
}


int initialize_board(){
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    stdio_init_all();
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

int initialize_event(){
    // gpio_set_irq_enabled_with_callback(GPIO_KEY_UP, GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE, true, &gpio_callback);
    // gpio_set_irq_enabled_with_callback(GPIO_KEY_DOWN, GPIO_KEY_EVENTS_EDGE_FALL|GPIO_KEY_EVENTS_EDGE_RISE, true, &gpio_callback);

    return 0;
}

int main ()
{
     if(initialize_board()!=0){
        return 0;
     }

    UDOUBLE Imagesize = LCD_1IN3_HEIGHT * LCD_1IN3_WIDTH * 2;
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return 0;
    }
    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);
    
    draw_splash_screen(BlackImage);
    DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);

    draw_menu_screen(BlackImage);
    initialize_event();
    draw_radio_menu_screen(BlackImage, select_menu_idx);

    int led_stat = 0;

    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return 1;
    }

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    // Get notified if the user presses a key
    state->context = cyw43_arch_async_context();
    key_pressed_worker.user_data = state;
    async_context_add_when_pending_worker(cyw43_arch_async_context(), &key_pressed_worker);
    stdio_set_chars_available_callback(key_pressed_func, state);

    const char *ap_name = "picow_test";
#if 0
    const char *password = "password";
#else
    const char *password = NULL;
#endif

    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

    ip4_addr_t mask;
    IP4_ADDR(ip_2_ip4(&state->gw), 192, 168, 4, 1);
    IP4_ADDR(ip_2_ip4(&mask), 255, 255, 255, 0);

    // Start the dhcp server
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &state->gw, &mask);

    // Start the dns server
    dns_server_t dns_server;
    dns_server_init(&dns_server, &state->gw);

    if (!tcp_server_open(state, ap_name)) {
        DEBUG_printf("failed to open server\n");
        return 1;
    }

    state->complete = false;

    while(1){
        if(DEV_Digital_Read(GPIO_KEY_UP) == GPIO_KEY_EV_PUSH){
            int menusCount = sizeof menu_lists / sizeof menu_lists[0];
            select_menu_idx = (select_menu_idx-1 < 0) ? menusCount-1 : select_menu_idx-1;
            draw_radio_menu_screen(BlackImage, select_menu_idx);
            printf("gpio =%d\r\n",GPIO_KEY_UP);
            DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);
        }
        if(DEV_Digital_Read(GPIO_KEY_DOWN) == GPIO_KEY_EV_PUSH){
            int menusCount = sizeof menu_lists / sizeof menu_lists[0];
            select_menu_idx = (menusCount-1 < select_menu_idx+1) ? 0 : select_menu_idx+1;
            draw_radio_menu_screen(BlackImage, select_menu_idx);
            printf("gpio =%d\r\n",GPIO_KEY_DOWN);
            DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);
        }
        if(select_menu_idx == 0){
            led_stat = (led_stat+1) % 10;
            cyw43_gpio_set(&cyw43_state, LED_GPIO, (led_stat < 5));
            DEV_Delay_ms(100);
        }
        if(select_menu_idx == 1){
            // the following #ifdef is only here so this same example can be used in multiple modes;
            // you do not need it in your code
#if PICO_CYW43_ARCH_POLL
            // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
            // main loop (not from a timer interrupt) to check for Wi-Fi driver or lwIP work that needs to be done.
            cyw43_arch_poll();
            // you can poll as often as you like, however if you have nothing else to do you can
            // choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
            cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
#else
            // if you are not using pico_cyw43_arch_poll, then Wi-FI driver and lwIP work
            // is done via interrupt in the background. This sleep is just an example of some (blocking)
            // work you might be doing.
            sleep_ms(1000);
#endif
        } else {
            // sleep_ms(1000);
            // state->complete = false;
        }

    }

    tcp_server_close(state);
    dns_server_deinit(&dns_server);
    dhcp_server_deinit(&dhcp_server);
    cyw43_arch_deinit();
    free(BlackImage);
    BlackImage = NULL;
    
    DEV_Module_Exit();
    return 0;
}
