#ifndef _PICO_LCD_SRC_USB_USB_
#define _PICO_LCD_SRC_USB_USB_

#include "bsp/board_api.h"

void usb_device_task();
void usb_hid_task(uint32_t const btn);
int initialize_usb_module();
void hid_task();

#endif


