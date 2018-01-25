#include <stdint.h>
#include "share.c"

//~ #define SCREEN_W 256
//~ #define SCREEN_H 144

// Global shared memory
struct {
	uint8_t *vram;
} io;

void cpu_init(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr, 16);
	if (shmid_str == endptr)
		death("Error", "Must run via ./server", -1);
	uint8_t *mem = shalloc(&shmid, 0);
	io.vram = mem;
	//~ io.palette = io.screen + SCREEN_W*SCREEN_H;
	IPC *ipc = (IPC*)(mem + 256*256);
	ipc_init(ipc+1, ipc);
}

