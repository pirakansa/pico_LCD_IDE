#ifndef _PICO_LCD_SRC_USB_USB_
#define _PICO_LCD_SRC_USB_USB_

#include <stdbool.h>
#include "../events/events.h"

typedef stackevents_dt (*get_new_event_t)(void);
int initialize_usb_module();
void usb_device_task();
void usb_hid_task(get_new_event_t func);

#endif
