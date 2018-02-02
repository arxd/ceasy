#ifndef VEC_C
#define VEC_C

#define V0 ((Vec2){0.0, 0.0})

typedef struct s_Vec2 Vec2;
struct s_Vec2 {
	float x, y;
};

Vec2 v2(float x, float y);
Vec2 v2add(Vec2 v0, Vec2 v1);
Vec2 v2mult(Vec2 v0, float alpha);
Vec2 v2rot(Vec2 v0, float degrees);
Vec2 v2reflect(Vec2 v0, Vec2 normal);
float v2dot(Vec2 v0, Vec2 v1);

const char *v2str(Vec2 v0);

#if __INCLUDE_LEVEL__ == 0

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "pixie.h"

Vec2 v0(void)
{
	return (Vec2){0.0, 0.0};
}

Vec2 v2(float x, float y)
{
	return (Vec2){x,y};
}

Vec2 v2add(Vec2 v0, Vec2 v1)
{
	return (Vec2) {
		.x = v0.x + v1.x,
		.y = v0.y + v1.y,
	};
}

Vec2 v2mult(Vec2 v0, float alpha)
{
	return (Vec2) {alpha*v0.x, alpha*v0.y};
}

Vec2 v2rot(Vec2 v0, float degrees)
{
	float c = cos(M_PI*degrees/180.0);
	float s = sin(M_PI*degrees/180.0);
	return (Vec2) {
		.x = v0.x*c - v0.y*s,
		.y = v0.x*s + v0.y*c,
	};
}

Vec2 v2reflect(Vec2 v0, Vec2 normal)
{
	return v2add(v0, v2mult( normal , -2.0*v2dot(v0, normal)));
}

float v2dot(Vec2 v0, Vec2 v1)
{
	return v0.x*v1.x + v0.y*v1.y;
}


const char *v2str(Vec2 v0)
{
	static char buffer[32];
	snprintf(buffer, 32, "<%.1f, %.1f>", v0.x, v0.y);
	return buffer;
}

#endif
#endif
