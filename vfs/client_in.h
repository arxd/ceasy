

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>

IOMem *io;
//~ Voice *voices;
//~ Object *objects;
uint8_t *vram;
//~ Color *palette;
//~ Sprite *sprites;
//~ Mapel *maps;
Layer *layers;

void iomem_init(void *mem)
{
	io = (IOMem*)mem;
	//~ voices = io->voices;
	//~ objects = io->objects;
	vram = io->vram;
	//~ palette = io->palette;
	//~ sprites = io->sprites;
	//~ maps = io->maps;
	layers = io->layers;
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
	//~ IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	//~ ipc_init(ipc+1, ipc);
}

void set_palette(uint16_t idx, uint32_t rgba)
{
	vram[idx] = (rgba>>24)&0xff;
	vram[idx+1] = (rgba>>16)&0xff;
	vram[idx+2] = (rgba>>8)&0xff;
	vram[idx+3] = (rgba>>0)&0xff;
}


