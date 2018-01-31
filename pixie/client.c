#ifndef CLIENT_C
#define CLIENT_C

#if __INCLUDE_LEVEL__ == 0

#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "pixie.h"
#include "font4x6x1.h"
#include "util.c"

IOMem *io;
uint8_t *vram;
Layer *layers;
volatile Input *input;

void map_init(Map *self, int addr, int w, int h, uint8_t *data)
{
	self->addr = (uint16_t)addr;
	self->aux = self->addr;
	self->w = (uint16_t)w;
	self->h = (uint16_t)h;
	self->x = 0;
	self->y = 0;
	for (int i=0; i < self->w * self->h; ++i)
		vram[self->addr + i] = data? data[i] : 0;
}

void palette_init(int addr, int num, uint32_t *data)
{
	for (int i=0; i < num; ++i) {
		vram[addr + i*4 + 0] = data? ((data[i]>>24)&0xFF) : 0;
		vram[addr + i*4 + 1] = data? ((data[i]>>16)&0xFF) : 0;
		vram[addr + i*4 + 2] = data? ((data[i]>>8)&0xFF) : 0;
		vram[addr + i*4 + 3] = data? (data[i]&0xFF) : 0;
	}
}

void tiles_load8(int addr, int words, uint8_t *data)
{
	for (int i = 0; i < words; ++i )
		vram[addr + i] = data? data[i] : 0;
}

void tiles_load16(int addr, int words, uint16_t *data)
{
	for (int i = 0; i < words; ++i ) {
		vram[addr + i*2 + 0] = data? ((data[i]>>8)&0xFF) : 0;
		vram[addr + i*2 + 1] = data? (data[i]&0xFF) : 0;
	}
}

void tiles_load32(int addr, int words, uint32_t *data)
{
	for (int i = 0; i < words; ++i ) {
		vram[addr + i*4 + 0] = data? ((data[i]>>24)&0xFF) : 0;
		vram[addr + i*4 + 1] = data? ((data[i]>>16)&0xFF) : 0;
		vram[addr + i*4 + 2] = data? ((data[i]>>8)&0xFF) : 0;
		vram[addr + i*4 + 3] = data? (data[i]&0xFF) : 0;
	}
}

void tiles_init(Tiles *self, int addr, int w, int h, int b)
{
	self->addr = (uint16_t)addr;
	self->w = (uint16_t)w;
	self->h = (uint16_t)h;
	self->bits = (b == 2 || b == 4)? b : 1;
	self->num_tiles = 0;
	int rowbits = self->w * self->bits;
	self->row_bytes = (rowbits/8) + !!(rowbits%8);
}

void tiles_init_load8(Tiles *self, int addr, int w, int h, int b, int num, uint8_t *data)
{
	tiles_init(self, addr, w, h, b);
	self->num_tiles = num-1;
	tiles_load8(self->addr, self->row_bytes * self->h * (self->num_tiles+1), data);
}

void tiles_init_load16(Tiles *self, int addr, int w, int h, int b, int num, uint16_t *data)
{
	tiles_init(self, addr, w, h, b);
	self->num_tiles = num-1;
	tiles_load16(self->addr, (self->row_bytes/2) * self->h * (self->num_tiles+1), data);
}

void tiles_init_load32(Tiles *self, int addr, int w, int h, int b, int num, uint32_t *data)
{
	tiles_init(self, addr, w, h, b);
	self->num_tiles = num-1;
	tiles_load32(self->addr, (self->row_bytes/4) * self->h * (self->num_tiles+1), data);
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

void frame_sync_interrupt(int frame)
{
}

void sig_handler(int sig) {
	static int frame = 0;
	frame_sync_interrupt(frame++);
}

void io_init(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr, 16);
	if (shmid_str == endptr)
		ABORT(1, "ROM Must be run as an argument to the server");
	io = (IOMem*)shmat(shmid, NULL, 0);
	if ((void*)io == (void*)-1)
		ABORT(2, "Couldn't attache shared memory");

	vram = io->vram;
	layers = io->layers;
	input = &io->input;
	
	signal(SIGUSR1, sig_handler);
	
	palette_init(0x9000, 6, (uint32_t[]) {
		0xdddddd00,
		0x00000080,
		0xff5555ff,
		0x55ff55ff,
		0x5555ffff,
		0xffff00ff,
		0x444444ff});

	
	// Layer 0
	map_init(&layers[0].map, 0x0000, 256, 144, 0);
	layers[0].palette = 0x9000;
	tiles_init(&layers[0].tiles, 0x0000, 1, 1, 1);
	layers[0].flags = LAYER_ON;
	
	//Layer 1
	map_init(&layers[1].map, 0x9700, 64, 24, 0);
	layers[1].map.aux = 0xf300;
	layers[1].palette = 0x9000;
	tiles_init_load8(&layers[1].tiles, 0x9400, 4, 6, 1, 128, FONT4x6x1);
	layers[1].flags = LAYER_ON | LAYER_AUX;

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

void printf_window(int ulX, int ulY, int brX, int brY, const char *fmt, ...)
{
	char buffer[64*24+1];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 64*24+1, fmt, args);	
	va_end(args);
}

#endif
#endif

