add_library(nofrendo-nes
    mmclist.c
    nes.c
    nes_mmc.c
    nes_pal.c
    nes_ppu.c
    nes_rom.c
    nesinput.c
    nesstate.c
)

target_link_libraries(nofrendo-nes nofrendo-cpu nofrendo-mappers nofrendo-snss nofrendo-core)