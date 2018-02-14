#ifndef CLIENT_C
#define CLIENT_C

#ifndef IOMEM_H
#include "iomem.h"
#endif

typedef void FrameSyncHandler(int);

extern IOMem *io;
extern uint8_t *vram;
extern Layer *layers;
extern volatile Input *input;

void pixie_init(const char *shmid_str);
void printf_xy(int x, int y, const char *fmt, ...);
void on_frame_sync(FrameSyncHandler *handler);
double now(void);
void copy32(uint8_t *dest, uint32_t *src, int words);
void copy16(uint8_t *dest, uint16_t *src, int words);
void copy8(uint8_t *dest, uint8_t *src, int words);

#if __INCLUDE_LEVEL__ == 0 || defined(PIXIE_NOLIB)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifndef FONT4x6x1_C
#include "font4x6x1.c"
#endif

#ifndef LOGGING_C
#include "logging.c"
#endif

IOMem *io;
uint8_t *vram;
Layer *layers;
volatile Input *input;

void copy8(uint8_t *dest, uint8_t *src, int words)
{
	memcpy(dest, src, words);
	//~ while(words--)
		//~ *dest++ = *src++;
}

void copy16(uint8_t *dest, uint16_t *src, int words)
{
	while (words--) {
		*(dest++) = ((*src)>>8)&0xFF;
		*(dest++) = ((*src)>>0)&0xFF;
		++src;
	}
}

void copy32(uint8_t *dest, uint32_t *src, int words)
{
	while (words--) {
		*(dest++) = ((*src)>>24)&0xFF;
		*(dest++) = ((*src)>>16)&0xFF;
		*(dest++) = ((*src)>>8)&0xFF;
		*(dest++) = ((*src)>>0)&0xFF;
		++src;
	}
}



FrameSyncHandler *g_fs_handler;

void sig_handler(int sig) {
	static int frame = 0;
	if (g_fs_handler)
		g_fs_handler(frame++);
}

void on_frame_sync(FrameSyncHandler *handler)
{
	g_fs_handler = handler;
}

#ifdef WIN32
void setup_shm(const char *shmid_str)
{
	printf("SETUP Windows shared mem\n");
	ABORT(1, "Windows Sucks");
}

double now(void)
{
	return 0.0;
}


#else

#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void setup_shm(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr, 16);
	ASSERT (shmid_str != endptr, "ROM Must be run as an argument to the server");
	io = (IOMem*)shmat(shmid, NULL, 0);
	ASSERT((void*)io != (void*)-1, "Couldn't attache shared memory");
	signal(SIGUSR1, sig_handler);	
}

double now(void)
{
	static struct timespec t0 = {0xFFFFFFFF, 0xFFFFFFFF};
	if (t0.tv_nsec == 0xFFFFFFFF) 
		clock_gettime(CLOCK_REALTIME, &t0);
	struct timespec t1;
	clock_gettime(CLOCK_REALTIME, &t1);
	return (t1.tv_sec - t0.tv_sec) + 1.0e-9*( t1.tv_nsec - t0.tv_nsec);
}


#endif


void pixie_init(const char *shmid_str)
{
	setup_shm(shmid_str);
	vram = io->vram;
	layers = io->layers;
	input = &io->input;
	
	
	uint32_t pal[256] = {
		0xdddddd00,
		0x00000080,
		0xff5555ff,
		0x55ff55ff,
		0x5555ffff,
		0xffff00ff,
		0x444444ff};
	for (int p =7; p < 256; ++p)
		pal[p] = 0xFF00FFFF;
	
	copy32(vram + 0x9000, pal, 256);
	
	// Layer 0 is a direct pixel map of the whole screen
	layers[0] = (Layer) {
		.palette = 0x9000,
		.map.addr = 0x0000,
		.map.w = 256, .map.h = 144,
		.flags = LAYER_ON,
		.tiles.w = 1, .tiles.h = 1,
	};
	
	//Layer 1 is a map of ACII tiles
	copy8(vram + 0x9400, FONT4x6x1, 128*6);
	layers[1] = (Layer) {
		.palette = 0x9000,
		.map.addr = 0x9700,
		.map.aux = 0xA300,
		.tiles.addr = 0x9400,
		.map.w = 64, .map.h = 24,
		.tiles.w = 4, .tiles.h = 6, .tiles.bits = 1,
		.tiles.row_bytes = 1, .tiles.num_tiles = 128-1,
		.flags = LAYER_ON | LAYER_AUX,
	};

}

void printf_xy(int x, int y, const char *fmt, ...)
{
	char buffer[64*24+1];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 64*24+1, fmt, args);	
	va_end(args);
	
	int addr=y*64+x;
	char *chr = buffer;
	while (*chr) {
		vram[(addr%(64*24)) + layers[1].map.addr] = (*chr)%128;
		++addr;
		++chr;
	}
}

#endif
#endif

