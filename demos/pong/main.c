#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define PIXIE_NOLIB
#include "pixie.h"
#include "vec2.c"

typedef struct s_GameState GameState;

struct s_GameState {
	Vec2 xy; // ball position
	Vec2 v; // ball velocity
	int p1Y, p2Y; // player 1,2 paddle position
	int score1, score2; // player score
	int state;
	int anim_frame; 
};

static GameState GS = {0}; // Global game state

void ball_set_pos(Vec2 xy)
{
	GS.xy = xy;
	layers[10].map.x = xy.x;
	layers[10].map.y = xy.y;
}

void ball_reset(void)
{
	ball_set_pos(v2(124, 68));
	GS.v = v2((rand()%2)*2.0 - 1.0, 0.0); // random left or right
	GS.v = v2mult(GS.v, 1.5 * rand()/RAND_MAX +1.0); // random speed
	GS.v = v2rot(GS.v, 45.0 * rand() / RAND_MAX  - 22.5); // random angle
}

/** This is called automatically once every frame.
 */
void frame_sync_handler(int frame)
{
	// update the ball position
	ball_set_pos(v2add(GS.xy, GS.v));
	
	if (input->alpha & (1<<('a'-'a')))
		layers[11].map.y --;
	if (input->alpha & (1<<('z'-'a')))
		layers[11].map.y ++ ;
	if (input->alpha & (1<<('o'-'a')))
		layers[12].map.y --;
	if (input->alpha & (1<<('l'-'a')))
		layers[12].map.y ++ ;
	
	 // bounce off player 2 paddle ? 
	float off_center = (GS.xy.y+4 - layers[12].map.y-16) / 16.0;
	if (GS.xy.x >= 256-12 && fabs(off_center) < 1.0)
		GS.v = v2reflect(GS.v, v2rot(v2(1.0, 0.0), -10.0*sin(M_PI*0.5*off_center)));
	
	// bounce off player 2 paddle ?
	off_center = (GS.xy.y+4 - layers[11].map.y-16) / 16.0;
	if (GS.xy.x <= 4 && fabs(off_center) < 1.0) 
		GS.v = v2reflect(GS.v, v2rot(v2(-1.0, 0.0), 10.0*sin(M_PI*0.5*off_center))); 


	if (GS.xy.x >= 256-8) { // player 1 wins
		ball_reset();
		GS.score1 += 1;
	}
	
	if (GS.xy.x <= 0) { // player 2 wins
		ball_reset();
		GS.score2 += 1;
	}
	
	// bounce off the the top and bottom
	if (GS.xy.y <= 0 || GS.xy.y >= 144-8)
		GS.v.y = -GS.v.y;
	
	// Draw the scores
	printf_xy(10,11, "%d  ", GS.score1);
	printf_xy(64-13,11, "%3d", GS.score2);
}

int main(int argc, char *argv[])
{
	io_init(argv[0]);
	srand((char)(*argv[0])); // throw some random bits as a seed
	
	// layer 10 is the ball
	
	copy32(vram + 0xF000, (uint32_t[]) { // put the palette data in vram
		0x00000000, 0x881231FF, 0xFF3214FF, 0xcc1234FF, 0xdd1234FF, 0x550102FF,
	}, 6);
	copy32(vram + 0x3F000, (uint32_t[]) { // put the ball tile data in vram
		0x00111100,0x01231110,0x12341111,0x13411111,0x11111111,0x11111551,0x01115510,0x00111100,
	}, 8);
	
	layers[10] = (Layer){
		.palette = 0xF000,
		.map.addr=0x3FFF0,  // this is just some zeros in vram  (only need one)
		.map.w = 1, .map.h = 1,
		.tiles.addr = 0x3F000,
		.tiles.w = 8, .tiles.h = 8,
		.tiles.row_bytes = 4,
		.tiles.bits =4,
		.flags = LAYER_ON,
	};
	
	// layer 11 is the player 1 paddle
	
	copy32(vram + 0x10000, (uint32_t[]) { // player 1 palette data
		0x00000000, 0x8812A1FF
	}, 2);
	copy16(vram + 0x12000, (uint16_t[]) { // pong paddle data
		0x0101, 0x1010, 0x0101, 0x1010,
	}, 4);
	layers[11] = (Layer) {
		.palette = 0x10000,
		.map.addr = 0x3FFF0, // this is just some zeros in vram  (only need eight)
		.tiles.addr = 0x12000,
		.map.w = 1, .map.h = 8,
		.tiles.w = 4, .tiles.h = 4,
		.tiles.bits = 4,
		.tiles.row_bytes = 2,
		.flags = LAYER_ON};

	
	// layer 12 is the player 2 paddle (Same as layer 11)
	layers[12] = layers[11];
	layers[12].map.x = 251;
	
	printf_xy(20,2, "Controls: (A,Z) (O,L)");
	
	ball_reset();
	
	on_frame_sync(frame_sync_handler);
	while(!(input->status & STATUS_CLOSE))
		sleep(1); // dont waste CPU in a busy loop.  Sleep and wait for frame_sync
	return 0;
}
