#include "client.h"
void frame_sync_interrupt(int frame)
{
	//~ printf("%d, %d %d\n", input->hoverX, input->hoverY, frame);
	vram[input->hoverY*256 + input->hoverX] = 3;
	vram[frame] = 2;
}

int main(int argc, char *argv[])
{
	io_init(argv[0]);
		
	    
	
	
	
	//~ char bob[4] = "9BC";
	//~ char bob[4] = {'9', 'B', 'C', 0};
	//~ char bob[4] = {58, 65, 66, 0};
	//~ bob[0] = 58;
	//~ bob[1] = 65;
	//~ bob[2] = 66;
	//~ bob[3] = 0;
	
	//~ char aaron = 100;
	//~ char mei[4] = {1,2,3,4};
	
	//~ mei[1] = 4;
	
	for (int i=0; i < 256*144; ++i) {
		vram[i] = i;
	}	
	while(! (io->input.status & STATUS_CLOSE));
	return 0;
}
