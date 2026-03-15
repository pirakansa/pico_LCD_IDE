
aux_source_directory(. DIR_USB_SRCS)

add_library(libusb
    ${DIR_USB_SRCS}
)

target_include_directories(libusb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/pico-examples/usb/device
)

if(DEFINED USB_BTN1_TEXT)
    target_compile_definitions(libusb PUBLIC USB_BTN1_TEXT="${USB_BTN1_TEXT}")
endif()

if(DEFINED USB_BTN2_TEXT)
    target_compile_definitions(libusb PUBLIC USB_BTN2_TEXT="${USB_BTN2_TEXT}")
endif()

if(DEFINED USB_BTN3_TEXT)
    target_compile_definitions(libusb PUBLIC USB_BTN3_TEXT="${USB_BTN3_TEXT}")
endif()

if(DEFINED USB_BTN4_TEXT)
    target_compile_definitions(libusb PUBLIC USB_BTN4_TEXT="${USB_BTN4_TEXT}")
endif()

target_link_libraries(libusb PUBLIC
    pico_stdlib
    tinyusb_device
    tinyusb_board
)
