#include "client.h"
void framesync(int frame)
{
	printf_xy(54, 0, "%10d", frame);
	vram[frame] = 2;
	layers[0].flags = frame%10 < 4;
}

int main(int argc, char *argv[])
{	
	cpu_init(argv[0]);

	//~ tiles_init(&layers[0].tiles, 0x00, 1, 1, 1);
	
	//~ tiles_init_load32(&layers[0].tiles, 0x600, 8, 8, 4, 5, (uint32_t[]){ // heart
		//~ 0x00000000,0x01000100,0x11101110,0x11111110,0x11111110,0x01111100,0x00111000,0x00010000,
		//~ 0x11000022,0x11100222,0x01100220,0x00050000,0x00000000,0x03300440,0x33300444,0x33000044,
		//~ 0x22222225,0x20101015,0x21010105,0x20101015,0x21010105,0x20101015,0x21010105,0x24444444,
		//~ 0x22222222,0x21111112,0x21111112,0x21111112,0x21111112,0x21111112,0x21111112,0x22222222,
		//~ 0x20000002,0x01000010,0x00100100,0x00011000,0x00011000,0x00100100,0x01000010,0x20000002,
	//~ });
	
	printf_xy(0, 0, "Hello World!");
	int_framesync(framesync);
	
	map_init(&layers[2].map, 0xa300, 16, 16, 0);
	layers[2].palette = 0x9000;
	tiles_init(&layers[2].tiles, 0xa300, 1, 1, 1);
	layers[2].flags = LAYER_ON;
	layers[2].map.y = -40;
	layers[2].map.x = -40;
	
	for (int i=0; i < 256; ++i)
		vram[0xa300+i] = i;
	
	
	//~ int frame = 0;
	while(1) {
		//~ frame ++;
		//~ if (frame%1000000 == 0)
		sleep(1);
		//~ usleep(25000);
	}
	return 1;
}