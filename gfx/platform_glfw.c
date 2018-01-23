
double timer_start;
static GLFWwindow *window = 0;
static GLFWwindow * w_window = 0;
static GLFWwindow * fs_window = 0;
static double monitor_aspect = 16.0/9.0;

static void set_window(GLFWwindow *w);


void death(const char *title, const char *err_str, int errid)
{	
	if (title)
		printf("%s:%d: %s\n", title, errid, err_str);
	if (fs_window)
		glfwDestroyWindow(fs_window);
	if (w_window)
		glfwDestroyWindow(w_window);
	glfwTerminate();
	exit(errid);

}


void time_init(void)
{
	timer_start = glfwGetTime();
}

double cur_time(void)
{
	return glfwGetTime() - timer_start;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	fb_width = width;
	fb_height = height;
	if (fabs((double)width /height - monitor_aspect) > 0.01) {
		if ((double) width/height > monitor_aspect)
			glfwSetWindowSize(window, height*monitor_aspect, height);
		else
			glfwSetWindowSize(window, width, width/monitor_aspect);
		glfwPollEvents();
	}
}

static void error_callback(int error, const char* description)
{
	death("GLFW Error", description, error);
}

void do_fullscreen(void)
{
	if (fs_window) {
		set_window(w_window);
		glfwDestroyWindow(fs_window);
		glfwShowWindow(w_window);
		fs_window = 0;
	} else {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		fs_window = glfwCreateWindow(mode->width, mode->height, "Glyphy Graphics", monitor, w_window);
		if (fs_window) {
			//glfwHideWindow(w_window);
			set_window(fs_window);
		} else {
			death("GLFW", "No FS window", -1);
		}
	}	
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
		do_fullscreen();
	} else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		DEBUG ^= 1;
	}
}


static void set_window(GLFWwindow *w)
{
	window = w;
	glfwMakeContextCurrent(window);
	init_gl();
	glfwSwapInterval (1);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	
	int width, height;
	glfwGetFramebufferSize(window, &fb_width, &fb_height);
	//framebuffer_size_callback(window, width, height);
}



void init_platform()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		death("GLFW", "init", -1);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	bg_width = mode->width;
	bg_height = mode->height;
	monitor_aspect = 16.0/ 9.0;//(double)mode->width / (double)mode->height;
	w_window = glfwCreateWindow(1280, 720, "Glyphy Graphics", NULL, NULL);
	if (!w_window)
		death("GLFW", "window create", -1);
	set_window(w_window);
	cur_state = HOVER;
}

void render_loop(void)
{
	if(g_drawing_bg) {
		printf("WARNING: You forgot to call end_background_draw()\n");
		end_background_draw();
	}
	while (!glfwWindowShouldClose(window)) {
		// get mouse events
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			switch(cur_state) {
				case RELEASE:
				case HOVER: cur_state = PRESS; break;
				case HELD:
				case PRESS: cur_state = HELD; break;
			}	
		} else {
			switch(cur_state) {
				case RELEASE:
				case HOVER: cur_state = HOVER; break;
				case HELD:
				case PRESS: cur_state = RELEASE; break;
			}	
		}
		// get mouse position
		double xpos, ypos;
		int width, height;
		glfwGetCursorPos(window, &xpos, &ypos);
		//glfwGetFramebufferSize(window, &width, &height);
		ypos = fb_height - ypos;
		xpos = xpos * 32.0 / fb_width;
		ypos = (ypos) * 18.0 / fb_height;
		cur_xy = V(xpos, ypos);
		
		reset_gl();
		draw_background();
		render();		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char **argv)
{
	init_platform();
	init_objects();
	init_gl();
	time_init();
	init();	
        render_loop();
	death(NULL, NULL, 0);
	return 0;
}


