# pico_LCD_IDE

This repository is a project utilizing the Raspberry Pi Pico and the Pico-LCD-1.3.

## Overview

This project demonstrates how to use the Raspberry Pi Pico microcontroller with the Waveshare Pico-LCD-1.3 display module. It includes features such as event handling, menu rendering, and USB HID communication.

### Key Features
- **LCD Display Integration**: Draw menus, splash screens, and other UI elements on the Pico-LCD-1.3.
- **Event Handling**: Manage button presses and other events using a custom event queue.
- **USB HID Support**: Communicate with a host device via USB HID.


### Related Resources
- [Raspberry Pi Pico Documentation](https://rptl.io/picow-connect)
- [Waveshare Pico-LCD-1.3 Documentation](https://www.waveshare.com/wiki/Pico-LCD-1.3)
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [Raspberry Pi Pico Examples](https://github.com/raspberrypi/pico-examples)


## Getting Started

### Prerequisites
- Raspberry Pi Pico
- Waveshare Pico-LCD-1.3

### Setup

1. Clone this repository:
   ```sh
   $ git clone --recurse-submodules https://github.com/pirakansa/pico_LCD_IDE.git
   $ cd pico_LCD_IDE
   ```

   > Note: If you forgot to use the `--recurse-submodules` option when cloning, you can manually initialize and update the submodules with the following command:
   > 
   > ```sh
   > $ git submodule update --init --recursive
   > ```

2. Set up the development environment using the provided `.devcontainer` configuration (if applicable).

3. Build the project.


## Build Instructions

Follow these steps to build the project and generate a `.uf2` file for flashing onto the Raspberry Pi Pico:

```sh
$ mkdir -p build
$ cmake -DPICO_BOARD=pico -S . -B ./build
$ make -C ./build
```

The resulting `.uf2` file will be located in the `build` directory.


## Flashing the Pico

1. Connect your Raspberry Pi Pico to your computer while holding the BOOTSEL button.
2. Copy the generated `.uf2` file to the Pico's storage.

