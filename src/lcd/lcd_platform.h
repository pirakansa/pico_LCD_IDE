#ifndef _PICO_LCD_SRC_LCD_PLATFORM_
#define _PICO_LCD_SRC_LCD_PLATFORM_

#ifdef HOST_TEST
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gpios.h"
#include "lcd.h"

typedef unsigned int uint;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;
typedef unsigned int UDOUBLE;

typedef struct {
    UWORD WIDTH;
    UWORD HEIGHT;
} lcd_1in3_t;

typedef struct {
    int unused;
} mutex_t;

extern lcd_1in3_t LCD_1IN3;
extern int menu_count;

int DEV_Module_Init(void);
void DEV_SET_PWM(int pwm);
void LCD_1IN3_Init(int orientation);
void LCD_1IN3_Clear(UWORD color);
void SET_Infrared_PIN(int pin);
void Paint_NewImage(UBYTE *image, UWORD width, UWORD height, int rotate, UWORD color);
void Paint_SetScale(int scale);
void Paint_SetRotate(int rotate);
void Paint_Clear(UWORD color);
int draw_splash_screen(UWORD *image);
int draw_menu_screen(UWORD *image);
int draw_radio_menu_screen(UWORD *image, int selected);
void DEV_Delay_ms(int ms);
uint32_t time_us_32(void);
int mutex_try_enter(void *mutex, uint32_t *owner);
void mutex_exit(void *mutex);
void gpio_set_irq_enabled_with_callback(uint gpio, uint32_t events, _Bool enabled, void (*callback)(uint, uint32_t));
void gpio_set_irq_enabled(uint gpio, uint32_t events, _Bool enabled);

#define auto_init_mutex(name) void *name = NULL
#define WHITE 0
#define HORIZONTAL 0
#define ROTATE_0 0
#define LCD_1IN3_HEIGHT 240
#define LCD_1IN3_WIDTH 240
#define LCD_MENU_COUNT menu_count
#else
#include "GUI_Paint.h"
#include "LCD_1in3.h"
#include "DEV_Config.h"
#include "Infrared.h"
#include "gpios.h"

#include "pico/mutex.h"
#include "lcd.h"

#define LCD_MENU_COUNT ((int)(sizeof menu_lists / sizeof menu_lists[0]))
#endif

#endif
