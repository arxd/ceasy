#define PIXIE_NOLIB
#include "pixie.h"

#include "trump.c"


void show_num_recursion(int x)
{ //432
	if (x/10 != 0)
		show_num_recursion(x/10);
	draw_num(128, 72, x%10, 5, 2);
	//~ putchar(x%10 + '0');
}



void show_num(int x)
{
	
	
	
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
	
	show_num(432);
	
	
	while(!(input->status & STATUS_CLOSE))
		sleep(1);
	
	return 0;
}









