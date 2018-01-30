#ifndef WINDOW_GLFW_C
#define WINDOW_GLFW_C


typedef void EventCallback(int type, float ts, int p1, int p2);

int win_should_close();
void win_size(int *w, int *h);
void win_update();

int win_init(int w, int h, EventCallback* callback);
void win_fini();
void win_on_exit(int status, void *arg);
void win_hover(int *x, int *y);

#if __INCLUDE_LEVEL__ == 0

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "util.c"

typedef struct s_Window Window;
struct s_Window {
	GLFWwindow *window;
	GLFWwindow *w_window;
	GLFWwindow *fs_window;
	EventCallback *callback;
	int should_close;
	int w, h;
	int cx, cy;
	
};

Window gw;

int win_should_close(void)
{
	return glfwWindowShouldClose(gw.window);
}

void win_size(int *w, int *h)
{
	*w = gw.w;
	*h = gw.h;
	//~ glfwGetFramebufferSize(gw.window, w, h);
}
void win_hover(int *x, int *y)
{
	*x = gw.cx;
	*y = gw.cy;
	//~ glfwGetFramebufferSize(gw.window, w, h);
}

double win_time(void)
{
	return glfwGetTime();
}

void win_update(void)
{
	glfwSwapBuffers(gw.window);
	glfwPollEvents();
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int x = xpos*256.0/gw.w;
	int y = ypos*144.0/gw.h;
	//~ printf("Mouse Move %.1f %.1f (%d, %d) %d %d\n", xpos, ypos, x, y,gw.w, gw.h);
	gw.cx = x;
	gw.cy = y;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	float monitor_aspect = (16.0/9.0);
	if (fabs((double)width /height - monitor_aspect) > 0.01) {
		//~ printf("BAD");
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
		gw.w = width;
		gw.h = height;
		glViewport(0, 0, width, height);
	}
}

static void set_window(GLFWwindow *w)
{
	gw.window = w;
	glfwMakeContextCurrent(gw.window);
	//~ init_gl();
	//~ glfwSwapInterval(0);
	//~ glfwSetWindowAspectRatio(gw.window, 16, 9);
	//~ glfwSetWindowSizeLimits(gw.window, 256, 144, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetFramebufferSizeCallback(gw.window, framebuffer_size_callback);
	//~ glfwSetKeyCallback(window, key_callback);
	//~ glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glfwSetCursorPosCallback(gw.window, cursor_position_callback);
	
	int width, height;
	glfwGetFramebufferSize(gw.window, &width, &height);
	framebuffer_size_callback(gw.window, width, height);
}


static void error_callback(int error, const char* description)
{
	ABORT(100, "GLFW:%d: %s", error, description);
}

void win_fini(void)
{
	if (gw.fs_window) {
		DEBUG("Destroy FS window");
		glfwDestroyWindow(gw.fs_window);
	}
	if (gw.w_window) {
		DEBUG("Destroy Window");
		glfwDestroyWindow(gw.w_window);
	}
	DEBUG("GLFW Terminate");
	glfwTerminate();
	memset(&gw, 0, sizeof(Window));
}

void win_on_exit(int status, void *arg)
{
	win_fini();
}

int win_init(int w, int h, EventCallback* callback)
{
	memset(&gw, 0, sizeof(Window));	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return 0;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	//~ const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//~ printf("MODE: %d,%d  %f %d\n", mode->width, mode->height, (double)mode->width/(double)mode->height, mode->refreshRate);
	gw.w_window = glfwCreateWindow(w, h, "Glyphy Graphics", NULL, NULL);
	if (!gw.w_window) {
		glfwTerminate();
		return 0;
	}
	set_window(gw.w_window);
	DEBUG("GL_VERSION  : %s", glGetString(GL_VERSION) );
	DEBUG("GL_RENDERER : %s", glGetString(GL_RENDERER) );
	return 1;
}

#endif
#endif
