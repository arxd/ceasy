
const int COLOR_STACK_MAX = 64;

typedef union u_Color Color;
union u_Color {
	GLfloat rgba[4];
	struct {
		GLfloat rgb[3];
		GLfloat alpha;
	};
	struct {
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat a;
	};
};

Color cur_color_stack[COLOR_STACK_MAX];
int cur_color_stack_top;
Color cur_color;

float clamp(float x)
{
	return  x > 1.0 ? 1.0 : (x < 0.0? 0.0: x);
}

Color hex(unsigned int rgb)
{
	Color clr;
	clr.r = ((rgb >> 16)&0xFF)/255.0;
	clr.g = ((rgb >> 8)&0xFF)/255.0;
	clr.b = ((rgb)&0xFF)/255.0;
	clr.a = 1.0;
	return clr;
}

Color rgb(float r, float g, float b)
{
	Color clr;
	clr.r = clamp(r);
	clr.g = clamp(g);
	clr.b = clamp(b);
	clr.a = 1.0;
	return clr;
}
Color rgba(float r, float g, float b, float a)
{
	Color clr;
	clr.r = clamp(r);
	clr.g = clamp(g);
	clr.b = clamp(b);
	clr.a = clamp(a);
	return clr;
}

Color hsv(float h /* 0 - 360 */, float s, float v)
{
	while (h < 0.0)
		h += 360.0;
	while (h >= 360.0)
		h -= 360.0;
	s = clamp(s);
	v = clamp(v);
	
	double c = v*s;
	double x = c*(1.0-fabs(fmod((h/60.0), 2.0) - 1.0));
	double m = v-c;
	if (h < 60.0) 
		return rgb(c, x, 0.0);
	else if (h < 120.0)
		return rgb(x, c, 0.0);
	else if (h < 180.0)
		return rgb(0, c, x);
	else if (h < 240.0)
		return rgb(0, x, c);
	else if (h < 300.)
		return rgb(x, 0, c);
	return rgb(c, 0, x);
	
}

Color rand_color(void)
{
	return hsv(rand()%360, 0.7, 1.0);
}

/** Multiply rgb values by amount.
If amount > 1.0 then it is lighter.
if amount < 1.0, then it is darker
*/
Color dodge(Color c, float amount)
{
	c.r = clamp(c.r * amount);
	c.g = clamp(c.g * amount);
	c.b = clamp(c.b * amount);
	return c;
}
void set_color(Color c)
{
	cur_color = c;
}

Color color_dodge(float amount)
{
	set_color(dodge(cur_color, amount));
	return cur_color;
}



void push_color(void)
{
	if (cur_color_stack_top == COLOR_STACK_MAX)
		death("Overflow", "Too many color pushes!\n", -1);
	cur_color_stack[cur_color_stack_top++] = cur_color;
}

void pop_color(void)
{
	if (!cur_color_stack_top) 
		death("Underflow", "Popped more than you pushed!\n", -1);
	cur_color = cur_color_stack[--cur_color_stack_top];
}


