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

void palette(unsigned char idx, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	vram[256*144 + idx] = r;
	vram[256*145 + idx] = g;
	vram[256*146 + idx] = b;
	vram[256*147 + idx] = a;
}

int main(int argc, char *argv[])
{
	cpu_init(argv[0]);

	int i, frame=0;
	
	palette(0, 255, 255, 0, 0);
	palette(1, 255, 0, 0, 0);
	palette(2, 0, 255, 0, 0);
	palette(3, 0, 0, 255, 0);
	
	
	int r, c;
	for (r=0; r < 144; ++r) {
		for (c=0; c < 256; ++c) {
			if (r < 25) {
				if (c < 66) {
					vram[r*256+c] = 0;
				} else {
					vram[r*256+c] = 1;
				}
			} else {
				if (c < 120) {
					vram[r*256+c] = 2;
				} else {
					vram[r*256+c] = 3;
				}
			}
		}
	}
	for (i=0;i < 144*256; ++i)
		vram[i] = rand()%4;
	
	
	for (i=0; i < 16; ++i) {
		int ratio = (int)(i*256.0/14.99);
		palette(i, ratio, ratio, ratio, 255);
		
	}
	//~ for (i=0; i < 256; ++i) {
		//~ palette(i, 0, 0, i, 0);
		//~ vram[256*i+i] = 255;
	//~ }
	
	//~ for (i=0; i < 144; ++i) {
		//~ vram[256*i+i] = 255;
	//~ }
	

	while(1) {
		//~ for (i=0; i < 256; ++i) {
			//~ palette((i+frame)%256, rand()%256, rand()%256, rand()%256, 0);
	for (i=0;i < 144*256; ++i)
		vram[i] = rand()%16;
		//~ }
		//~ unsigned char x = frame%256;
		//~ vram[256*x+x] = 255;
		//~ frame ++;
		usleep(25000);
	}
	return 1;
}