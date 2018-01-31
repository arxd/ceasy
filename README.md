# ceasy
A fun, simple library for playing with the C language through memory mapped IO.

C is a very fun programming language because it is the lowest high-level language.
It maintains a close connection with the CPU and memory.  
There are few abstractions to get in the way.

This is a collection of 'devices' that can be programmed easily with C by using memory mapped IO.
MMIO is usually seen in microcontrollers, and quickly abstracted away by operating systems.

## Pixie
This device gives you a 256x144 pixel screen to draw on.

The `IOMem` struct in `iomem.h` shows how the memory mapped IO is layed out.

The `io->vram` aka `vram` is where data for drawing is stored.  It is 0x40000 bytes big.
The [tiles](#tile), [tile-maps](#tile-map), and [palettes](#palette) can be allocated anywhere in that space that you like.

Rendering is controlled by the `io->layers` structures.  There are 256 layers available and they are all rendered
from bottom `io->layers[0]` to top `io->layers[255`.  Each layer sets up three^* objects to use:
	1) A 1d array of [tile](#tile) data.
	2) A 2d 8bit map of tiles to be rendered.
	
is a WxH map   
A tile is an NxM array of pixels.  Each pixel is one byte so a pixels color is looked up in a palette.


## Compiling

Just run `make` from the root directory and it will build subdirectories.

Nothing is installable.  Copy libpixie.a pixie.h and pixie to appropriate places if you want.

## TLDR;

test.c
```c
#include <pixie.h>

void frame_sync_interrupt(int frame)
{
    if (input->hoverY < 144)
        vram[256*input->hoverY + input->hoverX] = 2;
}

int main(int argc, char *argv[])
{
    io_init(argv[0]); // setup the memory mapped IO

    printf_xy(27, 12, "Hello World!");

    while (!(input->status & STATUS_CLOSE)); // just handle frame_sync and do nothing else
    return 0;
}
```

Run it
```
# gcc test.c -o test -Lpath/to/ceasy/pixie -lpixie 
# path/to/ceasy/pixie/pixie test
```






