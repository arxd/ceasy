#include <stdio.h>
#include "cpu.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
	while (argc--)
		printf("ARG %d: %s\n", argc, argv[argc]);
	cpu_init(argv[0]);

	unsigned char x = 0;
	while (1) {
		io.vram[++x] = x;
	}
	return 0;
}