cmake_minimum_required(VERSION 3.13...3.27)

set(NAME picopro)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)


project(${NAME} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

# add a compilation subdirectory
add_subdirectory(Pico_code/c/lib/Config)
add_subdirectory(Pico_code/c/lib/Infrared)
add_subdirectory(Pico_code/c/lib/LCD)
add_subdirectory(Pico_code/c/lib/Fonts)
add_subdirectory(Pico_code/c/lib/GUI)

# add a header directory
include_directories(./Pico_code/c/lib/Config)
include_directories(./Pico_code/c/lib/GUI)
include_directories(./Pico_code/c/lib/LCD)
include_directories(./Pico_code/c/lib/Infrared)
include_directories(./Pico_code/c/lib/Fonts)
include_directories(./pico-examples/pico_w/wifi/access_point)
include_directories(./pico-examples/pico_w/wifi)
include_directories(./pico-examples/pico_w/wifi/access_point/dhcpserver)
include_directories(./pico-examples/pico_w/wifi/access_point/dnsserver)

# generate an executable file
add_executable(${NAME} 
    main.c
    pico-examples/pico_w/wifi/access_point/dhcpserver/dhcpserver.c
    pico-examples/pico_w/wifi/access_point/dnsserver/dnsserver.c
    )

# enable usb output, disable uart output
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${NAME})

target_link_libraries(
    ${NAME}
    LCD
    Infrared
    GUI
    Fonts
    Config
    pico_stdlib
    hardware_spi
    pico_cyw43_arch_lwip_poll
)