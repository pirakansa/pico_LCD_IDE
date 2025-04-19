
add_subdirectory(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/Config     ${CMAKE_CURRENT_BINARY_DIR}/Pico_code/c/lib/Config)
add_subdirectory(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/Infrared   ${CMAKE_CURRENT_BINARY_DIR}/Pico_code/c/lib/Infrared)
add_subdirectory(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/LCD        ${CMAKE_CURRENT_BINARY_DIR}/Pico_code/c/lib/LCD)
add_subdirectory(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/Fonts      ${CMAKE_CURRENT_BINARY_DIR}/Pico_code/c/lib/Fonts)
add_subdirectory(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/GUI        ${CMAKE_CURRENT_BINARY_DIR}/Pico_code/c/lib/GUI)

include_directories(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/Config)
include_directories(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/GUI)
include_directories(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/LCD)
include_directories(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/Infrared)
include_directories(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/Fonts)

aux_source_directory(. DIR_LCD_SRCS)

add_library(liblcd
    ${DIR_LCD_SRCS}
)

target_link_libraries(liblcd PUBLIC
    pico_stdlib
    pico_multicore
    Config
    LCD
    Infrared
    GUI
    Fonts
    Config
)