#include <pixie.h>

int main(int argc, char *argv[])
{
    io_init(argv[0]); // setup the memory mapped IO

    printf_xy(27, 12, "Hello World!"); // print some text
    vram[256*42 + 101] = 5;  // set pixel (101, 42) to color #5 (yellow)

    while (!(input->status & STATUS_CLOSE)); // wait for the window to close
    return 0;
}
