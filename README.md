# pico_LCD_IDE

- https://rptl.io/picow-connect
- https://www.waveshare.com/wiki/Pico-LCD-1.3
- https://github.com/raspberrypi/pico-sdk
- https://github.com/raspberrypi/pico-examples

## build to uf2

After setting up the environment in .devcontainer, follow these steps:

```sh
$ git clone https://github.com/waveshare/Pico_code.git lcd
$ patch -p1 --directory=lcd < fix_typo.patch
$ git clone https://github.com/raspberrypi/pico-examples.git examples
$ mkdir -p build && cmake -DPICO_BOARD=pico_w -S . -B ./build
$ make -C ./build
```

