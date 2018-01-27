#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "util.c"
#include "share.c"
#include "iomem.h"
#include <math.h>

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

	iomem_init(g_shm.mem);
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

int main(int argc, char *argv[])
{
	printf("Voice %lu (%lu)\n", sizeof(Voice), sizeof(Voice)*32);
	printf("Input %lu\n", sizeof(Input));
	printf("VRAM %d\n", 256*144);
	printf("Mapel %lu\n", sizeof(Mapel));
	printf("Object %lu\n", sizeof(Object));
	printf("IOMEM %lu (%d)\n", sizeof(IOMem), 256*256);
	//~ printf("toal %lu / %d\n", 256*256, 256*144+256*4+)
	cpu_init(argv[0]);

	int i, frame=0;
	
	
	//~ int r, c;
	//~ for (r=0; r < 144; ++r) {
		//~ for (c=0; c < 256; ++c) {
			//~ if (r < 25) {
				//~ if (c < 66) {
					//~ vram[r*256+c] = 0;
				//~ } else {
					//~ vram[r*256+c] = 1;
				//~ }
			//~ } else {
				//~ if (c < 120) {
					//~ vram[r*256+c] = 2;
				//~ } else {
					//~ vram[r*256+c] = 3;
				//~ }
			//~ }
		//~ }
	//~ }
	int r, c, z;
	//~ for (i=0;i < 144*256; ++i)
		//~ vram[i] = rand()%256;
	
	//~ for (i=0;i < 256; ++i) {
		//~ if (i < 64)
			//~ set_palette(i, i*4, 0, 0, 255);
		//~ else if (i < 128)
			//~ set_palette(i, 0, i*4, 0, 255);
		//~ else if (i < 192)
			//~ set_palette(i, 0, 0, i*4, 255);
		//~ else
			//~ set_palette(i, i*4, i*4, i*4, 0);
		//~ set_palette(i, i, i, i, 0);
	//~ }
	//~ for (i=0; i < 16; ++i) {
		//~ int ratio = (int)(i*256.0/14.99);
		//~ set_palette(i, ratio, 0, 0, 0);
		
	//~ }
	//~ for (i=0; i < 256; ++i) {
		//~ palette(i, 0, 0, i, 0);
		//~ vram[256*i+i] = 255;
	//~ }
	
	//~ for (i=0; i < 144; ++i) {
		//~ vram[256*i+i] = 255;
	//~ }

	uint8_t *pal = (uint8_t*)palette;
	for (i=0; i < 16; ++i)
		printf("%02x ", vram[256*144+i]);
	printf("\n");

	while(1) {
		frame ++;
		for (c=0; c < 256; ++c) {
			for (r=0; r < 144; ++r) {
				double z = (sin((c+frame/10)*M_PI/180) + cos(r*1.7*M_PI/180.0)+2.0)/4.0;
				vram[r*256+c] = (int)(z*16.0);
			}
		}
		usleep(25000);
	}
	return 1;
}