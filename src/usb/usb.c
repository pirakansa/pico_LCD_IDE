#include "./usb.h"
#include "tusb.h"
#include "dev_hid_composite/usb_descriptors.h"
#include "dev_lowlevel/usb_common.h"

#include "bsp/board_api.h"

// Descriptors
tusb_desc_device_t const device_descriptor = {
    .bLength         = sizeof(tusb_desc_device_t),
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB          = 0x0110,      // USB 1.1 device
    .bDeviceClass    = 0x00,        // Specified in interface descriptor
    .bDeviceSubClass = 0x00,        // No subclass
    .bDeviceProtocol = 0x00,        // No protocol
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,     // Max packet size for ep0
    .idVendor        = 0x1234,      // Your vendor id
    .idProduct       = 0xABCD,      // Your product ID
    .bcdDevice       = 0x0000,      // No device revision number
    .iManufacturer   = 0x01,        // Manufacturer string index
    .iProduct        = 0x01,        // Product string index
    .iSerialNumber = 0x00,          // No serial number
    .bNumConfigurations = 0x01      // One configuration
};

const uint8_t hid_report_desc[] = {
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(REPORT_ID_KEYBOARD ))
};

const uint8_t desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN, 0, 0x0F),
    TUD_HID_INOUT_DESCRIPTOR(1, 0, 0, sizeof(hid_report_desc), 0x01, 0x81, 64, 0x0F)
};

const uint16_t string_desc_lang[] = { // Index: 0
    4 | (3 << 8), // bLength & bDescriptorType
    0x411 // ja-JP
};
const uint16_t string_desc_product[] = { // Index: 1
    (2 + 2 * 9) | (3 << 8),
    'P', 'i', 'r', 'a', 's', 'p', 'i', 'c', 'o'
};

uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &device_descriptor;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    return desc_configuration;
}

enum {
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
};  

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    uint16_t const *ret = NULL;
    switch(index) {
        case STRID_LANGID:
            ret = string_desc_lang;
            break;
        case STRID_MANUFACTURER:
            ret = string_desc_product;
            break;
        case STRID_PRODUCT:
            return string_desc_product;
        default:
            break;
    }
    return ret;
}

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    return hid_report_desc;
}
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    return 0;
}
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    return;
}


int initialize_usb_module(){
    return (tud_init(BOARD_TUD_RHPORT) == true) ? 0 : -1;
}

void usb_device_task(){
    // tinyusb device task
    tud_task();
}

static void send_hid_report(uint8_t report_id, uint32_t btn)
{
    // skip if hid is not ready yet
    if ( !tud_hid_ready() ) return;

    switch(report_id){
        case REPORT_ID_KEYBOARD:
            {
                // use to avoid send multiple consecutive zero report for keyboard
                static bool has_keyboard_key = false;

                if ( btn ){
                    uint8_t keycode[6] = { 0 };
                    keycode[0] = HID_KEY_A;

                    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
                    has_keyboard_key = true;
                }else{
                    // send empty key report if previously has key pressed
                    if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                    has_keyboard_key = false;
                }
            }
            break;
        default:
            break;
    }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void usb_hid_task(bool const btn){
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    // Remote wakeup
    if ( tud_suspended() && btn ){
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }else{
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        send_hid_report(REPORT_ID_KEYBOARD, btn);
    }
}


