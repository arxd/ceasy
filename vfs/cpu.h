#include <stdint.h>
#include "share.c"

#define SCREEN_W 256
#define SCREEN_H 144

typedef struct s_SharedMem SharedMem;

struct s_SharedMem {
	uint8_t palette[256][4];
	uint8_t screen[SCREEN_W*SCREEN_H];
};

// Global shared memory
SharedMem *io = 0;
//~ int shmid = 0;

void cpu_init(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr,16);
	io = shalloc(&shmid, 0);
	printf("Connected to CPU: %p", io);
}

