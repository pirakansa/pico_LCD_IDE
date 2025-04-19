
aux_source_directory(. DIR_USB_SRCS)

add_library(libusb
    ${DIR_USB_SRCS}
)

target_include_directories(libusb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/pico-examples/usb/device
)

target_link_libraries(libusb PUBLIC
    pico_stdlib
    tinyusb_device
    tinyusb_board
)