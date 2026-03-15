#include "lcd_internal.h"

#ifndef HOST_TEST
#include "DrawData.h"
#endif

#define GPIO_KEY_UX_PUSH_WAIT 250

static UWORD *black_image;
static int selected_menu_index;
static lcd_callback_t lcd_event_callback;

static int initialize_liblcd(void) {
    if (DEV_Module_Init() != 0) {
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

static int allocate_lcd_buffer(void) {
    UDOUBLE image_size = LCD_1IN3_HEIGHT * LCD_1IN3_WIDTH * 2;

    black_image = (UWORD *)malloc(image_size);
    if (black_image == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }

    return 0;
}

static void draw_initial_screens(void) {
    Paint_NewImage((UBYTE *)black_image, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);

    draw_splash_screen(black_image);
    DEV_Delay_ms(GPIO_KEY_UX_PUSH_WAIT);

    selected_menu_index = 0;
    draw_menu_screen(black_image);
    draw_radio_menu_screen(black_image, selected_menu_index);
}

int lcd_menu_next_index(int current, int direction, int menu_count) {
    if (menu_count <= 0) {
        return 0;
    }

    if (direction < 0) {
        return (current - 1 < 0) ? menu_count - 1 : current - 1;
    }

    if (direction > 0) {
        return (menu_count - 1 < current + 1) ? 0 : current + 1;
    }

    return current;
}

int initialize_lcd_module(void) {
    return initialize_liblcd();
}

int initialize_lcd_draw(lcd_callback_t callback) {
    lcd_event_callback = callback;

    if (allocate_lcd_buffer() != 0) {
        return -1;
    }

    draw_initial_screens();
    initialize_lcd_event();

    return 0;
}

UWORD *lcd_current_image(void) {
    return black_image;
}

int lcd_current_menu_index(void) {
    return selected_menu_index;
}

void lcd_set_current_menu_index(int index) {
    selected_menu_index = index;
}

int lcd_current_menu_count(void) {
    return LCD_MENU_COUNT;
}

lcd_callback_t lcd_current_callback(void) {
    return lcd_event_callback;
}

#ifdef HOST_TEST
void *lcd_test_image_buffer(void) {
    return black_image;
}

int lcd_test_selected_menu_index(void) {
    return selected_menu_index;
}

void lcd_test_set_selected_menu_index(int index) {
    selected_menu_index = index;
}

void lcd_test_reset_runtime_state(void) {
    free(black_image);
    black_image = NULL;
    selected_menu_index = 0;
    lcd_event_callback = NULL;
}
#endif
