# ceasy

A fun, simple library for playing with the C language through memory mapped IO.

C is a very fun programming language because it is the lowest high-level language.
It maintains a close connection with the CPU and memory.  
There are few abstractions to get in the way.

This is a collection of 'devices' that can be programmed easily with C by using [memory mapped IO](#memory-mapped-io).
MMIO is usually seen in microcontrollers, and quickly abstracted away by operating systems.

Currently there is only one 'device', [Pixie](#pixie).

## Pixie

This device gives you a 256x144 pixel screen to draw on.  You can get input from the mouse and keyboard, and play some sounds.  See [documentation](#pixie-documentation) for details...

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

# Memory mapped IO

The Ceasy devices are controlled through memory mapped IO.  That means that special memory locations are given special meaning, and reading/writing of those memory locations can have special effects.  

This is normally only seen in microcontrollers where device pins and other functions are mapped into memory.  Operating systems quickly cover this action up and expose function based interfaces instead.  So to make things more fun for C programmers, the devices in Ceasy are controlled through memory instead of function calls.

For example, setting the second bit at address 0xf16e sets off fireworks.  And reading the byte at 0xc01d gives you the current temperature in Antarctica.



# Pixie Documentation

Since the Pixie device is controlled through memory, the documentation consists of listing the memory locations and what
happens if you read/write them.

The `IOMem` struct in `iomem.h` shows all the shared memory. There is:

  * [vram](#vram) : A place for storing [palettes](#palettes), [tile-maps](#tile-maps) and [tile-data](#tiles)
  * [input](#input) : Mouse, and keyboard input
  * [voices](#sound) : Sound output
  * [layers](#layers) : Graphics control


## VRAM

VRAM (Video Random Access Memory) is the place where [palettes](#palettes), [tile-maps](#tile-maps) and [tile-data](#tiles) are stored.  It is 512x512 bytes large.  That is 252144 bytes or 0x40000 in hex.  Assuming the [layers](#layers) are configured properly, changing this data has an immediate (next rendered frame) effect on the display.  By default the vram configured as follows:

  * 0x00000: A [tile-map](#tile-maps) of 1x1 tiles.  Each byte maps straight into the palette at 0x09000.
  * 0x09000: [Palette](#palettes) memory.
  * 0x09400: ASCII 4x6 [tile](#tiles) data.  
  * 0x09700: A [tile-map](#tile-maps) of ascii characters.  Each byte maps to an ASCII character.  64x24 characters.  
  * 0x0a300: This is the [auxillary](#auxillary-data) data for the ASCII characters.

Of course, this is just the default setup and the memory can be controlled however you want.

## Layers

The graphics system is composed of layers of graphics with upper layers obscuring lower layers.
There are 256 layers available and they are all rendered
from bottom `layers[0]` to top `layers[255]` every frame (60fps).  Each `struct s_Layer` configures:

  * A 2d 8bit [map of tiles](#tile-maps).  Each byte indexes into the array of [tiles](#tiles).
  * A 1d array of [tile](#tiles) data.
  * A [palette](#palettes)
  * `border` flags control what is displayed outside the bounds of the layers' edges.
  * `flags` that control other aspects of how the layer is rendered.

### layer.border

If the layer size is smaller than the screen size, what happens to the on-screen pixels outside of the map?

  * BDR_CLIP: Nothing is drawn.  You can see the layers below.  ...     01234     ...
  * BDR_CLAMP: The edge-most tile is copied out indefinitely.  ...000000123444444...
  * BDR_FLIP: This is the same as BDR_CLAMP execpt every other tile is flipped.
  * BDR_REPEAT: The entire map repeats indefinitely in both directions   ...340123401234012...

The vertical direction is controlled independantly from the horizontal direction.  The high 4bits of `layer.border` controls the X direction and the lower 4bits control the Y direction. `BDR_CLAMP | (BDR_FLIP << 4)`

### layer.flags

This is a bitfield of flags that control how the layer is rendered.

  * LAYER_ON : The lowest bit controls whether or not the layer is drawn.
  * LAYER_FLIPX : Flip the whole layer left to right.
  * LAYER_FLIPY : Flip the whole layer top to bottom.
  * LAYER_AUX : Does the layer [map](#tile-maps) use [auxillary data]?(#auxillary-data)



### Tiles

A tile is a 2d array of pixels with any width or height (8x8) is nice.  Each pixel of the tile can be configured as 1bit, 2bits or 4bits.  Those pixel values index into a [palette](#palettes) to determine the color on the screen.  Which palette gets used is controlled by the [layer](#layer)

### Palettes
The computer screen needs RGB values, but we only hav

### Tile Maps

#### Auxillary Data

## Input

## Sound


# Compiling Ceasy

Just run `make` from the root directory and it will build subdirectories.

Nothing is installable.  Copy libpixie.a pixie.h and pixie to appropriate places if you want.

## Compiling your program

```
# gcc hello.c -o hello -Lpath/to/ceasy/pixie -lpixie 
# path/to/ceasy/pixie/pixie hello
```







