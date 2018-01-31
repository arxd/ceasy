# ceasy
A fun, simple library for playing with the C language through memory mapped IO.

C is a very fun programming language because it is the lowest high-level language.
It maintains a close connection with the CPU and memory.  
There are few abstractions to get in the way.

This is a collection of 'devices' that can be programmed easily with C by using memory mapped IO.
MMIO is usually seen in microcontrollers, and quickly abstracted away by operating systems.

# TLDR;

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
# gcc test.c -o test -lpixie
# pixie test
```

# Pixie
This device gives you a 256x144 pixel screen to draw on.





