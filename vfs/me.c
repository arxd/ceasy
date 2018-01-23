#include <stdio.h>
#include <stdlib.h>
#include "gfx.c"

double frnd() { return rand()*(1.0/RAND_MAX);}


int main(int argc, char *argv[])
{
	if (!graphics_connect(argv[0]))
		printf("No graphics\n");
	int f=0;
	
	for(double x = -32; x<32; ++x) {
		for (double y=-18; y < 18; y++) {
			if ((((int)x+32)^((int)y+18))&1)
				gfx_triangles(4, x, y, x+1, y, x, y+1, x+1, y+1);
		}
		
	}
	
	
	
	//~ while(1) {
		//~ f = gfx_sync();		
		
		//~ gfx_triangles(3, frnd()*10,frnd()*10, frnd()*10, frnd()*10, frnd()*10, frnd()*10);
		
		//~ fflush(stdout);
	//~ }
	//~ int *x = 0;
	//~ *x = 99;
	return -4;
}
