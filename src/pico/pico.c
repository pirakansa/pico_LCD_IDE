#include "pico/stdlib.h"
#include "boards/pico.h"

#ifdef PICO_CYW43_SUPPORTED
#include "pico/cyw43_arch.h"
#endif

#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

#include "./pico.h"

// Initializes the Pico library, including LED setup.
int initialize_libpico();

// Initializes the LED for the Pico board.
int pico_led_init();

// Sets the state of the onboard LED (on/off).
void pico_set_led(bool);

/**
 * Reads the state of the BOOTSEL button.
 * This function temporarily disables flash access to safely read the button state.
 * Returns true if the button is pressed, false otherwise.
 */
bool __no_inline_not_in_flash_func(get_bootsel_button_state)() {
    const uint CS_PIN_INDEX = 1;

    // Must disable interrupts, as interrupt handlers may be in flash, and we
    // are about to temporarily disable flash access!
    uint32_t flags = save_and_disable_interrupts();

    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    // Note we can't call into any sleep functions in flash right now
    for (volatile int i = 0; i < 1000; ++i);

    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
// #if PICO_RP2040
    #define CS_BIT (1u << 1)
// #else
    // #define CS_BIT SIO_GPIO_HI_IN_QSPI_CSN_BITS
// #endif
    bool button_state = !(sio_hw->gpio_hi_in & CS_BIT);

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return button_state;
}

/**
 * Sets the state of the onboard LED.
 * If `led_on` is true, the LED is turned on; otherwise, it is turned off.
 */
void pico_set_led(bool led_on) {
#ifdef PICO_CYW43_SUPPORTED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#else
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#endif
}

/**
 * Turns on the onboard LED to signal that the system has started.
 */
void set_started_led_signal() {
    pico_set_led(true);
}

/**
 * Blinks the onboard LED to indicate an error.
 * The LED blinks `count` times, pauses, and repeats indefinitely.
 */
void set_err_led_signal(int count) {
    while(true){
        for (int i = 0; i < count ; i++) {
            pico_set_led(true);
            sleep_ms(500);
            pico_set_led(false);
            sleep_ms(500);
        }
        sleep_ms(2000);
    }
}

/**
 * Initializes the onboard LED.
 * Returns PICO_OK if successful, or an error code otherwise.
 */
int pico_led_init(void) {
#ifdef PICO_CYW43_SUPPORTED
    return cyw43_arch_init();
#else
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#endif
}

/**
 * Initializes the Pico library and sets up the onboard LED.
 * Returns 0 if successful.
 */
int initialize_libpico(){
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    pico_set_led(false);

    return 0;
}

/**
 * Initializes the Pico module by calling `initialize_libpico`.
 * Returns 0 if successful.
 */
int initialize_pico_module(){
    return initialize_libpico();
}



