#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#include "iomem.h"

extern IOMem *io;
extern uint8_t *vram;
extern Layer *layers;
extern volatile Input *input;

void io_init(const char *shmid_str);
void printf_xy(int x, int y, const char *fmt, ...);
void frame_sync_interrupt(int frame) __attribute__((weak));
double now(void);
void tiles_init_load32(Tiles *self, int addr, int w, int h, int b, int num, uint32_t *data);
void tiles_init_load16(Tiles *self, int addr, int w, int h, int b, int num, uint16_t *data);
void tiles_init_load8(Tiles *self, int addr, int w, int h, int b, int num, uint8_t *data);
void tiles_init(Tiles *self, int addr, int w, int h, int b);
void palette_init(int addr, int num, uint32_t *data);
void map_init(Map *self, int addr, int w, int h, uint8_t *data);

#endif

