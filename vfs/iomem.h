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

#define AUX_NONE (0)
#define AUX_PAL8 (1)
#define AUX_ALPHA8 (2)

#define LAYER_ON (1)
//~ #define LAYER_OFF (0)
#define MAX_LAYERS 256

typedef struct s_Map Map;
struct s_Map {
	uint16_t addr;	///< Address of the map data
	uint16_t aux;	///< Address of the auxillary map data (if equal to addr then not used)
	uint16_t w,h;	///< Width/Height of the layer map (in tiles)
	int16_t x,y;	///< Signed screen offset in pixels
};

typedef struct s_Tiles Tiles;
struct s_Tiles {
	uint16_t addr;		///< Address of the tile data
	uint16_t w,h;		///< Width / Height of one tile in pixels
	uint16_t row_bytes;	///< Number of bytes to encode one row of a tile
	uint8_t bits;		///< Number of bits per pixel (1, 2, 4)
	uint8_t num_tiles;	///< Number of tiles - 1 (helpful information) (max 256)
};

typedef struct s_Layer Layer;
struct s_Layer {
	Map map;
	Tiles tiles;
	uint16_t palette;	///< Address of the palette data
	uint8_t border;		///< How tiles outside the map are handled.  x(high nibble) y(low nibble)
	uint8_t flags;		
};

typedef struct s_IOMem IOMem;
struct s_IOMem {
	Input input;
	Voice voices[32];
	Layer layers[MAX_LAYERS];
	// fowllowing data is 256*256 to fit in a texture
	uint8_t vram[256*256];
};


#endif
