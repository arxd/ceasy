#include "engine.c"

int glyphs_width = 16;
int g_cur;

void init(void)
{
	g_cur = 158;
	start_background_draw();
	set_color(hex(0xE1D8C7));

	translate(0.5, 0.0);
	for (int i = 0; i < num_glyphs; ++i) 
		draw_glyph_xy(V(i%glyphs_width, (i/glyphs_width)*1.5), ALIGN_LEFT, i);
	end_background_draw();
}

void render(void)
{
	draw_string_xy(V(32-4, 1), ALIGN_LEFT, "%d", g_cur);
	
	translate(0.5, 0.0);
	if (cur_state == PRESS) {
		Vec xy = vtrans_inv(cur_xy);
		g_cur = (int)(xy.y/1.5)*glyphs_width + (int)xy.x;
	}
	
	translate(19, 4);
	scale(10, 10);
	cur_color.alpha = 0.2;
	draw_glyph_xy(V0, ALIGN_LEFT, 0);
	cur_color.alpha = 1.0;
	draw_glyph_xy(V0, ALIGN_LEFT, g_cur);
	
}
