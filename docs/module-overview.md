# Module Overview

This document describes the current firmware structure implemented under `src/`.
It is implementation-driven documentation: the code is the source of truth.

## High-Level Architecture

The firmware is split into a small set of focused modules:

- `main`: startup order and module orchestration
- `events`: shared event queue between producers and consumers
- `pico`: Pico board-specific initialization and status signals
- `lcd`: LCD initialization, menu drawing, and GPIO-driven button handling
- `usb`: TinyUSB device setup and HID keyboard output
- `tests`: host-side checks for selected logic paths

The current runtime flow is:

1. Initialize the event queue.
2. Initialize Pico board support.
3. Initialize the LCD driver.
4. Initialize the USB device stack.
5. Start LCD drawing and register the LCD event callback.
6. Enter the main loop and repeatedly run USB device and HID tasks.

## `main`

Primary file: `src/main.c`

`main` is the composition layer for the firmware.
It does not implement device-specific logic directly. Instead, it wires the modules together and controls startup order.

Current responsibilities:

- Declare binary metadata for the firmware image.
- Initialize the event queue before any producers or consumers are active.
- Initialize the Pico, LCD, and USB modules in sequence.
- Register the LCD event callback used to forward button events into the queue.
- Provide the event source callback consumed by the USB HID layer.
- Enter the infinite runtime loop.

Important behaviors:

- If queue initialization or any module initialization fails, startup returns early.
- If LCD drawing setup fails, the firmware switches to the error LED signal.
- If the event queue overflows, an error message is printed and the error LED signal is triggered.

## `events`

Primary files: `src/events/events.h`, `src/events/events.c`

`events` provides a mutex-protected FIFO queue for cross-module event exchange.

### Event Types

The current event enum is:

- `STACKEVENTS_NONE`: no event available
- `STACKEVENTS_FULL`: queue overflow sentinel
- `STACKEVENTS_INTERRUPT`: interrupt-style wake signal
- `STACKEVENTS_BTN1`
- `STACKEVENTS_BTN2`
- `STACKEVENTS_BTN3`
- `STACKEVENTS_BTN4`

### Queue Behavior

- Queue size is currently fixed at 10 entries.
- `initialize_ev_data()` resets head, tail, and count to an empty state.
- `enqueue()` appends one event and returns the inserted event on success.
- `enqueue()` returns `STACKEVENTS_FULL` on overflow and does not advance the queue.
- `dequeue()` returns the oldest queued event.
- `dequeue()` returns `STACKEVENTS_NONE` on underflow.

### Why This Module Exists

The queue decouples event producers from event consumers:

- The LCD module can push button events asynchronously.
- The USB HID task can pull events on its own polling schedule.

This keeps the HID layer independent from GPIO interrupt timing.

## `pico`

Primary files: `src/pico/pico.h`, `src/pico/pico.c`

`pico` contains board-specific helpers for the Raspberry Pi Pico platform.

Current responsibilities:

- Initialize the onboard LED support.
- Provide a steady "started" LED indication.
- Provide a repeating error LED blink pattern.
- Read the BOOTSEL button safely from QSPI-related hardware registers.

### BOOTSEL Handling

`get_bootsel_button_state()` temporarily disables interrupts and reads the QSPI chip-select state directly.
This is necessary because the BOOTSEL button shares hardware paths that require careful flash access handling.

### LED Signaling

- `set_started_led_signal()` turns the onboard LED on.
- `set_err_led_signal(count)` enters an infinite blink loop with `count` blinks per cycle.

Because the error signal loops forever, it is a terminal state for the current firmware flow.

## `lcd`

Primary files: `src/lcd/lcd.h`, `src/lcd/lcd.c`

`lcd` is responsible for display initialization, basic screen drawing, and GPIO button event handling for the Pico-LCD-1.3 hardware.

Current responsibilities:

- Initialize the Waveshare display support library.
- Configure display brightness and orientation.
- Set up the infrared and key GPIO pins used by the board support code.
- Allocate the framebuffer used by the drawing layer.
- Draw the splash screen and menu UI.
- Register GPIO interrupts for key handling.
- Convert selected key presses into shared events.

### UI and Input Behavior

During `initialize_lcd_draw()` the module:

1. Stores the callback supplied by `main`.
2. Allocates a framebuffer.
3. Draws a splash screen.
4. Delays briefly.
5. Draws the menu screen.
6. Enables GPIO interrupts.
7. Draws the initial radio-menu selection.

### Button Mapping

The current callback behavior is:

- `UP`: move the selected menu item up and redraw
- `DOWN`: move the selected menu item down and redraw
- `A`: emit `STACKEVENTS_BTN1`
- `B`: emit `STACKEVENTS_BTN2`
- `X`: emit `STACKEVENTS_BTN3`
- `Y`: emit `STACKEVENTS_BTN4`

Notes about the present implementation:

- `LEFT` and `RIGHT` interrupts are enabled, but no action is currently implemented in the GPIO callback.
- The callback uses a mutex guard and drops the GPIO event if the mutex is already locked.
- The framebuffer allocated in `initialize_lcd_draw()` is not currently freed during normal execution.

## `usb`

Primary files: `src/usb/usb.h`, `src/usb/usb.c`

`usb` implements a TinyUSB device that exposes a HID keyboard interface.

Current responsibilities:

- Initialize TinyUSB.
- Publish device, configuration, string, and HID report descriptors.
- Run the TinyUSB device task.
- Poll for new events on a fixed interval.
- Convert button events into keyboard output sent to the host.

### HID Polling Model

`usb_hid_task()` polls every 10 ms.
On each eligible tick it asks the caller for one event using the function pointer supplied by `main`.

Behavior:

- If USB is suspended and the event is `STACKEVENTS_INTERRUPT`, the module requests remote wakeup.
- Otherwise it sends a keyboard report for the current event.

### Current Output Mapping

The HID keyboard payloads are currently hard-coded:

- `BTN1`: types `mail@example.com`
- `BTN2`: types `btn2 click!`
- `BTN3`: types `btn3 click!`
- `BTN4`: types `btn4 click!`

This means the firmware currently behaves like a small USB keyboard macro device driven by the LCD board buttons.

## `tests`

Primary files: `src/events/tests/test_events.c`, `src/tests/test_main.c`

The repository includes host-side tests and compile checks for logic that does not require real hardware access.

Current coverage includes:

- event queue initialization, FIFO behavior, wraparound, and overflow handling
- startup sequencing behavior in `main`
- host-test-compatible compilation paths guarded by `HOST_TEST`

These tests do not verify real LCD, GPIO, or USB behavior on hardware.

## Module Interaction Summary

- `lcd` produces button events.
- `main` forwards those events into `events`.
- `usb` consumes events from `main` through a callback.
- `pico` provides board signals and BOOTSEL state.
- `tests` verify parts of the logic in a host environment.

In practical terms, this firmware is currently a button-driven Pico-LCD interface that sends predefined USB HID keyboard strings to a connected host.