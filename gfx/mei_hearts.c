

#include "engine.c"

void init(void)
{
	
}

void render(void)
{
	Color pink = hex(0xE149B2);
	Color cream = hex(0xE1D8C7);
	//cream.alpha = 0.5;
	set_color(cream);
	
	translate(16,9);
	scale(2.0, 2.0);
	
	int i;
	for (i=0; i < 18; i++ ) {
		push_matrix();
		rotate(360.0/18.0 * i + 4.0*cur_time());
		translate(5.0, 0.0);
		rotate(-90);
		scale(5.0*sin(cur_time()), 5.0*sin(cur_time()));
		draw_glyph_xy(V0, ALIGN_CENTER, 18);
		pop_matrix();
	}
	scale(3.0, 3.0);
	cur_color = pink;
	draw_string_xy(V0, ALIGN_CENTER, "Mei");
}
