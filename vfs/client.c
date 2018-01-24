#include <stdio.h>
#include "cpu.h"

int main(int argc, char *argv[])
{
	cpu_init(argv[0]);

	io->screen[0] = 101;
	
	return 0;
}