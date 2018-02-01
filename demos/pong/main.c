#include <pixie.h>
#include <unistd.h>

void frame_sync_interrupt(int frame)
{
	if (input->hoverY == 255) {
		printf_xy(10,10, "OUT");
	} else {
		vram[input->touch[0].y*256 + input->touch[0].x] = input->touch[0].buttons;
		printf_xy(10, 10, "IN ");
	}
	printf_xy(50, 1, "%.2f", now());
	printf_xy(20, 2, "%.2d %.2d        ", input->touch[0].x0, input->touch[0].y0); 
	printf_xy(50, 2, "%.2d %.2d        ", input->touch[0].x, input->touch[0].y); 
	printf_xy(50, 3, "%d       ", input->touch[0].buttons);
	
	static int chpos = 64*10;
	if (input->getchar) {
		vram[layers[1].map.addr + (chpos++)] = input->getchar;
		input->getchar = 0;
	}
	
	for (int a=0; a < 26;++a) 
		vram[layers[1].map.aux + a] = (input->alpha >> a)&1;

	
}




int main(int argc, char *argv[])
{
	io_init(argv[0]);
	
	for (int a = 0; a < 26; ++a)
		vram[layers[1].map.addr + a] = 'A'+a;
	
	
	for (int i=0; i < 256*144; ++i) {
		vram[i] = i;
	}	
	while(!(input->status & STATUS_CLOSE))
		sleep(1);
	return 0;
}
