#include <stdint.h>
#include "share.c"

#define SCREEN_W 256
#define SCREEN_H 144

// Global shared memory
struct {
	uint8_t *palette;
	uint8_t *screen;
} io;

void cpu_init(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr,16);
	uint8_t *mem = shalloc(&shmid, 0);
	io.screen = mem;
	io.palette = io.screen + SCREEN_W*SCREEN_H;
	IPC *ipc = (IPC*)(io.palette + 256*4);
	ipc_init(ipc+1, ipc);
}

