#ifndef UTIL_C
#define UTIL_C

#define ABORT(code, fmt, ...)   _abort(code, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

void _abort(int code, char *file, int line, const char *fmt, ...) __attribute__((noreturn));


#if __INCLUDE_LEVEL__ == 0
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void _abort(int code, char *file, int line, const char *fmt, ...)
{
	if (errno)
		perror("Errno: ");
	fprintf(stderr, "%s : %d\n", file, line);
	va_list args;
	va_start (args, fmt);
	vfprintf (stderr, fmt, args);	
	va_end (args);
	fprintf(stderr, "\n");
	fflush(stderr);
	exit(code);
}

#endif
#endif
