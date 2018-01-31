#include <pixie.h>
#include <unistd.h>

void frame_sync_interrupt(int frame)
{
	
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

	


	while(!(input->status & STATUS_CLOSE)) sleep(1);
	return 0;
}
