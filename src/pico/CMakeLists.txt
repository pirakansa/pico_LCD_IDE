
add_library(libpico STATIC
    pico.c
)

target_link_libraries(libpico
    pico_stdlib
)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(libpico
        pico_cyw43_arch_none
    )
endif()

