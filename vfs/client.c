#include "client.h"

void sprite_init(Sprite *self, uint32_t data[8])
{
	for (int i=0; i < 8; ++i) {
		((uint8_t*)self)[i*4 + 0] = (data[i]>>24)&0xff;
		((uint8_t*)self)[i*4 + 1] = (data[i]>>16)&0xff;
		((uint8_t*)self)[i*4 + 2] = (data[i]>>8)&0xff;
		((uint8_t*)self)[i*4 + 3] = (data[i]>>0)&0xff;
	}
}

void obj_init(Object *obj, int address, int width, int height, int xbounds, int ybounds, Mapel *data)
{
	obj->address = address;
	obj->width = width;
	obj->height = height;
	obj->xoff = 0;
	obj->yoff = 0;
	for (int i =0 ;i < width*height; ++i) {
		maps[address + i] = data[i];
		//~ printf("%02x%c%c ", data[i].sprite, data[i].flipx?'x':' ', data[i].flipy?'y':' ');
	}
	obj->alpha = 0;
	obj->xBounds = xbounds;
	obj->yBounds = ybounds;
}

void obj_move(Object *obj, int xoff, int yoff)
{
	obj->xoff += xoff;
	obj->yoff += yoff;
}

void dump_mapels(int count)
{
	for (int i=0; i < count ;++i) {
		if (i && !(i%16))
			printf("\n");
		printf("%02x%c%c ", maps[i].sprite, maps[i].flipx?'x':' ', maps[i].flipy?'y':' ');
		
		
	}
	printf("\n");
	
	
}

int main(int argc, char *argv[])
{
	cpu_init(argv[0]);

	sprite_init(&sprites[0], (uint32_t[8]){ // heart
		0x00000000,
		0x01000100,
		0x11101110,
		0x11111110,
		0x11111110,
		0x01111100,
		0x00111000,
		0x00010000});
	sprite_init(sprites+1, (uint32_t[8]){ // grid
		0x22222225,
		0x20101015,
		0x21010105,
		0x20101015,
		0x21010105,
		0x20101015,
		0x21010105,
		0x24444444,});
	sprite_init(sprites+2, (uint32_t[8]){ // solid
		0x22222222,
		0x21111112,
		0x21111112,
		0x21111112,
		0x21111112,
		0x21111112,
		0x21111112,
		0x22222222,});
	sprite_init(&sprites[3], (uint32_t[8]){ // x
		0x20000002,
		0x01000010,
		0x00100100,
		0x00011000,
		0x00011000,
		0x00100100,
		0x01000010,
		0x20000002,});
	sprite_init(&sprites[4], (uint32_t[8]){ // asym
		0x11000022,
		0x11100222,
		0x01100220,
		0x00050000,
		0x00000000,
		0x03300440,
		0x33300444,
		0x33000044,});

	set_palette(0, 200, 200, 200, 255);
	set_palette(1, 255, 100, 100, 0);
	set_palette(2, 100, 255, 100, 0);
	set_palette(3, 100, 100, 255, 0);
	set_palette(4, 255, 255, 0, 0);
	set_palette(5, 100, 100, 100, 0);
	
	obj_init(&objects[0], 0, 4, 3, OBJ_REPEAT, OBJ_REPEAT, (Mapel[]){
		{0}, {2}, {2}, {1},
		{2}, {.sprite=4, .flipx=1}, {.sprite=4, .flipy=1}, {2},
		{4}, {2}, {2}, {3}});
		
	obj_init(&objects[1], 16, 1, 1, OBJ_FLIP, OBJ_FLIP, (Mapel[]){{4}});
	
	obj_move(&objects[0], -26, -43);
	dump_mapels(16);
		
	sleep(60);
	return 1;
}