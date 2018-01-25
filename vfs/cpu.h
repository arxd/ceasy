#include <stdint.h>
#include "share.c"

//~ #define SCREEN_W 256
//~ #define SCREEN_H 144

// Global shared memory
struct {
	uint8_t *vram;
} io;


SharedMem g_shm;

int cpu_init(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr, 16);
	if (shmid_str == endptr)
		return 0;
	
	if (!shm_init(&g_shm, 0))
		return 0;

	on_exit(shm_fini, &g_shm);
	io.vram = g_shm.mem;
	IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	ipc_init(ipc+1, ipc);
}

