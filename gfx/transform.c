
typedef union s_Transform Transform;
union s_Transform {
	struct {
		GLfloat scale[4];
		GLfloat translate[2];
	};
	struct {
		GLfloat a, d, b, e, c, f;
	};
};

typedef union u_Vec Vec;
union u_Vec {
	GLfloat xy[2];
	struct {
		GLfloat x;
		GLfloat y;
	};
};

const int TRANSFORM_STACK_MAX = 64;

Transform g_transform_stack[TRANSFORM_STACK_MAX];
int g_transform_stack_top;
Transform g_mat;
Vec V0 = {0.0, 0.0};

Vec V(GLfloat x, GLfloat y)
{
	Vec v;
	v.x = x;
	v.y = y;
	return v;
}

Vec vadd(Vec v1, Vec v2)
{
	Vec v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	return v;
}

Vec vsub(Vec v1, Vec v2)
{
	Vec v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	return v;
}

Vec vmult(Vec v1, GLfloat c)
{
	Vec v;
	v.x = v1.x * c;
	v.y = v1.y * c;
	return v;
}

Vec vrot90(Vec v1)
{
	Vec v;
	v.x = -v1.y;
	v.y = v1.x;
	return v;
}

Vec vrot180(Vec v1)
{
	Vec v;
	v.x = -v1.x;
	v.y = -v1.y;
	return v;
}

Vec vrot270(Vec v1)
{
	Vec v;
	v.x = v1.y;
	v.y = -v1.x;
	return v;
}

GLfloat vdot(Vec v1, Vec v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

GLfloat vmag(Vec v1)
{
	return sqrt(v1.x*v1.x + v1.y*v1.y);
}

GLfloat vangle(Vec v1)
{
	if (v1.x > 0.0 && v1.y >= 0.0) // quad 1
		return atan(v1.y / v1.x);
	if (v1.x <= 0.0 && v1.y > 0.0) // quad 2
		return M_PI_2 - atan(v1.x / v1.y);
	if (v1.x < 0.0 && v1.y <= 0.0) // quad 3
		return M_PI + atan(v1.y / v1.x);
	if (v1.x >= 0.0 && v1.y < 0.0) //quad 4
		return M_PI + M_PI_2 - atan(v1.x / v1.y);
	// V0
	return 0.0;
}

Vec vnorm(Vec v1)
{
	Vec v;
	double mag = vmag(v1);
	v.x = v1.x / mag;
	v.y = v1.y / mag;
	return v;
}

Vec vtrans(Vec v1)
{
	Vec v;
	v.x = g_mat.a*v1.x + g_mat.b*v1.y + g_mat.c;
	v.y = g_mat.d*v1.x + g_mat.e*v1.y + g_mat.f;
	return v;
}

Vec vtrans_inv(Vec v1)
{
	Vec v;
	double denom = g_mat.b*g_mat.d - g_mat.a*g_mat.e;
	v.x = (g_mat.c*g_mat.e - g_mat.b*(g_mat.f - v1.y) - g_mat.e*v1.x) / denom;
	v.y = -(g_mat.c*g_mat.d - g_mat.a*(g_mat.f - v1.y) - g_mat.d*v1.x) /denom;
	return v;
	
}

void push_matrix(void)
{
	if (g_transform_stack_top == TRANSFORM_STACK_MAX)
		death("Transform", "Maximum 64 pushes exceeded!\n", -1);
	g_transform_stack[g_transform_stack_top++] = g_mat;
}

void pop_matrix(void)
{
	if (!g_transform_stack_top)
		death("Transform", "Popped more than you pushed!\n", -1);
	g_mat = g_transform_stack[--g_transform_stack_top]; 
}

void translate(GLfloat dx, GLfloat dy)
{
	/** Translation matrix 
	1  0  dx
	0  1  dy
	0  0  1
	*/
	g_mat.c = g_mat.a*dx + g_mat.b*dy + g_mat.c;
	g_mat.f = g_mat.d*dx + g_mat.e*dy + g_mat.f;
}

void translatev(Vec dxy)
{
	translate(dxy.x, dxy.y);
}


void scale(GLfloat sx, GLfloat sy)
{
	/** Scale Matrix
	sx  0  0
	0   sy 0
	0   0   1
	*/
	g_mat.a *= sx;
	g_mat.d *= sx;
	g_mat.b *= sy;
	g_mat.e *= sy;
}

void scalev(Vec sxy)
{
	scale(sxy.x, sxy.y);
}

void rotate(GLfloat degrees)
{
	/** Rotate Matrix
	cos   -sin   0
	sin    cos   0
	  0      0      1
	*/
	
	GLfloat c = cos(degrees*0.017453292519943295f);
	GLfloat s = sin(degrees*0.017453292519943295f);
	
	GLfloat a = g_mat.a*c + g_mat.b*s;
	g_mat.b = g_mat.a*-s + g_mat.b*c;
	g_mat.a = a;
	
	GLfloat d = g_mat.d*c + g_mat.e*s;
	g_mat.e = g_mat.d*-s + g_mat.e*c;
	g_mat.d = d;
}

void skewX(GLfloat degrees)
{
	/** SkewX Matrix
	1  tan  0
	0   1    0
	0   0    1
	*/
	GLfloat t = tan(degrees*0.017453292519943295f);
	g_mat.b = g_mat.a*t + g_mat.b;
	g_mat.e = g_mat.d*t + g_mat.e;
}


void skewY(GLfloat degrees)
{
	/** SkewX Matrix
	1     0  0
	tan  1  0
	0     0  1
	*/
	GLfloat t = tan(degrees*0.017453292519943295f);
	g_mat.a = g_mat.a + g_mat.b*t;
	g_mat.d = g_mat.d + g_mat.e*t;
}

void reset_matrix(void)
{
	if (g_transform_stack_top != 1)
		death("Transform", "Number of pushes and pops is not equal!\n", -1);
	g_mat.a = 1.0;
	g_mat.b = 0.0;
	g_mat.d = 0.0;
	g_mat.e = 1.0;
	
	g_mat.translate[0] = 0.0;
	g_mat.translate[1] = 0.0;
	g_transform_stack[0]  = g_mat;
}


void dump_transform(void)
{
	printf("%.4f  %.4f  %.4f\n", g_mat.a, g_mat.b, g_mat.c);
	printf("%.4f  %.4f  %.4f\n", g_mat.d, g_mat.e, g_mat.f);	
}
