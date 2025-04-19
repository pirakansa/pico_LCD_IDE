#ifndef _PICO_LCD_SRC_USB_USB_
#define _PICO_LCD_SRC_USB_USB_

#include <stdbool.h>

void usb_device_task();
void usb_hid_task(bool const btn);
int initialize_usb_module();
void hid_task();

#endif


