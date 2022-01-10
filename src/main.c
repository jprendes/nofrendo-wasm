#include "nofrendo.h"

#include "osd/external.h"

/* input */
void controller_init() {}
uint32 controller_read_input() { return 0; }
void controller_stop() {}

/* display */
void display_init() {}
void display_write_palette(const uint32 palette[]) {}
void display_write_frame(const uint8 *data[]) {}
void display_clear() {}
void display_stop() {}

/* sound */
void sound_init() {}
void sound_write_frame(const int16_t samples[], int n) {}
void sound_stop() {}

int main(int argc, char **argv) {
    return nofrendo_main(1, argv + 1);
}