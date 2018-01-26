#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "util.c"
#include "share.c"

unsigned char *vram;
SharedMem g_shm;

void cpu_init(const char *shmid_str)
{
	char * endptr;
	g_shm.shmid = (int)strtol(shmid_str, &endptr, 16);
	if (shmid_str == endptr)
		ABORT(10, "ROM Must be run as an argument to the server");
	
	if (!shm_init(&g_shm, 0))
		ABORT(11, "Failed to init shared mem");

	on_exit(shm_on_exit, &g_shm);
	vram = g_shm.mem;
	//~ IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	//~ ipc_init(ipc+1, ipc);
}

int main(int argc, char *argv[])
{
	cpu_init(argv[0]);

	unsigned char x = 6;
	while (--x) {
		vram[x] = x;
		sleep(1);
	}
	return 1;
}