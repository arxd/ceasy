#include "engine.c"

// This must be defiend.  
// It gets called once befoe render is ever called.
// It is a good place to draw your background and set up state
void init(void)
{
	start_background_draw(); // this indicates that all drawing will go to the background (not to the screen)
	translate(10,5);
	scale(3, 1);
	// drawing a grid is good for seeing what your transforms are doing
	// The checks are 1x1 unit squares
	// black: (+,+) quadrant
	// blue : (-, -) quadrant
	// red : (+,-) quadrant
	// green : (-, +) quadrant
	draw_grid();
	end_background_draw(); // now we are done drawing to the background and further draws will go to the screen.
}


void example_rotate_translate(void)
{
	// the origin starts in the lower left corner
	// move right 6 and up 4
	translate(6,4); 
	// this pushes the current transform on the matrix stack (for undoing later);
	push_matrix();
	// Then rotate (in degrees CCW)  
	cur_color = rgb(1,1,1);
	draw_glyph_xy(V0, ALIGN_CENTER, 15);
	// move some more to the right
	translate(3, 0); 
	// Then rotate (in degrees CCW)  
	rotate(cur_time()*20); 
	cur_color = rgb(0.0, 1.0, 0.0);
	draw_glyph_xy(V0, ALIGN_LEFT, 3); // Die #3 spins around the (9,4) origin
	// This reverts to the previously pushed matrix state
	// It gets rid of the translate(3,0) and rotate.
	pop_matrix();
	// We are now back at (6,4)
	// This time we rotate first and then translate.
	rotate(cur_time()*20);
	translate(3, 0);
	cur_color = rgb(1.0, 0.0, 0.0);
	draw_glyph_xy(V0, ALIGN_LEFT, 6); // Die #1 is drawn at the new origin (6,4)
}

void example_shear(void)
{
	translate(3, 15);
	scale(2,2);
	push_matrix();
	// This is another way to visualize the current matrix transformation
	// If the blue Z arrow is going CCW then you are in a mirror ( you scaled by a negative)
	scale(1.0, 0.5);
	skewX(sin(cur_time()) * 45); // in degrees
	cur_color = rgba(0,0,0, 0.7);
	draw_glyph_xy(V0, ALIGN_MID, 10);
	pop_matrix();
	cur_color = rgb(1.0, 1.0, 1.0);
	draw_glyph_xy(V0, ALIGN_MID, 10);
	draw_origin();
	
	translate(3,0);
	scale(-1, 1);
	skewY(sin(cur_time()) * 45); // in degrees
	cur_color = rgba(0,0,0, 0.7);
	draw_glyph_xy(V0, ALIGN_MID, 10);
	draw_origin();

}


void example_vector(void)
{
	// There is a vector type Vec  that holds a float x and float y;
	Vec a = V(-1.0, 3.14);
	
	translate(4, 9);
	cur_color = rgb(1.0, 1.0, 1.0);
	draw_glyph_xy(V0, ALIGN_LEFT, 0);
	cur_color = rgb(1.0, 0.0, 0.0);
	draw_vec(V0, V(1.0, 0.0));
	
	
	
	
}


// This must be defined
// It is called about 60 times per second (or whatever your monitor refresh rate is)
// This is where you draw the rest of the scene on top of the background
// The background is automatically drawn to the screen for you before render() is called.
// The current matrix transform is reset before render() is called.  Origin at lower left, Screen width 32, height 18
void render(void)
{
	void (*examples[])(void) = {example_rotate_translate, example_shear, example_vector};
	int i;
	for (i =0; i < 3; ++i) {
		push_matrix();
		examples[i]();
		pop_matrix();
	}
}


