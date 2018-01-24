#include <stdio.h>
#include "cpu.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
	while (argc--)
		printf("ARG %d: %s\n", argc, argv[argc]);
	cpu_init(argv[0]);

	int x = 10;
	while (x--) {
		io.palette[0] = x+100;
		sleep(1);
	}
	return 0;
}