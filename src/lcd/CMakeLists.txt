aux_source_directory(. DIR_LCD_SRCS)


add_subdirectory(${PROJECT_SOURCE_DIR}/Pico_code/c/lib/Config)
# add_subdirectory(Pico_code/c/lib/Infrared)
# add_subdirectory(Pico_code/c/lib/LCD)
# add_subdirectory(Pico_code/c/lib/Fonts)
# add_subdirectory(Pico_code/c/lib/GUI)

include_directories(../../Pico_code/c/lib/Config)
include_directories(../../Pico_code/c/lib/GUI)
include_directories(../../Pico_code/c/lib/LCD)
include_directories(../../Pico_code/c/lib/Infrared)
include_directories(../../Pico_code/c/lib/Fonts)

add_library(locallcd
    ${DIR_LCD_SRCS}
)

target_link_libraries(locallcd
    PUBLIC
    pico_stdlib
    Config
    LCD
    Infrared
    GUI
    Fonts
    Config
)