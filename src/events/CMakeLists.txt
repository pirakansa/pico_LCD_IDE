
add_library(libevent STATIC
    events.c
)

target_link_libraries(libevent
    pico_stdlib
)


