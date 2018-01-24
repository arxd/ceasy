#ifndef GFX_H
#define GFX_H
#include "share.c"

enum {GFX_SYNC=1, GFX_FRAME, GFX_POINTS, GFX_COLORS, GFX_PTS} GFXTypes;

typedef union s_Color Color;
typedef union s_PointXY PointXY;
typedef struct s_RLEColor RLEColor;

typedef struct s_Bitmap Bitmap;
typedef struct s_Points Points;

struct s_Bitmap {
	int w, h;
	unsigned char *px;
	int x0, y0;
	float rotation;
};

union s_Color {
	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	unsigned int rgba;
};

union s_PointXY {
	float xy[2];
	struct {
		float x;
		float y;
	};
};
struct s_Points {
	unsigned long npts;
	PointXY pts[];
};


struct s_RLEColor {
	Color rgba;
	unsigned int count;
};
struct s_Colors {
	Color red, green, blue, yellow, black, white, gray;	
};

extern struct s_Colors Colors;
extern Color cur_color;

int graphics_connect(const char *id);
Color gfx_rgba(float r, float g, float b, float a);
Color gfx_rgb(float r, float g, float b);
void gfx_triangles(int npts, ...);

int gfx_sync(void);



#if __INCLUDE_LEVEL__ == 0

//~ #include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//~ #include <unistd.h>
//~ #include <sys/shm.h>

Color cur_color = {55, 93,100,0};
struct s_Colors Colors = {
	{128, 0, 0, 0},
	{0, 128, 0, 0},
	{0, 0, 128, 0},
	{192, 192, 0, 0},
	{0, 0, 0, 0},
	{255,255,255,0},
	{128,128,128,0}};
	
int graphics_connect(const char *id)
{
	char * endptr;
	long int shmid = strtol(id, &endptr,16);
	
	return ipc_connect(shmid, 0);;
}


Color gfx_rgba(float r, float g, float b, float a)
{
	Color rgba;
	rgba.r = (unsigned char)((r < 0)? 0 : ((r >= 1.0)?255 : (255*r+0.5)));
	rgba.g = (unsigned char)((g < 0)? 0 : ((g >= 1.0)?255 : (255*g+0.5)));
	rgba.b = (unsigned char)((b < 0)? 0 : ((b >= 1.0)?255 : (255*b+0.5)));
	rgba.a = (unsigned char)((a < 0)? 0 : ((a >= 1.0)?255 : (255*a+0.5)));
	return rgba;
}

Color gfx_rgb(float r, float g, float b)
{
	return gfx_rgba(r,g,b,0.0);
}

void gfx_triangles(int npts, ...)
{
	Points *tris = alloca(sizeof(Points) + sizeof(PointXY)*(npts));
	tris->npts = npts;
	va_list args;                     
	va_start(args, npts);
	for(int i=0; i < npts; ++i) {
		tris->pts[i].x = (float)va_arg(args, double);
		tris->pts[i].y = (float)va_arg(args, double);
	}
	va_end(args);
	ipc_send(sizeof(Points) + sizeof(PointXY)*npts, GFX_PTS, (u64*)tris);
	
}

int gfx_sync(void)
{
	int frame;
	ipc_send(0, GFX_SYNC, 0);
	Package *pkg = ipc_recv_block();
	frame = (int) *((unsigned long*)pkg->pdata);
	ipc_free_pkg(pkg);
	return frame;
}


#endif
#endif







//~ void death(const char *title, const char *err_str, int errid);

double timer_start;
static GLFWwindow *window = 0;
static GLFWwindow * w_window = 0;
static GLFWwindow * fs_window = 0;
//~ static double monitor_aspect = 16.0/9.0;
//~ static pthread_t render_thread;
static void set_window(GLFWwindow *w);
int fb_width, fb_height;
//~ int DEBUG = 0;
int cur_state;

Shader* bg_shader;
Shader* tri_shader;
GLfloat bg_tex_coords[] = {0.0, 0.0,  0.0, 1.0, 1.0, 0.0,  1.0, 1.0};
GLuint fb_bg_id;
GLuint tex_bg_id;
//~ GLuint attr_aPos_bg;
//~ GLuint attr_aTex;

int bg_width, bg_height;
int fb_width, fb_height;

typedef enum{HOVER, PRESS, HELD, RELEASE} PressState;

#define AVG_SIZE 60
typedef struct s_TimeAvg TimeAvg;
struct s_TimeAvg {
	double buf[AVG_SIZE];
	int i;
	double max, min, avg;
};

void avg_init(TimeAvg* avg, double value)
{
	for (int i=0; i < AVG_SIZE; ++i)
		avg->buf[i] = value;
	avg->i = 0;
	avg->max = value;
	avg->min = value;
	avg->avg = value;
}

void avg_push(TimeAvg* avg, double value)
{
	int recalc = 0;
	double old = avg->buf[avg->i];
	avg->buf[avg->i] = value;
	avg->i = (avg->i + 1) %AVG_SIZE;
	avg->avg = avg->avg + (value-old) *(1.0/AVG_SIZE);
	if (old == avg->min || old == avg->max) {
		avg->max = avg->min = avg->buf[0];
		for (int i =1; i < AVG_SIZE; ++i) {
			if (avg->buf[i] < avg->min)
				avg->min = avg->buf[i];
			if (avg->buf[i] > avg->max)
				avg->max = avg->buf[i];
		}
	}	
}


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



	GLuint vbo_bg;
	GLuint vbo_bg_tex;

void init_bg_shader()
{
	char *args[] = {"aPos", "aTex", "uFramebuffer", NULL};
	bg_shader = shader_new("\
		#version 100 \n\
		attribute vec2 aPos; \n\
		attribute vec2 aTex; \n\
		varying vec2 vTex; \n\
		void main() \n\
		{ \n\
			vTex = aTex; \n\
			gl_Position = vec4(aPos, 0.0, 1.0); \n\
		}", 
	"\
		#version 100 \n\
		precision mediump float;\n\
		varying vec2 vTex;\n\
		uniform sampler2D uFramebuffer; \n\
		void main() { \n\
			gl_FragColor = texture2D(uFramebuffer, vTex);\n\
		}", args);

	// background vertexes
	glGenBuffers(1, &vbo_bg);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bg);
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0,     0.0, 0.0,  0.0, 1.0, 1.0, 0.0,  1.0, 1.0};
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	glVertexAttribPointer(bg_shader->args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(bg_shader->args[0]);

	// texture coords
	glGenBuffers(1, &vbo_bg_tex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bg_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bg_tex_coords, GL_STATIC_DRAW);
	glVertexAttribPointer(bg_shader->args[1], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(bg_shader->args[1]);
	
	// uniform frame buffer
	glUniform1i(bg_shader->args[2], 0);
}


void init_tri_shader()
{
	char *args[] = {"aPos", NULL};
	tri_shader = shader_new("\
		#version 100\n\
		attribute vec2 aPos;\n\
		void main() {\n\
			gl_Position = vec4(aPos.x / 32.0, aPos.y / 18.0, 0.0, 1.0);\n\
		}", 
	"\
		#version 100\n\
		precision mediump float;\n\
		float rand(vec2 co){\n\
			return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);\n\
		}\n\
		void main() {\n\
			gl_FragColor = vec4(0.3, 0.4, 0.7, 1.0);//rand(gl_FragCoord.xy),rand(gl_FragCoord.xy),0.3,rand(gl_FragCoord.xy));\n\
		}", 
	args);
	
	//glVertexAttribPointer(tri_shader->args[0], 2, GL_FLOAT, 0, 0, 0);
	//glEnableVertexAttribArray(tri_shader->args[0]);
}


void init_bg_framebuffer()
{
	glGenFramebuffers(1, &fb_bg_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_bg_id);
	glGenTextures(1, &tex_bg_id);
	glBindTexture(GL_TEXTURE_2D, tex_bg_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BGW, BGH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	int err;
	if (err=glGetError())
		printf("glTexImage2dError %d\n", err);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_bg_id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("BG Framebuffer NOT Ready %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER)); 
	
	glViewport(0, 0, BGW, BGH);
	glClearColor(7.0, 0.024 , 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//~ glClearColor(0.0, 0.624 , 0.682, 1.0);
	//~ glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
}

/*
		
	make_program(vert_glyphs, frag_glyphs, &prog_glyphs);	
	attr_aPos_gl =  glGetAttribLocation(prog_glyphs,  "aPos");
	unif_uColor = glGetUniformLocation(prog_glyphs, "uColor");
	unif_uScale = glGetUniformLocation(prog_glyphs, "uScale");
	unif_uTranslate = glGetUniformLocation(prog_glyphs, "uTranslate");
	unif_uCharScale = glGetUniformLocation(prog_glyphs, "uCharScale");
	unif_uCharTrans = glGetUniformLocation(prog_glyphs, "uCharTrans");

	make_program(vert_background, frag_background, &prog_background);	
	attr_aPos_bg =  glGetAttribLocation(prog_background,  "aPos");
	attr_aTex = glGetAttribLocation(prog_background,  "aTex");
	unif_uFramebuffer = glGetUniformLocation(prog_background, "uFramebuffer");
*/

	//~ printf("Create Texture %d / %d\n", bg_width, bg_height);
	//~ printf("Framebuffer %d x %d\n", fb_width, fb_height);
	//~ int max_tex_size;
	//~ glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
	//~ printf("MAX TEXT SIZE %d\n", max_tex_size);
	
	// create the offscreen FB to draw to
	// Create a vertex object for quad
		

	//~ g_transform_stack_top = 1;
	//~ reset_matrix();
	//~ cur_color_stack_top = 0;
	//~ set_color(hex(0xE1D8C7));


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//~ glViewport(0, 0, width, height);
	fb_width = width;
	fb_height = height;
	//~ printf("FBSIZZE %dx%d\n",width,height);
	if (fabs((double)width /height - monitor_aspect) > 0.01) {
		if ((double) width/height > monitor_aspect)
			glfwSetWindowSize(window, height*monitor_aspect, height);
		else
			glfwSetWindowSize(window, width, width/monitor_aspect);
		//glfwPollEvents();
		//~ printf("SETSIZE\n");
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
		//do_fullscreen();
	} else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		//~ DEBUG ^= 1;
	}
}

void init_gl()
{
	init_bg_shader();
	init_bg_framebuffer();
	init_tri_shader();

}

static void set_window(GLFWwindow *w)
{
	window = w;
	glfwMakeContextCurrent(window);
	init_gl();
	//~ glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	framebuffer_size_callback(window, width, height);
}



void init_platform()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		death("GLFW", "init", -1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	printf("MODE: %d,%d  %f %d\n", mode->width, mode->height, (double)mode->width/(double)mode->height, mode->refreshRate);
	//~ bg_width = mode->width;
	//~ bg_height = mode->height;
	monitor_aspect = 16.0/ 9.0;//(double)mode->width / (double)mode->height;
	w_window = glfwCreateWindow(BGW, BGH, "Glyphy Graphics", NULL, NULL);
	if (!w_window)
		death("GLFW", "window create", -1);
	set_window(w_window);
	printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
	printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );
	cur_state = HOVER;
}

void cmd_clear(float* rgba)
{
	glClearColor(rgba[0], rgba[1], rgba[2], rgba[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

	GLuint vbo;

void draw_tris(Points *pts)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pts->npts*2, pts->pts, GL_STREAM_DRAW);
	glVertexAttribPointer(tri_shader->args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(tri_shader->args[0]);
	//~ glValidateProgram(bg_shader->id);
	//~ glsl_check(glGetProgramiv,  glGetProgramInfoLog, bg_shader->id, GL_VALIDATE_STATUS);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, pts->npts);
	
}

void draw_tri(int num, ...)
{
	GLfloat *pts = (GLfloat*)alloca(sizeof(GLfloat)*2*num);
	va_list args;                     
	va_start(args, num);
	for(int i=0; i < num*2; ++i)
		pts[i] = (GLfloat)va_arg(args, double);
	va_end(args);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num*2, pts, GL_STREAM_DRAW);
	glVertexAttribPointer(tri_shader->args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(tri_shader->args[0]);
	glValidateProgram(bg_shader->id);
	glsl_check(glGetProgramiv,  glGetProgramInfoLog, bg_shader->id, GL_VALIDATE_STATUS);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, num);
	//~ glDeleteBuffers(1,&vbo);
}

int child_term = 0;
	
int let_child_render(void)
{
	int drawn = 0;
	static unsigned long frame =0;
	if (!child_term) {
		ipc_send(sizeof(long), GFX_FRAME, &frame);
		frame ++;
	}
	
	//~ t0 = glfwGetTime() ;		// continue drawing to the background framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fb_bg_id);
	glUseProgram(tri_shader->id);
	glViewport(0, 0, BGW, BGH);
	//~ glClear(GL_COLOR_BUFFER_BIT);
	//~ glClientWaitSync( glFenceSync(GL_SYNC_GPU_COMM`ANDS_COMPLETE, 0),GL_SYNC_FLUSH_COMMANDS_BIT, 50000000);
	//~ t1 = glfwGetTime() ;
	//~ avg_push(&stavg, (t1-t0)*1000);
	
	double t1, t0 = glfwGetTime();
	int draws = 0;
	while (t1 < t0 + 24e-3) {
		Package *pkg =ipc_recv();
		if (!pkg) {
			ipc_yield();
		} else if (pkg->type == GFX_SYNC) {
			ipc_free_pkg(pkg);
			break;
		} else if (pkg->type == GFX_PTS) {
			draw_tris( (Points*)pkg->pdata);
			//~ glFinish();
			drawn++;
		} else {
			printf("unknown cmd %d\n", pkg->type);
		}
		if (pkg)
			ipc_free_pkg(pkg);

		t1 = glfwGetTime();
	}
	//~ avg_push(&tdavg,(t1-t0)*1000);
	//~ printf("Drew %d in %.3f\n", drawn, (t1-t0)*1000);
	return drawn;
}


void render_loop(double frame_time)
{
	//~ if(g_drawing_bg) {
		//~ printf("WARNING: You forgot to call end_background_draw()\n");
		//~ end_background_draw();
	//~ }
		//~ int type = 0;
	//~ frame_time *= 2.0;
	//~ printf("Frame Time: %0.3fms\n", frame_time*1000);
	//~ fflush(stdout);
	glGenBuffers(1, &vbo);
	unsigned long frame = 0;
	double prev_time = glfwGetTime();
	TimeAvg ftavg, stavg, tdavg, bgavg, ipavg;
	avg_init(&ftavg, frame_time*1000);
	avg_init(&stavg, 1);
	avg_init(&tdavg, 1);
	avg_init(&bgavg, 1);
	avg_init(&ipavg, 1);
	
	glfwSwapInterval(0);
	glClearColor(0.0, 0.624 , 0.682, 1.0);
	double t0, t1, tstart;
	while (!glfwWindowShouldClose(window)) {
		tstart = glfwGetTime();
		avg_push(&ftavg, (tstart-prev_time)*1000);
		prev_time = tstart;
		
		//~ if (!(frame%41)) {
			//~ printf("FT: %0.3f <%0.3f / %0.3f>\n", ftavg.avg, ftavg.min, ftavg.max);
			//~ printf("ST: %0.3f <%0.3f / %0.3f>\n", stavg.avg, stavg.min, stavg.max);
			//~ printf("TD: %0.3f <%0.3f / %0.3f>\n", tdavg.avg, tdavg.min, tdavg.max);
			//~ printf("BG: %0.3f <%0.3f / %0.3f>\n", bgavg.avg, bgavg.min, bgavg.max);
			//~ printf("IP: %0.3f <%0.3f / %0.3f>\n\n", ipavg.avg, ipavg.min, ipavg.max);
			//~ fflush(stdout);
		//~ }
		//~ ipc_debug();

		
		//send frame start
		// before sending data to the child process make sure it is still alive....
		if (!child_term) {
			int status, e;
			if ((e=waitpid(-1, &status, WNOHANG)) > 0) {
				if (WIFEXITED(status)) {
					printf("CHild exited status : %d\n", WEXITSTATUS(status));
					child_term = 1;
				} else if (WIFSIGNALED(status)) {
					printf("Child exited by signal: %d\n", WTERMSIG(status));
					child_term = 1;
				} else {
					printf("Some other exit mode %d\n", status);
					child_term = 1;
				}
			} else if (e) {
				printf("WAITPID %d\n",e);
				child_term  = 1;
			}
		}
		let_child_render();
		
		// draw the background to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, fb_width, fb_height);
		glClearColor(0.0, 0.00 , 0.682, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(bg_shader->id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_bg);
		glVertexAttribPointer(bg_shader->args[0], 2, GL_FLOAT, 0, 0, 0);
		//~ glEnableVertexAttribArray(bg_shader->args[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_bg_tex);
		glVertexAttribPointer(bg_shader->args[1], 2, GL_FLOAT, 0, 0, 0);
		//~ glEnableVertexAttribArray(bg_shader->args[1]);

		//~ glActiveTexture(GL_TEXTURE0);
		//~ glBindTexture(GL_TEXTURE_2D, tex_bg_id);


		//~ glValidateProgram(bg_shader->id);
		//~ glsl_check(glGetProgramiv,  glGetProgramInfoLog, bg_shader->id, GL_VALIDATE_STATUS);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//~ glClientWaitSync( glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0),GL_SYNC_FLUSH_COMMANDS_BIT, 50000000);

		//~ glFinish();
		
		//~ t0=t1;
		glfwSwapBuffers(window);
		glfwPollEvents();
		//~ t1=glfwGetTime();
		//~ avg_push(&ipavg, (t1-t0)*1000);
	}
}

//~ void *thr_render(void *arg) {
	//~ while (!glfwWindowShouldClose(window)) {
		//~ glClear(GL_COLOR_BUFFER_BIT);
		//~ glfwSwapBuffers(window);
		//~ glfwPollEvents();
	//~ }

	//~ pthread_exit(NULL);
//~ }






//~ void lib_init(void)
//~ {
	//~ init();	
	//~ while (!glfwWindowShouldClose(window)) {
		//~ glfwPollEvents();
		//~ glClear(GL_COLOR_BUFFER_BIT);
		//~ glUseProgram(shader_program);
		//~ glDrawArrays(GL_TRIANGLES, 0, 3);
		//~ glfwSwapBuffers(window);
	//~ }
	//~ if (pthread_create(&render_thread, NULL, thr_render, NULL))
		//~ death("Pthread", "Create error", -1);
 
//~ }

//~ void lib_fini(int wait)
//~ {
	//~ pthread_join(render_thread, NULL);
	//~ death(NULL, NULL, 0);
//~ }


