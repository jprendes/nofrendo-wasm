add_library(nofrendo-core
    bitmap.c
    config.c
    event.c
    gui.c
    gui_elem.c
    intro.c
    log.c
    memguard.c
    nofrendo.c
    pcx.c
    vid_drv.c
)

add_executable(nofrendo
    main.cpp
)

target_link_libraries(nofrendo
    nofrendo-core
    nofrendo-cpu
    nofrendo-snss
    nofrendo-mappers
    nofrendo-nes
    nofrendo-sndhrdw
)
