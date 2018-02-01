#include <pixie.h>
#include <unistd.h>

typedef struct s_Vec2 Vec2;
struct s_Vec2 {
	float x, y;
};

struct s_GameState {
	Vec2 xy; // ball position
	Vec2 v; // ball velocity
	int p1Y, p2Y; // player 1,2 paddle position
	int score1, score2; // player score
	int state;
	int anim_frame; 
};

static struct s_GameState GS = {0}; // Global game state


void ball_set_pos(float x, float y)
{
	GS.xy = (Vec2){x,y};
	layers[10].map.x = GS.xy.x;
	layers[10].map.y = GS.xy.y;	
}

float rand_float(float from, float to)
{
	return ((float)rand()/(float)RAND_MAX)*(to-from) + from;
}

float rand_dir(void)
{
	return (rand()%2)?1.0:-1.0;
}

void ball_reset(void)
{
	// initialize the ball
	ball_set_pos(124.0, 68.0);
	GS.v.x = rand_float(1.0, 2.0) * rand_dir();
	GS.v.y = rand_float(-1.5, 1.5);
}

void frame_sync_interrupt(int frame)
{
	// update the ball position
	ball_set_pos(GS.xy.x + GS.v.x, GS.xy.y + GS.v.y);
	
	// update the ball velocity
	if (GS.xy.x >= 256-8) { // player 1 wins
		ball_reset();
		GS.score1 += 1;
		GS.v.x = -GS.v.x;
	}
	
	if (GS.xy.x <= 0) { // player 2 wins
		ball_reset();
		GS.score2 += 1;
		GS.v.x = -GS.v.x;
	}
	
	// bounce of the the top and bottom
	if (GS.xy.y <= 0 || GS.xy.y >= 144-8)
		GS.v.y = -GS.v.y;
	
	// Draw the scores
	printf_xy(1,1, "%d  ", GS.score1);
	printf_xy(60,1, "%3d", GS.score2);
	
}

int main(int argc, char *argv[])
{
	io_init(argv[0]);
	srand((int)(*argv)); // throw some random bits as a seed
	
	palette_init(0xF000, 6, (uint32_t[]) {0x00000000, 0x881231FF, 0xFF3214FF, 0xcc1234FF, 0xdd1234FF, 0x550102FF,});
	
	// don't need layer 0
	layers[0].flags = 0;
	
	// layer 10 is the ball
	map_init(&layers[10].map, 0x3FFFFF, 1, 1, 0); // just a single tile. The ball
	tiles_init_load32(&layers[10].tiles, 0x3f0000, 8, 8, 4, 1, 
		(uint32_t[]){0x00111100,0x01231110,0x12341111,0x13411111,0x11111111,0x11111551,0x01115510,0x00111100,});
	layers[10].flags = LAYER_ON;
	layers[10].palette = 0xF000;
	ball_reset();
		
		
	while(!(input->status & STATUS_CLOSE))
		sleep(1);
	return 0;
}
