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

typedef struct s_Object Object;
struct s_Object {
	uint16_t address;
	uint8_t width;
	uint8_t height;
	int16_t xoff;
	int16_t yoff;
	uint8_t alpha;
	uint8_t flags;
};

typedef struct s_Sprite Sprite;
struct s_Sprite {
	uint8_t data[8*4];
};

typedef struct s_Mapel Mapel;
struct s_Mapel {
	uint8_t sprite;
	uint8_t flipx:1;
	uint8_t flipy:1;
	uint8_t alpha :2;
	uint8_t color:4;
};

typedef struct s_IOMem IOMem;
struct s_IOMem {
	Input input;
	Voice voices[32];
	Object objects[256];
	// fowllowing data is 256*256 to fit in a texture
	uint8_t vram[256*144];
	Color palette[256];
	Sprite sprites[256];
	Mapel maps[9728];
};

IOMem *io;
Voice *voices;
Object *objects;
uint8_t *vram;
Color *palette;
Sprite *sprites;
Mapel *maps;

void iomem_init(void *mem)
{
	io = (IOMem*)mem;
	voices = io->voices;
	objects = io->objects;
	vram = io->vram;
	palette = io->palette;
	sprites = io->sprites;
	maps = io->maps;
	
}

#endif
