# Ceasy

A fun, simple library for playing with the C language through memory mapped IO.

C is a fun programming language because it is a very low, high-level language.  It maintains a close connection with the CPU and memory, and the high level constructs have a fairly straight-forward mapping into the underlying assembly code.

However, when just learning C for the first time, or trying to play around with the language, you are either stuck with <stdio.h> or confront large complicated interfaces of more capable libraries.  Neither of which is very fun.

Ceasy is a collection of 'virtual devices'.  Toys that can be programmed easily with C by using [memory mapped IO](#memory-mapped-io) instead of traditional functional interfaces.

The idea is that you write your code and compile it without any complicated linkages (or none).
  
  `gcc my_code.c`

Then, instead of executing your program directly, you execute it indirectly on one of the Ceasy 'virtual devices'

  `pixie a.out`.  


Currently there is only one 'device', [Pixie](#pixie).

Other planned devices include, RTS, FPS, Sports (hockey/soccer), Car racing, Board games, card games...

## Pixie

This device gives you a 256x144 pixel screen to draw on.  You can get input from the mouse and keyboard, and play some sounds.  See [documentation](src/pixie/README.md) for details...

### hello.c

```c
#include <pixie.h>

int main(int argc, char *argv[])
{
    io_init(argv[0]); // setup the memory mapped IO

    printf_xy(27, 12, "Hello World!"); // print some text
    vram[256*42 + 101] = 1;  // set pixel (101, 42)

    while (!(input->status & STATUS_CLOSE)); // wait for the window to close
    return 0;
}
```

# Compiling Ceasy

Just run `make` from the root directory and it will build subdirectories.

Nothing is installable.  Copy libpixie.a pixie.h and pixie to appropriate places if you want.

## Compiling your program

```
# gcc hello.c -o hello -Lpath/to/ceasy/pixie -lpixie 
# path/to/ceasy/pixie/pixie hello
```
# Memory mapped IO

The Ceasy devices are controlled through [memory mapped IO](https://en.wikipedia.org/wiki/Memory-mapped_I/O).  This means that special memory locations are given special meaning, and reading/writing of those memory locations can have special effects outside of your program.

This is normally only seen in microcontrollers where device pins and other functions are mapped into memory.  Operating systems quickly cover this up and expose function based interfaces instead.  So to make things more fun for C programmers, the devices in Ceasy are controlled through memory instead of function calls.

For example, setting the second bit at address 0xf16e sets off fireworks.  And reading the byte at 0xc01d gives you the current temperature in Antarctica.






