#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define PIXIE_NOLIB
#include "pixie.h"

#include "trump.c"
//~ void draw_num(int x, int y, int num, int color, int size);
//~ void draw_suit(int x, int y, int suit, int size);


void show_num(int x)
{ //231
	if (x/10 != 0)
		show_num(x/10);
	putchar(x%10 + '0');
}

void draw_ball(int x, int y,int z)
{	vram[x%256+y*256]  = z;
	vram[x%256+y*256+1]  = z;
	vram[x%256+y*256+256]  = z;
	vram[x%256+y*256+257]  = z;
}

int main(int argc, char *argv[])
{	
	pixie_init(argv[0]); // magic
	
	show_num(237587581);

	while(!(input->status & STATUS_CLOSE))
		sleep(1);
	
	return 0;
}









