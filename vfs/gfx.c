#ifndef GFX_H
#define GFX_H
#include "share.c"

enum {GFX_SYNC=1, GFX_FRAME, GFX_POINTS, GFX_COLORS, GFX_PTS} GFXTypes;

typedef union s_Color Color;
typedef union s_PointXY PointXY;
typedef struct s_RLEColor RLEColor;

typedef struct s_Bitmap Bitmap;
typedef struct s_Points Points;

struct s_Bitmap {
	int w, h;
	unsigned char *px;
	int x0, y0;
	float rotation;
};

union s_Color {
	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	unsigned int rgba;
};

union s_PointXY {
	float xy[2];
	struct {
		float x;
		float y;
	};
};
struct s_Points {
	unsigned long npts;
	PointXY pts[];
};


struct s_RLEColor {
	Color rgba;
	unsigned int count;
};
struct s_Colors {
	Color red, green, blue, yellow, black, white, gray;	
};

extern struct s_Colors Colors;
extern Color cur_color;

int graphics_connect(const char *id);
Color gfx_rgba(float r, float g, float b, float a);
Color gfx_rgb(float r, float g, float b);
void gfx_triangles(int npts, ...);

int gfx_sync(void);



#if __INCLUDE_LEVEL__ == 0

//~ #include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//~ #include <unistd.h>
//~ #include <sys/shm.h>

Color cur_color = {55, 93,100,0};
struct s_Colors Colors = {
	{128, 0, 0, 0},
	{0, 128, 0, 0},
	{0, 0, 128, 0},
	{192, 192, 0, 0},
	{0, 0, 0, 0},
	{255,255,255,0},
	{128,128,128,0}};
	
int graphics_connect(const char *id)
{
	char * endptr;
	long int shmid = strtol(id, &endptr,16);
	
	return ipc_connect(shmid, 0);;
}


Color gfx_rgba(float r, float g, float b, float a)
{
	Color rgba;
	rgba.r = (unsigned char)((r < 0)? 0 : ((r >= 1.0)?255 : (255*r+0.5)));
	rgba.g = (unsigned char)((g < 0)? 0 : ((g >= 1.0)?255 : (255*g+0.5)));
	rgba.b = (unsigned char)((b < 0)? 0 : ((b >= 1.0)?255 : (255*b+0.5)));
	rgba.a = (unsigned char)((a < 0)? 0 : ((a >= 1.0)?255 : (255*a+0.5)));
	return rgba;
}

Color gfx_rgb(float r, float g, float b)
{
	return gfx_rgba(r,g,b,0.0);
}

void gfx_triangles(int npts, ...)
{
	Points *tris = alloca(sizeof(Points) + sizeof(PointXY)*(npts));
	tris->npts = npts;
	va_list args;                     
	va_start(args, npts);
	for(int i=0; i < npts; ++i) {
		tris->pts[i].x = (float)va_arg(args, double);
		tris->pts[i].y = (float)va_arg(args, double);
	}
	va_end(args);
	ipc_send(sizeof(Points) + sizeof(PointXY)*npts, GFX_PTS, (u64*)tris);
	
}

int gfx_sync(void)
{
	int frame;
	ipc_send(0, GFX_SYNC, 0);
	Package *pkg = ipc_recv_block();
	frame = (int) *((unsigned long*)pkg->pdata);
	ipc_free_pkg(pkg);
	return frame;
}


#endif
#endif
