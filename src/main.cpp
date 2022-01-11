#include "nofrendo.h"

#include "osd/external.h"

#include <chrono>
#include <thread>

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
        std::this_thread::sleep_until(next_tick);
        return time_read_ticks(false);
    }
    int ticks = 1 + time_diff / frame_time;
    next_tick += frame_time * ticks;
    return ticks;
}

int main(int argc, char **argv) {
    return nofrendo_main(1, argv + 1);
}