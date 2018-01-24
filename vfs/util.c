#ifndef UTIL_C
#define UTIL_C

#include <stdio.h>
#include <stdlib.h>

void death(const char *title, const char *err_str, int errid);

#if __INCLUDE_LEVEL__ == 0

void death(const char *title, const char *err_str, int errid)
{	
	if (title)
		printf("%s:%d: %s\n", title, errid, err_str);
	//~ if (fs_window)
		//~ glfwDestroyWindow(fs_window);
	//~ if (w_window)
		//~ glfwDestroyWindow(w_window);
	//~ glfwTerminate();
	exit(errid);

}



#endif
#endif
