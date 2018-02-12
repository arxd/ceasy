#define PIXIE_NOLIB
#include "pixie.h"

int main(int argc, char *argv[])
{
    pixie_init(argv[0]); // setup the memory mapped IO

    printf_xy(27, 11, "Hello World!"); // print some text
    vram[256*68 + 129] = 5;  // set pixel (101, 42) to color #5 (yellow)

    while (!(input->status & STATUS_CLOSE)); // wait for the window to close
    return 0;
}
