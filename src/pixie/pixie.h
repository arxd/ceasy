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

#endif

