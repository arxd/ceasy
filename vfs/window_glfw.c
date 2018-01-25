#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//~ #include <string.h>
//~ #include <stdarg.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
//~ #include <unistd.h>
//~ #include <sys/mman.h>
//~ #include <sys/ipc.h>
//~ #include <sys/shm.h>
//~ #include <sys/stat.h> 
//~ #include <signal.h>
//~ #include <sys/wait.h>

//~ #include "gfx.c"

//~ #define BGW (1024)
//~ #define BGH (576)


#include "window.h"

static GLFWwindow *window = 0;
static GLFWwindow * w_window = 0;
static GLFWwindow * fs_window = 0;

int g_win_width, g_win_height;

int win_should_close(void)
{
	return glfwWindowShouldClose(window);
}

void win_size(int *w, int *h)
{
	*w = g_win_width;
	*h = g_win_height;
}

double win_time(void)
{
	return glfwGetTime();
}

void win_update(void)
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

static void win_death(const char *title, const char *err_str, int errid)
{	
	if (title)
		printf("%s:%d: %s\n", title, errid, err_str);
	win_fini();
	exit(errid);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	float monitor_aspect = (16.0/9.0);
	if (fabs((double)width /height - monitor_aspect) > 0.01) {
		if ((double) width/height > monitor_aspect)
			width = height*monitor_aspect;
		else
			height = width/monitor_aspect;
		if (width < 256) {
			width = 256;
			height = 144;
		}
		glfwSetWindowSize(window, width, height);
	} else {
		g_win_width = width;
		g_win_height = height;
		glViewport(0, 0, width, height);
	}
}

static void set_window(GLFWwindow *w)
{
	window = w;
	glfwMakeContextCurrent(window);
	//~ init_gl();
	//~ glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//~ glfwSetKeyCallback(window, key_callback);
	//~ glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	framebuffer_size_callback(window, width, height);
}


static void error_callback(int error, const char* description)
{
	win_death("GLFW Error", description, error);
}

void win_init(EventCallback* callback)
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		win_death("GLFW", "init", -1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	printf("MODE: %d,%d  %f %d\n", mode->width, mode->height, (double)mode->width/(double)mode->height, mode->refreshRate);
	w_window = glfwCreateWindow(256, 144, "Glyphy Graphics", NULL, NULL);
	if (!w_window)
		win_death("GLFW", "window create", -1);
	set_window(w_window);
	printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
	printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );
}

void win_fini(void)
{
	if (fs_window)
		glfwDestroyWindow(fs_window);
	if (w_window)
		glfwDestroyWindow(w_window);
	glfwTerminate();
	
}
