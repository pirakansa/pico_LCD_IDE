# UI Preview Guide

This guide explains how to render the current LCD UI on the host without flashing firmware to a Raspberry Pi Pico.

## Purpose

`vorbere run ui-preview` renders the existing LCD draw routines into image files on the development machine.

Use it when you want to:

- confirm screen layout changes before flashing hardware
- review menu selection states quickly
- keep a visual artifact that can be diffed in CI or code review

## Command

```sh
vorbere run ui-preview
```

The task writes PPM files under `build/ui-preview/`.

Current outputs:

- `build/ui-preview/splash.ppm`
- `build/ui-preview/menu_selected_0.ppm`
- `build/ui-preview/menu_selected_1.ppm`
- `build/ui-preview/menu_selected_2.ppm`
- `build/ui-preview/menu_selected_3.ppm`

PPM is a simple image format that many viewers can open directly.
If needed, it can also be converted with standard tools such as ImageMagick.

## What It Reuses

The preview path reuses the firmware draw routines in `src/lcd/DrawData.h`.
It adds a host-only compatibility layer for:

- `GUI_Paint`
- `LCD_1in3`
- framebuffer export to `.ppm`

This keeps the preview aligned with the current firmware UI instead of maintaining a second independent rendering implementation.

## Scope And Limits

The preview verifies draw output, not real hardware behavior.

It is useful for:

- splash image checks
- menu text placement
- radio-button selection states
- quick iteration on colors and spacing

It does not verify:

- SPI transfer behavior
- LCD controller initialization timing
- GPIO wiring
- backlight behavior
- runtime interaction on physical hardware

Hardware validation is still required before relying on the change in production use.
