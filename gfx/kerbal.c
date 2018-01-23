#include "engine.c"


typedef struct {
	int r, c, type;
} Pos;


int size;
int halfsize;
double prev_time;

float hex_side = 0.5773502691896258f;

int apples_eaten;
Pos apple;

int cur_level;
Pos level_2_walls[] = { {7,4}, {7,5}, {7,6}, {7,7}, {7,8}, {7,9}, {7,10}};
Pos level_3_walls[] = { {7,4}, {3,5}, {8,6}, {3,7}, {5,8}, {15,9}, {7,10}, {13, 4}, {12, 8}, {10, 2}, {2, 10}, {3, 13}};
Pos level_4_walls[] = { {7,0}, {8,0}, {6,1}, {7,1}, {7,2}, {7,3}, {7,4}, {7,5}, {7,6},  {7,8}, {7,9}, {7,10}, {7,11}, {7,12}, {7,13}, {7,14}, {8, 13}, {6,14}};

Pos *level_walls[] = {NULL, level_2_walls,  level_4_walls, level_3_walls,};
int level_walls_len[] = {0, 7,  18,   12,};
int num_levels = 4;

int body_should_turn;
int grow_more_body;
int body_dir;
int body_len;
int body_head;
Pos body[256];
char body_parts[256];
void (*state)(void);

double speed = 0.2;

Pos pos(int r, int c)
{
	Pos rc;
	rc.r = r;
	rc.c = c;
	rc.type = 0;
	return rc;
}

int pos_eq(Pos a, Pos b)
{
	return a.r == b.r && a.c == b.c;
}

void main_transform(void)
{
	translate(3,1.5);
	scale(1.25,1.25);	
}

int on_board(Pos rc)
{
	if (rc.r < 0 || rc.c < 0)
		return 0;
	if (rc.r >= size || rc.c >= size)
		return 0;
	if (rc.r < halfsize && rc.c < halfsize - rc.r)
		return 0;
	if (rc.r >= size - halfsize && rc.c >= size-(rc.r-(size-halfsize-1)))
		return 0;
	return 1;	
}

int on_wall(Pos rc)
{
	int i;
	for (i=0; i < level_walls_len[cur_level%num_levels]; ++i) {
		if (pos_eq(rc, level_walls[cur_level%num_levels][i]))
			return 1;
	}
	return 0;
}

int on_body(Pos rc)
{
	int i;
	for (i=0; i < body_len; ++i) {
		if (pos_eq(body[i], rc))
			return 1;
	}
	return 0;
}


void new_apple(void)
{
	apple.r = -1;
	apple.c = -1;
	while(!on_board(apple) || on_body(apple) || on_wall(apple)) {
		apple.r = rand()%size;
		apple.c = rand()%size;
	}
	
}


void grow_body(void)
{
	int i = body_head;
	Pos tmp, prev = body[i];
	while (++i <= body_len) {
		tmp = body[i];
		body[i] = prev;
		prev = tmp;
	}
	++body_len;
	-- grow_more_body;
}


void body_render(void)
{
	int i, r, c;
	cur_color = rgb(0.0, 0.0, 0.5);
	for (i =0; i < body_len; ++i ) {
		Pos head = body[(body_head+1+i)%body_len];		
		push_matrix();
		translate(head.c+0.5*head.r, head.r*(1.0-hex_side/4.0));
		if (head.type>>3) {
			rotate(60*(head.type&0x7));
			draw_glyph_xy(V0, ALIGN_CENTER, 161);
		} else {
			rotate(60*head.type);
			draw_glyph_xy(V0, ALIGN_CENTER, 160);
		}
		pop_matrix();
	}
}

void apple_render(void)
{
	push_matrix();
	cur_color=rgb(1.0, 0.0, 0.0);
	translate(apple.c+0.5*apple.r,apple.r*(1.0-hex_side/4.0));
	draw_glyph_xy(V0, ALIGN_CENTER, 163);
	cur_color=rgb(1.0, 1.0, 0.0);
	scale(0.4, 0.4);
	draw_glyph_xy(V0, ALIGN_CENTER, 150);
	pop_matrix();
}

void stats_render(void)
{
	push_matrix();
	cur_color = rgb(1.0, 1.0, 0.9);
	draw_string_xy(V(6.2, 16), ALIGN_LEFT, "%d\n%d", cur_level+1, 10-apples_eaten);
	pop_matrix();
	
}

void game_render(void)
{
	push_matrix();
	stats_render();
	main_transform();
	body_render();
	apple_render();
	pop_matrix();
}

void new_game(void);
void run_wait(void);

void reset_body(void)
{
	grow_more_body = 0;
	apples_eaten = 0;
	prev_time = cur_time();
	body_dir = 1;
	body_len = 1;
	body_head = 0;
	body[0] = pos(8, 7);
	body[0].type = 9;
	body_should_turn = 0;
}


void game_over(void)
{
	game_render();
	cur_color = rgb(1.0, 1.0, 0.9);
	translate(16,9);
	scale(3,3);
	draw_string_xy(V0, ALIGN_CENTER, "Try Again!");
	if (cur_state == PRESS) {// wait for a click to start the next game
		reset_body();
		state = run_wait;
	}
}

void draw_level(void)
{
	int r, c;
	start_background_draw();
	cur_color = rgb(1.0, 1.0, 0.9);
	//scale(2, 2);
	draw_string_xy(V(0, 16), ALIGN_LEFT, "Level:\nLeft :");

	main_transform();
	push_matrix();
	translate(halfsize+0.5*halfsize, halfsize *(1.0-hex_side/4.0));
	scale(size-0.9,size-0.9);
	rotate(30);
	cur_color = hex(0x663300);
	draw_glyph_xy(V0, ALIGN_CENTER, 156);
	pop_matrix();
	for(r=0; r < size; ++r) {
		for (c=0; c < size; ++c) {
			if (!on_board(pos(r, c)))
				continue;
			cur_color=hsv(120.0, 1.0, (rand()%100 + 100)/200.0);
			if (on_wall(pos(r,c)))
				cur_color = hex(0x663300);
			draw_glyph_xy(V(c+0.5*r, r*(1.0-hex_side/4.0)), ALIGN_CENTER, 156);
		}
	}
	end_background_draw();
	
	
}


void level_ready(void)
{
	game_render();
	cur_color = rgb(1.0, 1.0, 0.9);
	translate(16,13);
	scale(3,3);
	draw_string_xy(V0, ALIGN_CENTER, "Level %d\nReady...", cur_level+1);
	if (cur_state == PRESS) { // wait for a click to start the next game
		if (cur_xy.x > 30 && cur_xy.y > 16) {
			do_fullscreen();
		} else {
			state = run_wait;
		}
	}
}

void run_step(void)
{
	// turn to a new direction
	if (body_should_turn) {
		body_dir = ((body_dir+body_should_turn)+6)%6;
		body_should_turn = 0;
	}
	// move to the next spot
	Pos head = body[body_head];
	switch(body_dir){
		case 0: case 3:  head.c = head.c - ((body_dir&0x2) - 1); head.type = 8; break;
		case 1: case 4:  head.r = head.r - (body_dir/2 - 1); head.type = 9; break;
		case 2:  head.r += 1; head.c -= 1; head.type = 10; break;
		case 5:  head.r -= 1; head.c += 1; head.type = 10; break;
	}
	// are we dead?
	if(!on_board(head) || on_body(head) || on_wall(head)) {
		state = game_over;
		return;
	}
	// are we growing?
	if (grow_more_body)
		grow_body();
	// move the head
	body_head = (body_head+1)%body_len;
	body[body_head] = head;
	// did we get the apple?
	if (pos_eq(head, apple)) {
		if (++apples_eaten == 10) {
			if (!((cur_level+1)%4)) {
				speed /= 1.25;
			}
			state = new_game;
			return;
		}
		new_apple();
		grow_more_body += 3;
	}
	state = run_wait;
	// run_wait will render the scene for us
	state();
}

void run_wait(void)
{
	if (cur_state == PRESS) {
		if (cur_xy.x > 16) {
			body_should_turn = -1;
			body[body_head].type = body_dir;
		} else {
			body_should_turn = 1;
			body[body_head].type = (body_dir+4)%6;
		}
	}
	if (cur_time() - prev_time > speed) {
		prev_time = cur_time();
		state = run_step;
	}
	game_render();
}


void new_game(void)
{
	reset_body();
	++ cur_level;
	draw_level();
	new_apple();
	state=level_ready;	
}


void init()
{
	cur_level = -1;
	size = 15;
	halfsize = size/2;
	state = new_game;
}


void render()
{
	state();
}

