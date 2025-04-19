#ifndef _PICO_LCD_SRC_USB_USB_
#define _PICO_LCD_SRC_USB_USB_

#include <stdbool.h>

int initialize_usb_module();
void usb_device_task();
void usb_hid_task(bool const btn);

#endif


