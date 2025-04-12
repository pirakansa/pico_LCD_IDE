# pico_LCD_IDE

This repository is a project utilizing the Raspberry Pi Pico and the Pico-LCD-1.3.

- https://rptl.io/picow-connect
- https://www.waveshare.com/wiki/Pico-LCD-1.3
- https://github.com/raspberrypi/pico-sdk
- https://github.com/raspberrypi/pico-examples

## build to uf2

After setting up the environment in .devcontainer, follow these steps:

```sh
$ mkdir -p build
$ cmake -DPICO_BOARD=pico  -S . -B ./build
$ make -C ./build
```

