cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(picopro C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

# add a compilation subdirectory
add_subdirectory(lcd/c/lib/Config)
add_subdirectory(lcd/c/lib/Infrared)
add_subdirectory(lcd/c/lib/LCD)
add_subdirectory(lcd/c/lib/Fonts)
add_subdirectory(lcd/c/lib/GUI)

# add a header directory
include_directories(./lcd/c/lib/Config)
include_directories(./lcd/c/lib/GUI)
include_directories(./lcd/c/lib/LCD)
include_directories(./lcd/c/lib/Infrared)
include_directories(./lcd/c/lib/Fonts)

add_library(ImageData ./ImageData.c)

# generate an executable file
add_executable(picopro main.c)

# enable usb output, disable uart output
pico_enable_stdio_usb(picopro 1)
pico_enable_stdio_uart(picopro 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(picopro)

target_link_libraries(
    picopro
    LCD
    Infrared
    GUI
    Fonts
    Config
    pico_stdlib
    hardware_spi
    ImageData
)