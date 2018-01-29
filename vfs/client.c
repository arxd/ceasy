#include "client.h"

void sprite_init32(int idx, int rows, uint32_t *data)
{
	for (int i=0; i < rows; ++i) {
		vram[idx+i*4 + 0] = (data[i]>>24)&0xff;
		vram[idx+i*4 + 1] = (data[i]>>16)&0xff;
		vram[idx+i*4 + 2] = (data[i]>>8)&0xff;
		vram[idx+i*4 + 3] = (data[i]>>0)&0xff;
	}
}
void sprite_init16(int idx, int rows, uint16_t *data)
{
	for (int i=0; i < rows; ++i) {
		vram[idx+i*2 + 0] = (data[i]>>8)&0xff;
		vram[idx+i*2 + 1] = (data[i]>>0)&0xff;
	}
}

void sprite_init8(int idx, int rows, uint8_t *data)
{
	for (int i=0; i < rows; ++i) {
		vram[idx+i] = data[i];
	}
}

//~ void obj_init(Object *obj, int address, int width, int height, int xbounds, int ybounds, Mapel *data)
//~ {
	//~ obj->address = address;
	//~ obj->width = width;
	//~ obj->height = height;
	//~ obj->xoff = 0;
	//~ obj->yoff = 0;
	//~ for (int i =0 ;i < width*height; ++i) {
		//~ maps[address + i] = data[i];
		//~ printf("%02x%c%c ", data[i].sprite, data[i].flipx?'x':' ', data[i].flipy?'y':' ');
	//~ }
	//~ obj->alpha = 0;
	//~ obj->xBounds = xbounds;
	//~ obj->yBounds = ybounds;
//~ }

//~ void obj_move(Object *obj, int xoff, int yoff)
//~ {
	//~ obj->xoff += xoff;
	//~ obj->yoff += yoff;
//~ }

//~ void dump_mapels(int count)
//~ {
	//~ for (int i=0; i < count ;++i) {
		//~ if (i && !(i%16))
			//~ printf("\n");
		//~ printf("%02x%c%c ", maps[i].sprite, maps[i].flipx?'x':' ', maps[i].flipy?'y':' ');
		
		
	//~ }
	//~ printf("\n");
	
	
//~ }

int main(int argc, char *argv[])
{
	cpu_init(argv[0]);

	set_palette(256, 0xddddddff);
	set_palette(260, 0xff555500);
	set_palette(264, 0x55ff5500);
	set_palette(268, 0x5555ff00);
	set_palette(272, 0xffff0000);
	set_palette(276, 0x44444400);

	for (int i=0; i < 256; ++i)
		vram[i] = i%10;  
	
	layers[0].map = 0;
	layers[0].mapW = 10;
	layers[0].mapH = 1;
	layers[0].mapX = 0;
	layers[0].mapY = 0;
	layers[0].palette = 256;
	layers[0].tiles =256*200;
	layers[0].tileW = 4;
	layers[0].tileH = 6;
	layers[0].tileBits = 1;
	layers[0].border = (BDR_CLIP << 4) | BDR_CLIP;
	
	sprite_init8(256*200 + 6*0, 6, (uint8_t[]){ 0x00, 0x40, 0xa0, 0xa0, 0xa0, 0x40 });
	sprite_init8(256*200 + 6*1, 6, (uint8_t[]){ 0x00, 0x40, 0xc0, 0x40, 0x40, 0xe0 });
	sprite_init8(256*200 + 6*2, 6, (uint8_t[]){ 0x00, 0xc0, 0x20, 0x40, 0x80, 0xe0 });
	sprite_init8(256*200 + 6*3, 6, (uint8_t[]){ 0x00, 0xe0, 0x20, 0x40, 0x20, 0xe0 });
	sprite_init8(256*200 + 6*4, 6, (uint8_t[]){ 0x00, 0x80, 0xa0, 0xe0, 0x20, 0x20 });
	sprite_init8(256*200 + 6*5, 6, (uint8_t[]){ 0x00, 0xe0, 0x80, 0xc0, 0x20, 0xc0 });
	sprite_init8(256*200 + 6*6, 6, (uint8_t[]){ 0x00, 0x60, 0x80, 0xc0, 0xa0, 0xe0 });
	sprite_init8(256*200 + 6*7, 6, (uint8_t[]){ 0x00, 0xe0, 0x20, 0x40, 0x40, 0x40 });
	sprite_init8(256*200 + 6*8, 6, (uint8_t[]){ 0x00, 0xe0, 0xa0, 0xe0, 0xa0, 0xe0 });
	sprite_init8(256*200 + 6*9, 6, (uint8_t[]){ 0x00, 0xc0, 0xa0, 0xe0, 0x20, 0x20 });
	
	sprite_init32(256*100, 8,  (uint32_t[]){ // heart
		0x00000000,
		0x01000100,
		0x11101110,
		0x11111110,
		0x11111110,
		0x01111100,
		0x00111000,
		0x00010000});
	sprite_init32(256*100+32*1, 8, (uint32_t[]){ // grid
		0x22222225,
		0x20101015,
		0x21010105,
		0x20101015,
		0x21010105,
		0x20101015,
		0x21010105,
		0x24444444,});
	//~ sprite_init(sprites+2, (uint32_t[8]){ // solid
		//~ 0x22222222,
		//~ 0x21111112,
		//~ 0x21111112,
		//~ 0x21111112,
		//~ 0x21111112,
		//~ 0x21111112,
		//~ 0x21111112,
		//~ 0x22222222,});
	//~ sprite_init(&sprites[3], (uint32_t[8]){ // x
		//~ 0x20000002,
		//~ 0x01000010,
		//~ 0x00100100,
		//~ 0x00011000,
		//~ 0x00011000,
		//~ 0x00100100,
		//~ 0x01000010,
		//~ 0x20000002,});
	//~ sprite_init(&sprites[4], (uint32_t[8]){ // asym
		//~ 0x11000022,
		//~ 0x11100222,
		//~ 0x01100220,
		//~ 0x00050000,
		//~ 0x00000000,
		//~ 0x03300440,
		//~ 0x33300444,
		//~ 0x33000044,});

	
	//~ obj_init(&objects[0], 0, 4, 3, OBJ_REPEAT, OBJ_REPEAT, (Mapel[]){
		//~ {0}, {2}, {2}, {1},
		//~ {2}, {.sprite=4, .flipx=1}, {.sprite=4, .flipy=1}, {2},
		//~ {4}, {2}, {2}, {3}});
		
	//~ obj_init(&objects[1], 16, 1, 1, OBJ_FLIP, OBJ_FLIP, (Mapel[]){{4}});
	
	//~ obj_move(&objects[0], -26, -43);
	//~ dump_mapels(16);
		
	sleep(60);
	return 1;
}