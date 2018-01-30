

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>

IOMem *io;
volatile uint8_t *vram;
Layer *layers;

void iomem_init(void *mem)
{
	io = (IOMem*)mem;
	vram = io->vram;
	layers = io->layers;
}


void set_palette(uint16_t idx, uint32_t rgba)
{
	vram[idx] = (rgba>>24)&0xff;
	vram[idx+1] = (rgba>>16)&0xff;
	vram[idx+2] = (rgba>>8)&0xff;
	vram[idx+3] = (rgba>>0)&0xff;
}

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

typedef void FrameCallback(int);

int g_frame = 0;
FrameCallback *g_on_frame = 0;

void sig_handler(int sig) {
	if (g_on_frame)
		g_on_frame(g_frame++);
}

void int_framesync(FrameCallback *callback)
{
	g_on_frame = callback;
}

void cpu_init(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr, 16);
	if (shmid_str == endptr) {
		printf("ROM Must be run as an argument to the server");
		exit(1);
	}
	io = (IOMem*)shmat(shmid, NULL, 0);
	if ((void*)io == (void*)-1) {
		perror("Couldn't attache shared memory");
		exit(2);
	}

	iomem_init(io);
	
	signal(SIGUSR1, sig_handler);
	
	// Load up some defaults
	map_init(&layers[0].map, 0x0000, 256, 144, 0);
	layers[0].palette = 0x9000;
	tiles_init(&layers[0].tiles, 0x0000, 1, 1, 1);
	layers[0].flags = LAYER_ON;
	
	//0x0000 map
	map_init(&layers[1].map, 0x9700, 64, 24, 0);
	//0x9000 palette
	layers[1].palette = 0x9000;
	
	//~ float factors[] = {
		//~ 1.0, 1.0, 1.0,
		
	//~ for (i=0; i < 256; ++i) {
		
		
		//~ vram[0x9000 + i*4 + 0] = 
		
	//~ }
	
	
	palette_init(0x9000, 6, (uint32_t[]) {
		0xdddddd00,
		0x000000ff,
		0xff5555ff,
		0x55ff55ff,
		0x5555ffff,
		0xffff00ff,
		0x444444ff});
	//0x9400 tiles
	tiles_init_load8(&layers[1].tiles, 0x9400, 4, 6, 1, 128, FONT4x6x1);
	layers[1].flags = LAYER_ON;
	
	//~ memset(&layers[0], 0, sizeof(Layer));

	
	//~ IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	//~ ipc_init(ipc+1, ipc);
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


