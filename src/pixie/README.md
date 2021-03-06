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

By default, two layers are configured.

  * `layers[0]` is configured as a 256x144 map of the pixels on the screen.  i.e. no tiles (1x1)
  * `layers[1]` is configured as a 64x24 map of ASCII character tiles (4x6 pixels)

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

A tile is a 2d array of pixels with any width or height.  1x1 tiles are special.  They are just one pixel big, so no tile data is needed.  Each pixel of the tile can be configured as 1bit, 2bits or 4bits.  Those pixel values index into a [palette](#palettes) to determine the color on the screen.  Which palette gets used is controlled by the [layer](#layer) and the [tile map auxillary data](#auxillary-data).

The pixel data is stored sequencially packed into bytes from left to right, top to bottom.  For 1bit tiles 8 pixels can fit in one byte. 4 pixels for 2bit tiles, and 2 pixels for 8bit tiles.  Each row of the tile data always starts at the high end of a new byte.  So a 4x3 1bit tile would be three bytes `0xf0, 0xf0, 0xf0`.

### Tile Maps
A tile map is a 2d array of bytes.  Each byte in the map indexes to one [tile](#tiles).  Which set of tiles gets indexed is configured in the [layer](#layers).  As a special case, if the [tiles](#tiles) configured by the layer are 1x1, then the tile-map byte indexes directly to the [palette](#palette).

Tile maps can be any width or height, measured in tiles.  Their width in pixels would be `layer.map.w * layer.tiles.w`. 


### Palettes
The computer screen needs 24bit RGB values, but we only have 8bit entries in the tile maps, and a maximum of 4bits in a tile pixel.  

#### Auxillary Data

## Input

## Sound