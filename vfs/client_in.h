

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>

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
	//~ IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	//~ ipc_init(ipc+1, ipc);
}

void set_palette(uint8_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	palette[i].r = r;
	palette[i].g = g;
	palette[i].b = b;
	palette[i].a = a;
	
}
