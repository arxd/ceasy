#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define STATUS_CLOSE (1)


#define K_ENTER 1
#define K_UP
#define K_DOWN
#define K_LEFT
#define K_RIGHT

typedef struct s_Input Input;
struct s_Input {
	uint32_t frame;
	uint8_t status;
	uint8_t getchar;
	uint8_t hoverX;
	uint8_t hoverY;
	
	// inputs
	struct {
		uint8_t buttons;
		uint8_t mod;
		uint8_t x;
		uint8_t y;
		uint8_t x0;
		uint8_t y0;
	} touch[4];
	
	struct {
		int8_t ain[4];
		uint8_t buttons[2];
	} controllers[4];
	
	uint32_t alpha;
	uint32_t num;
	uint32_t function;
	uint32_t cmd;
	uint32_t mod;
};

#endif
