/* start rewrite from: https://github.com/espressif/esp32-nesemu.git */
#include <string.h>
#include <stdlib.h>

#include "../noftypes.h"
#include "../event.h"
#include "../gui.h"
#include "../log.h"
#include "../nes/nes.h"
#include "../nes/nes_pal.h"
#include "../nes/nesinput.h"
#include "../nofconfig.h"
#include "../osd.h"

#include "external.h"

/* memory allocation */
extern void *mem_alloc(int size, bool prefer_fast_memory) {
	UNUSED(prefer_fast_memory);
	return malloc(size);
}

/* time keeping */
void osd_ticks_frequency(int hertz) {
	time_write_frequency(hertz);
}

int osd_ticks(bool blocking) {
	return time_read_ticks(blocking);
}

/* audio */
#define DEFAULT_SAMPLERATE 22050
#define DEFAULT_FRAGSIZE 1024

static void (*audio_callback)(void *buffer, int length) = NULL;
static int16_t audio_frame[DEFAULT_FRAGSIZE];

static void do_audio_frame() {
	int left = DEFAULT_SAMPLERATE / NES_REFRESH_RATE;
	while (left)
	{
		int n = DEFAULT_FRAGSIZE;
		if (n > left)
			n = left;
		audio_callback(audio_frame, n); //get more data
		sound_write_frame(audio_frame, n);
		left -= n;
	}
}

void osd_setsound(void (*playfunc)(void *buffer, int length)) {
	//Indicates we should call playfunc() to get more data.
	audio_callback = playfunc;
}

/* video */
/* get info */
static char fb[1]; //dummy
bitmap_t *myBitmap;

/* initialise video */
static int init(int width, int height)
{
	return 0;
}

static void shutdown(void)
{
}

/* set a video mode */
static int set_mode(int width, int height)
{
	return 0;
}

/* copy nes palette over to hardware */
uint32 palette[256];
static void set_palette(rgb_t *pal)
{
	for (int i = 0; i < 256; i++)
	{
		palette[i] = ((pal[i].r & 0xFF) << 0) | ((pal[i].g & 0xFF) << 8) | ((pal[i].b & 0xFF) << 16);
	}

	display_write_palette(palette);
}

/* clear all frames to a particular color */
static void clear(uint8 color)
{
	// SDL_FillRect(mySurface, 0, color);
	display_clear();
}

/* acquire the directbuffer for writing */
static bitmap_t *lock_write(void)
{
	// SDL_LockSurface(mySurface);
	myBitmap = bmp_createhw((uint8 *)fb, NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, NES_SCREEN_WIDTH * 2);
	return myBitmap;
}

/* release the resource */
static void free_write(int num_dirties, rect_t *dirty_rects)
{
	bmp_destroy(&myBitmap);
}

static void custom_blit(bitmap_t *bmp, int num_dirties, rect_t *dirty_rects)
{
	display_write_frame((const uint8_t **)bmp->line);
	do_audio_frame();
}

viddriver_t sdlDriver =
	{
		"Simple DirectMedia Layer", /* name */
		init,						/* init */
		shutdown,					/* shutdown */
		set_mode,					/* set_mode */
		set_palette,				/* set_palette */
		clear,						/* clear */
		lock_write,					/* lock_write */
		free_write,					/* free_write */
		custom_blit,				/* custom_blit */
		false						/* invalidate flag */
};

void osd_getvideoinfo(vidinfo_t *info)
{
	info->default_width = NES_SCREEN_WIDTH;
	info->default_height = NES_SCREEN_HEIGHT;
	info->driver = &sdlDriver;
}

void osd_getsoundinfo(sndinfo_t *info)
{
	info->sample_rate = DEFAULT_SAMPLERATE;
	info->bps = 16;
}

/* input */
void osd_getinput(void)
{
	const int ev[32] = {
		event_joypad1_up, event_joypad1_down, event_joypad1_left, event_joypad1_right,
		event_joypad1_select, event_joypad1_start, event_joypad1_a, event_joypad1_b,

		event_joypad2_up, event_joypad2_down, event_joypad2_left, event_joypad2_right,
		event_joypad2_select, event_joypad2_start, event_joypad2_a, event_joypad2_b,

		event_state_save, event_state_load, 0, 0,
		0, 0, 0, 0,
		
		0, 0, 0, 0,
		0, 0, 0, 0};
	static int oldb = 0xffff;
	uint32_t b = controller_read_input();
	uint32_t chg = b ^ oldb;
	int x;
	oldb = b;
	event_t evh;
	// nofrendo_log_printf("Input: %x\n", b);
	for (x = 0; x < 16; x++)
	{
		if (chg & 1)
		{
			evh = event_get(ev[x]);
			if (evh)
				evh((b & 1) ? INP_STATE_BREAK : INP_STATE_MAKE);
		}
		chg >>= 1;
		b >>= 1;
	}
}

void osd_getmouse(int *x, int *y, int *button)
{
}

/* init / shutdown */
static int logprint(const char *string)
{
	return printf("%s", string);
}

int osd_init()
{
	nofrendo_log_chain_logfunc(logprint);

	display_init();
	controller_init();
	sound_init();
	time_init();

	return 0;
}

void osd_shutdown()
{
	time_stop();
	sound_stop();
	controller_stop();
	display_stop();
}

char configfilename[] = "na";
int osd_main(int argc, char *argv[])
{
	config.filename = configfilename;
	return main_loop(argv[0], system_autodetect);
}

//Seemingly, this will be called only once. Should call func with a freq of frequency,
int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
{
	nofrendo_log_printf("Timer install, freq=%d\n", frequency);
	return 0;
}

/* filename manipulation */
void osd_fullname(char *fullname, const char *shortname)
{
	strncpy(fullname, shortname, PATH_MAX);
}

/* This gives filenames for storage of saves */
char *osd_newextension(char *string, char *ext)
{
	// dirty: assume both extensions is 3 characters
	size_t l = strlen(string);
	string[l - 3] = ext[1];
	string[l - 2] = ext[2];
	string[l - 1] = ext[3];

	return string;
}

/* This gives filenames for storage of PCX snapshots */
int osd_makesnapname(char *filename, int len)
{
	return -1;
}