#include "../noftypes.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* input */
extern void controller_init();
extern uint32 controller_read_input();
extern void controller_stop();

/* display */
extern void display_init();
extern void display_write_palette(const uint32 palette[]);
extern void display_write_frame(const uint8 *data[]);
extern void display_clear();
extern void display_stop();

/* sound */
extern void sound_init();
extern void sound_write_frame(const int16_t samples[], int n);
extern void sound_stop();

/* time */
extern void time_init();
extern void time_write_frequency(int hertz);
extern int time_read_ticks(bool blocking);
extern void time_stop();

#ifdef __cplusplus
}
#endif /* __cplusplus */