#ifndef _PICO_LCD_SRC_USB_USB_
#define _PICO_LCD_SRC_USB_USB_

#include <stdbool.h>
#include "../events/events.h"

typedef stackevents_dt (*get_new_event_t)(void);
typedef int (*usb_menu_id_provider_t)(void);
int initialize_usb_module();
void usb_device_task();
void usb_hid_task(get_new_event_t func);
void usb_set_menu_id_provider(usb_menu_id_provider_t provider);
const char *usb_event_text(stackevents_dt ev);

#endif
