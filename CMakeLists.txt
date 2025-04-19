cmake_minimum_required(VERSION 3.13...3.27)

set(NAME picopro)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

# Set default board to pico if not specified
if(NOT DEFINED PICO_BOARD)
    set(PICO_BOARD pico)
endif()

if(PICO_BOARD STREQUAL "pico_w")
    set(PICO_CYW43_SUPPORTED 1)
    add_compile_definitions(PICO_CYW43_SUPPORTED=1)
endif()

project(${NAME} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
#set(FAMILY rp2040)
pico_sdk_init()

# add a compilation subdirectory
add_subdirectory(src/pico)
add_subdirectory(src/lcd)
add_subdirectory(src/usb)


# generate an executable file
add_executable(${NAME} 
    src/main.c
)

# enable usb output, disable uart output
#pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_usb(${NAME} 0)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${NAME})

target_include_directories(${NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${NAME}
    libpico
    liblcd
    libusb
    pico_stdlib
)
