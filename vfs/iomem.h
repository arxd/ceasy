#ifndef IOMEM_H
#define IOMEM_H
#include <stdint.h>
#include <string.h>

typedef struct s_Voice Voice;
struct s_Voice{
	uint8_t volume;
	uint8_t instrument;
	uint16_t pitch;
	uint16_t param;
	
};

typedef struct s_Input Input;
struct s_Input {
	uint32_t frame;
	uint8_t status;
	uint8_t reserved[3];
	// inputs
	struct {
		uint8_t buttons[2];
		uint8_t x;
		uint8_t y;
	} touch[4];
	
	struct {
		int8_t ain[4];
		uint8_t buttons[2];
	} controllers[4];
	
	uint8_t keys[16];
};

typedef struct s_Color Color;
struct s_Color {
	uint8_t r,g,b,a;
};

#define BDR_CLIP (0)
#define BDR_CLAMP (1)
#define BDR_REPEAT (2)
#define BDR_FLIP (3)

typedef struct s_Layer Layer;
struct s_Layer {
	uint16_t map;		///< address of the layer map
	uint16_t mapW;	///< Width of the layer map (in tiles)
	uint16_t mapH;	///< Height
	int16_t mapX;		///< x offset of the layer map (in pixles)
	int16_t mapY;		///< y offset
	uint16_t tiles;		///< Address of the tile data
	uint8_t tileW;		///< Width of a tile (in pixels).
	uint8_t tileH;		///< Height
	uint16_t palette;	///< Address of the palette data
	uint8_t tileBits;		///< Bits per tile texel (1, 2, 4, 8)
	uint8_t border;		///< How tiles outside the map are handled.  x(high nibble) y(low nibble)
};

typedef struct s_Object Object;
struct s_Object {
	uint16_t address;
	uint8_t width;
	uint8_t height;
	int16_t xoff;
	int16_t yoff;
	uint8_t alpha;
#ifdef LITTLE_ENDIAN
	uint8_t xBounds:2;
	uint8_t yBounds:2;
	uint8_t flags:4;
#else
	uint8_t flags:4;
	uint8_t yBounds:2;
	uint8_t xBounds:2;
#endif
};

typedef struct s_Sprite Sprite;
struct s_Sprite {
	uint8_t data[8*4];
};

typedef struct s_Mapel Mapel;
struct s_Mapel {
	uint8_t sprite;
#ifdef LITTLE_ENDIAN
	uint8_t flipx:1;
	uint8_t flipy:1;
	uint8_t alpha :2;
	uint8_t color:4;
#else
	uint8_t color:4;
	uint8_t alpha :2;
	uint8_t flipy:1;
	uint8_t flipx:1;
#endif	
};

typedef struct s_IOMem IOMem;
struct s_IOMem {
	Input input;
	Voice voices[32];
	Layer layers[256];
	// fowllowing data is 256*256 to fit in a texture
	uint8_t vram[256*256];	// 144 rows
	//~ Color palette[256];		// 4 rows
	//~ Sprite sprites[256];		// 32 rows
	//~ Mapel maps[9728];		// 76 rows
};


#endif
