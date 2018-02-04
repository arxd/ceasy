#define HOCKEY_NOLIB
#include "hockey.h"

int main(int argc, char *argv[])
{
    io_init(argv[0]); // setup the memory mapped IO

    //~ printf_xy(27, 11, "Hello World!"); // print some text
    //~ vram[256*68 + 129] = 5;  // set pixel (101, 42) to color #5 (yellow)

    while (!(input->status & STATUS_CLOSE))
	    sleep(1); // wait for the window to close
    return 0;
}
