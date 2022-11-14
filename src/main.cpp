#include "nofrendo.h"

#include "osd/external.h"
#include "nes/nes.h"

#include <chrono>
#include <thread>

#include <emscripten.h>

/* input */
EM_JS(void, em_controller_init, (), {
    Module.controller_init && Module.controller_init();
});

EM_JS(uint32, em_controller_read_input, (), {
    return Module.controller_read_input ? Module.controller_read_input() : 0;
});

EM_JS(void, em_controller_stop, (), {
    Module.controller_stop && Module.controller_stop();
});

void controller_init() { em_controller_init(); }
uint32 controller_read_input() { return em_controller_read_input(); }
void controller_stop() { em_controller_stop(); }

/* display */
namespace {

uint32 nes_palette[256];
uint32 nes_screen[NES_SCREEN_HEIGHT * NES_SCREEN_WIDTH];

}

EM_JS(void, em_display_init, (), {
    Module.display_init && Module.display_init();
});

EM_JS(void, em_display_write_frame, (const uint32 * data), {
    Module.display_write_frame && Module.display_write_frame(new Uint8ClampedArray(Module.HEAP8.buffer, data, 240 * 256 * 4));
});

EM_JS(void, em_display_clear, (), {
    Module.display_clear && Module.display_clear();
});

EM_JS(void, em_display_stop, (), {
    Module.display_stop && Module.display_stop();
});

void display_init() { em_display_init(); }
void display_write_palette(const uint32 p[]) {
    for (int i = 0; i < 256; ++i)
        nes_palette[i] = p[i] | 0xFF000000;
}
void display_write_frame(const uint8 *data[]) {
    for (int l = 0; l < NES_SCREEN_HEIGHT; ++l) {
        for (int c = 0; c < NES_SCREEN_WIDTH; ++c) {
            nes_screen[l * NES_SCREEN_WIDTH + c] = nes_palette[data[l][c]];
        }
    }
    em_display_write_frame(nes_screen);
}
void display_clear() { em_display_clear(); }
void display_stop() { em_display_stop(); }

/* sound */
EM_JS(void, em_sound_init, (), {
    Module.sound_init && Module.sound_init();
});

EM_JS(void, em_sound_write_frame, (const int16_t * samples, int n), {
    Module.sound_write_frame && Module.sound_write_frame(Module.HEAP16.slice(samples >> 1, (samples >> 1) + n));
});

EM_JS(void, em_sound_stop, (), {
    Module.sound_stop && Module.sound_stop();
});

void sound_init() { em_sound_init(); }
void sound_write_frame(const int16_t samples[], int n) { em_sound_write_frame(samples, n); }
void sound_stop() { em_sound_stop(); }

/* time */
namespace {

auto next_tick = std::chrono::steady_clock::now();
auto frame_time = std::chrono::milliseconds(1000) / 60;

}

void time_init() {
    next_tick = std::chrono::steady_clock::now();
}
void time_stop() {}

void time_write_frequency(int hertz) {
    frame_time = std::chrono::milliseconds(1000) / hertz;
}

int time_read_ticks(bool blocking) {
    auto time_diff = std::chrono::steady_clock::now() - next_tick;
    if (time_diff < std::chrono::milliseconds::zero()) {
        if (!blocking) return 0;
        // std::this_thread::sleep_until(next_tick);
        emscripten_sleep(std::chrono::duration_cast<std::chrono::milliseconds>(-time_diff).count());
        return time_read_ticks(false);
    }
    int ticks = 1 + time_diff / frame_time;
    next_tick += frame_time * ticks;
    return ticks;
}

EMSCRIPTEN_KEEPALIVE
extern "C"
int run_nofrendo(char *filename) {
    return nofrendo_main(1, &filename);
}