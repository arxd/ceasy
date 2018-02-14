#ifndef CLIENT_C
#define CLIENT_C

#ifndef IOMEM_H
#include "iomem.h"
#endif

typedef void FrameSyncHandler(int);

extern IOMem *io;
extern volatile Input *input;

void io_init(const char *shmid_str);
void on_frame_sync(FrameSyncHandler *handler);
double now(void);

#if __INCLUDE_LEVEL__ == 0 || defined(NOLIB)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>


#ifndef LOGGING_C
#include "logging.c"
#endif

IOMem *io;
volatile Input *input;

double now(void)
{
	static struct timespec t0 = {0xFFFFFFFF, 0xFFFFFFFF};
	if (t0.tv_nsec == 0xFFFFFFFF) 
		clock_gettime(CLOCK_REALTIME, &t0);
	struct timespec t1;
	clock_gettime(CLOCK_REALTIME, &t1);
	return (t1.tv_sec - t0.tv_sec) + 1.0e-9*( t1.tv_nsec - t0.tv_nsec);
}

FrameSyncHandler *g_fs_handler;

void sig_handler(int sig) {
	static int frame = 0;
	if (g_fs_handler)
		g_fs_handler(frame++);
}

void on_frame_sync(FrameSyncHandler *handler)
{
	g_fs_handler = handler;
}


void io_init(const char *shmid_str)
{
	char * endptr;
	int shmid = (int)strtol(shmid_str, &endptr, 16);
	ASSERT (shmid_str != endptr, "ROM Must be run as an argument to the server");
	io = (IOMem*)shmat(shmid, NULL, 0);
	ASSERT ((void*)io != (void*)-1, "Couldn't attach shared memory");

	input = &io->input;
	
	signal(SIGUSR1, sig_handler);
}

#endif
#endif

