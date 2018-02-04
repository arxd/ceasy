#ifndef IOMEM_H
#define IOMEM_H

#define NUM_PLAYERS 4
#define NUM_GOALS 2
#define MAX_STARS 16

#include <stdint.h>

#ifndef INPUT_H
#include "input.h"
#endif

#ifndef VEC2_C
#include "vec2.c"
#endif

typedef struct s_Player Player;
struct s_Player {
	// These are set by the Client
	float set_thruster; // out: desired thruster angle
	float thrust; // out: amout of thrust to produce
	float set_grabber; // out: desired grabber angle
	float grab; // out: amout of grab to produce
	
	// These are set by the server
	float cur_thruster; // in: actual thruster angle (degrees)
	float cur_grabber; // in: actual grabber angle (degrees)
	Vec2 xy;  // in: current position
	Vec2 v;   // in: current velocity
	float r; // in: radius
	float m; // in: mass
	float energy; // in: energy
};

typedef struct s_Puck Puck;
struct s_Puck {
	Vec2 xy;  // in: current position
	Vec2 v;   // in: current velocity
	float r; // in: radius
	float m; // in: mass
};

typedef struct s_Goal Goal;
struct s_Goal {
	Vec2 xy;
	float r_goal; // radius of goal area
	float rmin; // radius of major hurt circle
	float rmax; // max bounds
};
typedef struct s_Circle Circle;
struct s_Circle {
	Vec2 xy;
	float r;
};

typedef struct s_Rink Rink;
struct s_Rink {
	float w, h;
	float r; // corner radius
	Goal goals[NUM_GOALS];
	int n_stars;
	Circle stars[MAX_STARS];
	
	//~ int n_powerup;
	//~ Circle powerups[MAX_POWERUPS];
};
	
	
typedef struct s_Team Team;
struct s_Team {
	Player player[NUM_PLAYERS];
};

typedef struct s_IOMem IOMem;
struct s_IOMem {
	uint32_t state;
	Team us;
	Team them;
	Puck puck;
	Input input;
	Rink rink;
};

#endif
