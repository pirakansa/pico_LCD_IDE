#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "tusb.h"

#include "pico/pico.h"
#include "lcd/lcd.h"

#include "bsp/board_api.h"

enum
{
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_GAMEPAD,
  REPORT_ID_COUNT
};


void hid_task(void);

// // USB HID Keyboard Report Descriptor
// const uint8_t desc_hid_report[] = {
//     0x05, 0x01, // Usage Page (Generic Desktop Ctrls)
//     0x09, 0x06, // Usage (Keyboard)
//     0xA1, 0x01, // Collection (Application)
//     0x05, 0x07, // Usage Page (Kbrd/Keypad)
//     0x19, 0xE0, // Usage Minimum (0xE0)
//     0x29, 0xE7, // Usage Maximum (0xE7)
//     0x15, 0x00, // Logical Minimum (0)
//     0x25, 0x01, // Logical Maximum (1)
//     0x75, 0x01, // Report Size (1)
//     0x95, 0x08, // Report Count (8)
//     0x81, 0x02, // Input (Data,Var,Abs)
//     0x95, 0x01, // Report Count (1)
//     0x75, 0x08, // Report Size (8)
//     0x81, 0x01, // Input (Cnst,Var,Abs)
//     0x95, 0x05, // Report Count (5)
//     0x75, 0x01, // Report Size (1)
//     0x05, 0x08, // Usage Page (LEDs)
//     0x19, 0x01, // Usage Minimum (Num Lock)
//     0x29, 0x05, // Usage Maximum (Kana)
//     0x91, 0x02, // Output (Data,Var,Abs)
//     0x95, 0x01, // Report Count (1)
//     0x75, 0x03, // Report Size (3)
//     0x91, 0x01, // Output (Cnst,Var,Abs)
//     0x95, 0x06, // Report Count (6)
//     0x75, 0x08, // Report Size (8)
//     0x15, 0x00, // Logical Minimum (0)
//     0x25, 0x65, // Logical Maximum (101)
//     0x05, 0x07, // Usage Page (Kbrd/Keypad)
//     0x19, 0x00, // Usage Minimum (0x00)
//     0x29, 0x65, // Usage Maximum (0x65)
//     0x81, 0x00, // Input (Data,Array,Abs)
//     0xC0        // End Collection
// };

// void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len) {
//     // Callback after report is sent
// }

// uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
//     // Callback to handle GET_REPORT request
//     return 0;
// }

// void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
//     // Callback to handle SET_REPORT request
// }

const uint8_t device_desc[] = {
    18, // bLength
    1, // bDescriptorType
    0x10,
    0x01, // bcdUSB
    0x00, // bDeviceClass
    0x00, // bDeviceSubClass
    0x00, // bDeviceProtocol
    CFG_TUD_ENDPOINT0_SIZE, // bMaxPacketSize0
    0x34,
    0x12, // idVendor
    0xcd,
    0xab, // idProduct,
    0x00,
    0x00, // bcdDevice
    0x00, // iManufacturer
    0x01, // iProduct
    0x00, // iSerialNumber
    0x01, // bNumConfigurations
};

const uint8_t hid_report_desc[] = {
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(REPORT_ID_KEYBOARD         ))
};

const uint8_t conf_desc[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN, 0, 0x0F),
    TUD_HID_INOUT_DESCRIPTOR(1, 0, 0, sizeof(hid_report_desc), 0x01, 0x81, 64, 0x0F)
};

const uint16_t string_desc_lang[] = { // Index: 0
    4 | (3 << 8), // bLength & bDescriptorType
    0x411 // ja-JP
};
const uint16_t string_desc_product[] = { // Index: 1
    16 | (3 << 8),
    'R', 'a', 's', 'p', 'i', 'c', 'o'
};

uint8_t const *tud_descriptor_device_cb(void) {
    return device_desc;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    return conf_desc;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    uint16_t const *ret = NULL;
    switch(index) {
        case 0:
            ret = string_desc_lang;
            break;
        case 1:
            ret = string_desc_product;
            break;
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

// Function to initialize the board and declare program metadata
int initialize_board(){
    // Declare program description for binary info
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    // Initialize Pico module and check for errors
    if(initialize_pico_module()!=0){
        return -1;
    }

    if(initialize_lcd_module()!=0){
        return -1;
    }

    return 0;
}


int main ()
{
     // Initialize the board and exit if initialization fails
     if(initialize_board()!=0){
        return 0;
     }

    // Initialize standard I/O for debugging and communication
    // stdio_init_all();
    set_started_led_signal();

    if(initialize_lcd_draw()!=0){
        set_err_led_signal(2);
    }

    if(!tud_init(BOARD_TUD_RHPORT)){
        set_err_led_signal(3);
    }

    while (true) {
        tud_task(); // TinyUSB device task
        hid_task();

        // if(tud_hid_ready()) {
        //     // char buf[] = {0x41};
        //     // tud_hid_n_report(0, 0, buf, 1);
        //     // multiple reports
        //     // char buf[] = {0x48, 0x45, 0x4c, 0x4c, 0x4f};    // HELLO
        //     // 
        //     // tud_hid_n_report(0, 0, buf, sizeof(buf));
        //     // uint8_t keycode[6] = {0};
        //     // keycode[0] = 0x04; // HID keycode for 'A'
        //     // tud_hid_keyboard_report(0, 0, keycode);
        //     // sleep_ms(1000);
        //     // keycode[0] = 0;
        //     // tud_hid_keyboard_report(0, 0, keycode);
        //     // sleep_ms(1000);
        // }

        // printf("%d\n", get_bootsel_button_state());
        // sleep_ms(1000); // Wait for 1 second
    }

    return 0;
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
void hid_task(void){
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = get_bootsel_button_state();

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
