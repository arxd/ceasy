#ifndef WINDOW_GLFW_C
#define WINDOW_GLFW_C

typedef void EventCallback(int type, float ts, int p1, int p2);

void win_size(int *w, int *h);
void win_update();
double time();
int win_init(int w, int h, EventCallback* callback);
void win_on_exit(int status, void *arg);

#if __INCLUDE_LEVEL__ == 0

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "util.c"
#include "iomem.h"

extern Input *input;
void gl_context_change(void);

typedef struct s_Window Window;
struct s_Window {
	GLFWwindow *window;
	GLFWwindow *w_window;
	GLFWwindow *fs_window;
	EventCallback *callback;
	int should_close;
	int w, h;
	int cx, cy;
	char input_chars[16];
	int input_i;
	int fs;
	
	GLuint fbtex;
	GLuint fbobj;
};

Window gw;

double time(void)
{
	return glfwGetTime ();
}



void win_size(int *w, int *h)
{
	*w = gw.w;
	*h = gw.h;
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
	if (gw.input_i)
		input->getchar = gw.input_chars[--gw.input_i];
	if (glfwWindowShouldClose(gw.window))
		input->status |= STATUS_CLOSE;
	
	//~ int x,y;
	//~ static int f = 0;
	//~ ++f;
	//~ glfwGetWindowPos	(gw.window, &x, &y);
	//~ if(! (f%30))
		//~ printf("%d, %d\n", x, y);

}


void set_window(GLFWwindow *w);

GLFWmonitor* cur_monitor(void)
{
	int count, xpos, ypos;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	GLFWmonitor *monitor =glfwGetPrimaryMonitor() ;
	glfwGetWindowPos(gw.window, &xpos, &ypos);
	for (int m=0; m < count; ++m) {
		int mx, my;
		glfwGetMonitorPos(monitors[m], &mx, &my);
		const GLFWvidmode* mode = glfwGetVideoMode(monitors[m]);
		DEBUG("MONITOR %s: %dx%d (%d, %d) %d", glfwGetMonitorName(monitors[m]), 
			mode->width,mode->height, mx,my, mode->refreshRate);
		int nmodes;
		const GLFWvidmode* modes = glfwGetVideoModes(monitors[m], &nmodes);
		for (int d=0; d < nmodes; ++d)
			DEBUG ("\t%dx%d %d", modes[d].width, modes[d].height, modes[d].refreshRate);
		
		if (xpos > mx && ypos > my && xpos < mx+mode->width && ypos < my+mode->height)
			monitor = monitors[m];
	}
	return monitor;
}

void do_fullscreen2(void)
{
	static int xpos, ypos, width, height;

	if (gw.fs) {
		glfwSetWindowMonitor(gw.window, NULL, xpos, ypos, width, height, 0);
	} else {
		glfwGetWindowPos(gw.window, &xpos, &ypos);
		glfwGetWindowSize(gw.window, &width, &height);
		// figure out which monitor we are on
		GLFWmonitor *monitor = cur_monitor();
		
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		DEBUG("FS ON %s: %dx%d", glfwGetMonitorName(monitor), mode->width, mode->height);
		glfwSetWindowMonitor(gw.window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	gw.fs = !gw.fs;
	
}

void do_fullscreen(void)
{
	if (!gw.fs_window) { // we need to make a fullscreen window
		GLFWmonitor *monitor =cur_monitor();
		int count;
		//~ GLFWmonitor** monitors = glfwGetMonitors(&count);
		//~ for (int m=0; m < count; ++m)
			//~ DEBUG("MONITOR %d: %s ", m, glfwGetMonitorName(monitors[m]));
		
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate); 
		DEBUG("Make fs window %dx%d %d", mode->width, mode->height, mode->refreshRate);
		gw.fs_window = glfwCreateWindow(mode->width, mode->height, "Glyphy FS", monitor, gw.w_window);
		if (!gw.fs_window) {
			fprintf(stderr, "Failed to create a fs window!\n");
			return;
		}
	}
	if (gw.fs) {
		DEBUG("go Windowed");
		set_window(gw.w_window);
		glfwDestroyWindow(gw.fs_window);
		gw.fs_window = 0;
	} else {
		DEBUG("go FS");
		set_window(gw.fs_window);
		//glfwHideWindow(gw.w_window);
	}
	glfwShowWindow(gw.window);
	
	gw.fs = !gw.fs;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int x = xpos*256.0/gw.w;
	int y = ypos*144.0/gw.h;
	x = (x<0)?0 : (x >255?255:x);
	y = (y<0)?0 : (y >143?143:y);
	
	input->hoverX = x;
	input->hoverY = y;
	input->touch[0].x = x;
	input->touch[0].y = y;
}
void cursor_enter_callback(GLFWwindow* window, int entered)
{
	if (!entered) {
		input->hoverX = 255;
		input->hoverY = 255;
	}
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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (!input->touch[0].buttons) { //first press
			input->touch[0].x0 = input->touch[0].x;
			input->touch[0].y0 = input->touch[0].y;
		}
	} 
	if (action == GLFW_PRESS)
		input->touch[0].buttons |= (1<<button);
	else
		input->touch[0].buttons &= ~(1<<button);
	
}
void character_callback(GLFWwindow* window, unsigned int codepoint)
{	
	if (codepoint > 0 && codepoint <='~' && gw.input_i < 16)
		gw.input_chars[gw.input_i++] = codepoint;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods |GLFW_MOD_ALT ) {
		do_fullscreen();
		return;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		input->status |= STATUS_CLOSE;
		return;
	}
	

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && gw.input_i < 16) {
		switch (key) {
			case GLFW_KEY_ENTER: gw.input_chars[gw.input_i++] = '\n'; break;
			case GLFW_KEY_TAB: gw.input_chars[gw.input_i++] = '\t'; break;
			case GLFW_KEY_BACKSPACE: gw.input_chars[gw.input_i++] = '\b'; break;
			default:break;
		}
	}
	uint32_t *bitfield = 0;
	int bit = 0;
	if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
		bit = key - GLFW_KEY_A;
		bitfield = &input->alpha;
	} else if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F16) {
		bit = key - GLFW_KEY_F1;
		bitfield = &input->function;
	} else if (key >= GLFW_KEY_0   && key <= GLFW_KEY_9) {
		bit = key - GLFW_KEY_0;
		bitfield = &input->num;
	} else if (key >= GLFW_KEY_KP_0   && key <= GLFW_KEY_KP_9) {
		bit = key - GLFW_KEY_KP_0 + 16;
		bitfield = &input->num;
	}
	if (bitfield) {
		if (action == GLFW_PRESS)
			*bitfield |= 1<<bit;
		else if (action == GLFW_RELEASE)
			*bitfield &= ~(1<<bit);
	}	
}


void set_window(GLFWwindow *w)
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
	glfwSetKeyCallback(gw.window, key_callback);
	glfwSetCharCallback(gw.window, character_callback);
	glfwSetCursorPosCallback(gw.window, cursor_position_callback);
	glfwSetCursorEnterCallback(gw.window, cursor_enter_callback);
	glfwSetMouseButtonCallback(gw.window, mouse_button_callback);

	int width, height;
	glfwGetFramebufferSize(gw.window, &width, &height);
	framebuffer_size_callback(gw.window, width, height);
	gl_context_change();
	
	// set up a framebuffer for this new context
	//~ if (gw.fbobj)
		//~ ABORT(9, "fbobj should have been released");
	//~ glGenFramebuffers(1, &gw.fbobj);
	//~ glBindFramebuffer(GL_FRAMEBUFFER, gw.fbobj);
	//~ glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gw.fbtex, 0);
	//~ if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//~ fprintf(stderr, "Couldn't get framebuffer (%d) ready! %d %d %d %d\n", gw.fbobj, glCheckFramebufferStatus(GL_FRAMEBUFFER),
		//~ GL_FRAMEBUFFER_COMPLETE, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
	//~ glBindFramebuffer(GL_FRAMEBUFFER, 0); // put the default framebuffer back
}

static void error_callback(int error, const char* description)
{
	ABORT(100, "GLFW:%d: %s", error, description);
}

void win_fini(void)
{
	//~ if (gw.fbtex) {
		//~ DEBUG("Delete FB Texture");
		//~ glDeleteTextures(1, &gw.fbtex);
	//~ }
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
	// create a texture for our framebuffer
	//~ glfwMakeContextCurrent(gw.w_window);
	//~ glGenTextures(1, &gw.fbtex);
	//~ DEBUG("MAKE TEX %d", gw.fbtex);
	//~ glBindTexture(GL_TEXTURE_2D, gw.fbtex);
	//~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	set_window(gw.w_window);
	glfwSetTime (0.0);
	DEBUG("GL_VERSION  : %s", glGetString(GL_VERSION) );
	DEBUG("GL_RENDERER : %s", glGetString(GL_RENDERER) );
	

	
	
	return 1;
}

#endif
#endif
