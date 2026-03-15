# LCD Input Reference

This reference describes the current LCD input handling implemented in `src/lcd/lcd.c`.

## Scope

The LCD module handles two related responsibilities:

- display initialization and menu drawing
- GPIO interrupt handling for the Pico-LCD-1.3 button inputs

This document focuses on the input side and the callback contract exposed to the rest of the firmware.

## Public Interface

Declared in `src/lcd/lcd.h`:

- `typedef void (*lcd_callback_t)(stackevents_dt ev);`
- `int initialize_lcd_module();`
- `int initialize_lcd_draw(lcd_callback_t callback);`

`lcd_callback_t` is used by the LCD module to publish selected button presses as shared firmware events.

## Initialization Split

The LCD setup is divided into two stages:

### `initialize_lcd_module()`

This performs low-level device setup:

- calls `DEV_Module_Init()`
- sets PWM brightness to `50`
- initializes the LCD in `HORIZONTAL` mode
- clears the display to `WHITE`
- configures the board key and infrared-related pins through `SET_Infrared_PIN(...)`

Return values:

- `0` on success
- `-1` if `DEV_Module_Init()` fails

### `initialize_lcd_draw(lcd_callback_t callback)`

This performs UI and input startup:

1. stores the provided callback in `lcd_event_callback`
2. allocates a framebuffer sized as `LCD_1IN3_HEIGHT * LCD_1IN3_WIDTH * 2`
3. initializes the paint buffer
4. draws the splash screen
5. delays for `250 ms`
6. resets the selected menu index to `0`
7. draws the menu screen
8. enables GPIO interrupts
9. draws the initial radio-menu selection

Return values:

- `0` on success
- `-1` if framebuffer allocation fails

## Interrupt Registration

`initialize_lcd_event()` enables GPIO interrupts for these inputs:

- `GPIO_KEY_UP`
- `GPIO_KEY_DOWN`
- `GPIO_KEY_LEFT`
- `GPIO_KEY_RIGHT`
- `GPIO_KEY_A`
- `GPIO_KEY_B`
- `GPIO_KEY_X`
- `GPIO_KEY_Y`

Each listed key is configured for both rise and fall edge notifications.

The callback registration is anchored by `gpio_set_irq_enabled_with_callback(...)` on `GPIO_KEY_UP`, and the remaining keys are enabled with `gpio_set_irq_enabled(...)`.

## GPIO Callback Behavior

The shared ISR-style handler is `gpio_callback(uint gpio, uint32_t events)`.

It first attempts to acquire `counter_mutex` using `mutex_try_enter(...)`.
If the mutex is already locked:

- the handler prints `Mutex is already locked`
- the current GPIO event is dropped
- no redraw or event publication occurs

If the mutex is acquired, the handler prints the GPIO and event values and then applies the current mapping.

## Input Mapping

The current behavior only reacts to `GPIO_KEY_EVENTS_EDGE_RISE`.
Configured fall-edge interrupts do not currently trigger any state change by themselves.

### Navigation Keys

- `UP`: decrements `select_menu_idx` with wraparound and redraws the radio-menu screen
- `DOWN`: increments `select_menu_idx` with wraparound and redraws the radio-menu screen

The number of menu entries is derived from `menu_lists`.

### Action Keys

- `A`: publishes `STACKEVENTS_BTN1`
- `B`: publishes `STACKEVENTS_BTN2`
- `X`: publishes `STACKEVENTS_BTN3`
- `Y`: publishes `STACKEVENTS_BTN4`

These action events are only emitted if a non-null callback has been registered.

### Currently Unused Enabled Keys

- `LEFT`: interrupt enabled, no implemented callback behavior
- `RIGHT`: interrupt enabled, no implemented callback behavior

`GPIO_KEY_CTRL` is configured during low-level setup but is not enabled in `initialize_lcd_event()`.

## State Managed By The Module

The LCD input path currently owns or updates:

- `BlackImage`: the allocated framebuffer pointer
- `select_menu_idx`: the current menu selection index
- `lcd_event_callback`: the upper-layer event callback

## Integration Contract

The rest of the firmware is expected to:

- call `initialize_lcd_module()` before `initialize_lcd_draw()`
- pass a valid `lcd_callback_t` to receive button events
- tolerate dropped input when the callback path is contended by the mutex guard

The LCD module does not enqueue events directly. It only invokes the callback supplied by its caller.

## Constraints and Current Limitations

- Only `UP`, `DOWN`, `A`, `B`, `X`, and `Y` have implemented behavior.
- Fall-edge interrupts are enabled but ignored by the current mapping logic.
- The framebuffer allocation is not released during normal execution.
- There is no debounce layer beyond the present edge-triggered logic and mutex gate.

## Related Documents

- `docs/module-overview.md`
- `docs/user-guides/startup-flow.md`
- `docs/reference/events-queue.md`