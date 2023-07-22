# pico_LCD_IDE

- https://github.com/raspberrypi/pico-sdk
- https://github.com/raspberrypi/pico-examples
- https://www.waveshare.com/wiki/Pico-LCD-1.3

## build to uf2

After setting up the environment in .devcontainer, follow these steps:

```sh
$ git clone https://github.com/waveshare/Pico_code.git lcd
$ patch -p1 --directory=lcd < fix_typo.patch
$ mkdir -p build && cmake -S . -B ./build
$ make -C ./build
```

