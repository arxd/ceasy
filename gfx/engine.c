#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#ifdef __EMSCRIPTEN__  // emscripten main

#include <GLES2/gl2.h>
//#include <EGL/egl.h>
#include <emscripten/html5.h>
#include <emscripten/emscripten.h> 

#else // windows / linux  main
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <stdio.h>


#endif // main

void death(const char *title, const char *err_str, int errid);


#include "glyphs.c"
#include "color.c"
#include "transform.c"


typedef enum {ALIGN_LEFT=1, ALIGN_RIGHT=2, ALIGN_MID=4, ALIGN_CENTER=8} Alignment;


extern void init(void);
extern void render(void);

double cur_time(void);
void start_background_draw(void);
void end_background_draw(void);
void draw_background(void);
void draw_glyph_xy(Vec xy, Alignment align, int code);
void draw_string_xy(Vec xy, Alignment align, char *fmt, ...);
void draw_grid(void);
void draw_vec(Vec v0, Vec v1);

typedef struct s_Program Program;

struct s_Program {
	GLuint frag_prog;
	GLuint shad_prog;
	GLuint prog;
	GLuint argc;
	GLuint argv[];
};


GLuint prog_glyphs;
GLuint prog_background;

GLuint glyphs_verts_id[num_glyphs];
GLuint glyphs_tris_id[num_glyphs];
GLuint attr_aPos_gl;
GLuint unif_uColor;
GLuint unif_uScale;
GLuint unif_uTranslate;
GLuint unif_uCharScale;
GLuint unif_uCharTrans;

// background
GLuint fb_bg_id;
GLuint tex_bg_id;
GLuint attr_aPos_bg;
GLuint attr_aTex;
GLuint vbo_bg;
GLuint vbo_bg_tex;
GLuint unif_uFramebuffer;
int g_drawing_bg;
GLfloat bg_tex_coords[] = {0.0, 0.0,  0.0, 1.0, 1.0, 0.0,  1.0, 1.0};

int bg_width, bg_height;
int fb_width, fb_height;

typedef enum{HOVER, PRESS, HELD, RELEASE} PressState;
int cur_state;
Vec cur_xy;

int DEBUG = 0;

int glsl_check(
	void (*get_param)(GLuint,  GLenum,  GLint *),
	void (*get_log)(GLuint,  GLsizei,  GLsizei *,  GLchar *),
	GLuint prog,
	GLuint param)
{
	GLint status;
	get_param(prog, param, &status);
	if (!status) {
		GLint len = 0;
		get_param(prog, GL_INFO_LOG_LENGTH, &len);
		if ( len > 0 ) {
			char* log = malloc(sizeof(char) * len);
			get_log(prog, len, NULL, log);
			printf("Shader Error: %d : %s\n", log, prog);
			return -1;
			//death("Shader", log, prog);
		}
	}
	return 0;
}

Program *program_new(const char *vert_src, const char *frag_src, char **args)
{
	int argc = 0;
	char *arg = args[0];
	while (arg) {
		argc ++;
	}
	
	printf("%d arguments\n", argc);
	Program *prog = malloc(sizeof(Program) + sizeof(GLuint)*argc);
	prog->argc = argc;
	
	GLuint vert_shader, frag_shader;
	// Compile Vertex Shader
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_src, NULL);
	glCompileShader(vert_shader);
	glsl_check(glGetShaderiv,  glGetShaderInfoLog, vert_shader, GL_COMPILE_STATUS);
	// Compile Fragment Shader
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_src, NULL);
	glCompileShader(frag_shader);
	glsl_check(glGetShaderiv,  glGetShaderInfoLog, frag_shader, GL_COMPILE_STATUS);
	// Link the program
	*program = glCreateProgram();
	glAttachShader(*program, vert_shader);
	glAttachShader(*program, frag_shader);
	glLinkProgram(*program);
	glsl_check(glGetProgramiv,  glGetProgramInfoLog, *program, GL_LINK_STATUS);
}


void make_program()
{
	
	
}

void load_glyphs(void)
{
	int i;
	glGenBuffers(num_glyphs, glyphs_verts_id);
	glGenBuffers(num_glyphs, glyphs_tris_id);
	
	for (i=0; i < num_glyphs; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, glyphs_verts_id[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLushort) * glyphs_verts_num[i], glyphs_verts[i], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyphs_tris_id[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * glyphs_tris_num[i], glyphs_tris[i], GL_STATIC_DRAW);
	}	
}

void init_objects()
{
	load_glyphs();

	
	char *vert_background = "\
		#version 100 \n\
		attribute vec2 aPos; \n\
		attribute vec2 aTex; \n\
		varying vec2 vTex; \n\
		void main() \n\
		{ \n\
			vTex = aTex; \n\
			gl_Position = vec4(aPos, 0.0, 1.0); \n\
		}";
	
	char *frag_background = "\
		#version 100 \n\
		precision mediump float;\n\
		varying vec2 vTex;\n\
		uniform sampler2D uFramebuffer; \n\
		void main() { \n\
			gl_FragColor = texture2D(uFramebuffer, vTex);\n\
		}";
	
	char *vert_glyphs ="\
		#version 100\n\
		attribute vec2 aPos;\n\
		uniform mat2 uScale;\n\
		uniform vec2 uTranslate;\n\
		uniform vec2 uCharScale;\n\
		uniform vec2 uCharTrans;\n\
		void main() {\n\
			vec2 xy = aPos * uCharScale + uCharTrans;\n\
			gl_Position = vec4(uScale*xy + uTranslate, 0.0, 1.0);\n\
			gl_Position.x = gl_Position.x / 16.0 - 1.0;\n\
			gl_Position.y = gl_Position.y / 9.0 - 1.0;\n\
		}";
	
	char *frag_glyphs =   "\
		#version 100\n\
		precision mediump float;\n\
		uniform vec4 uColor;\n\
		void main() {\n\
			gl_FragColor = uColor;\n\
		}";
		
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


	printf("Create Texture %d / %d\n", bg_width, bg_height);
	printf("Framebuffer %d x %d\n", fb_width, fb_height);
	int max_tex_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
	printf("MAX TEXT SIZE %d\n", max_tex_size);
	
	glGenFramebuffers(1, &fb_bg_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_bg_id);
	glGenTextures(1, &tex_bg_id);
	glBindTexture(GL_TEXTURE_2D, tex_bg_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg_width, bg_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		printf("glTexImage2D : %d\n", glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_bg_id, 0);
	printf("BG Framebuffer Ready %d (%d)\n", glCheckFramebufferStatus(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE); 
	glClearColor(0.498, 0.624 , 0.682, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Create a vertex object for quad
	glGenBuffers(1, &vbo_bg);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bg);
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0,     0.0, 0.0,  0.0, 1.0, 1.0, 0.0,  1.0, 1.0};
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	// texture coords
	glGenBuffers(1, &vbo_bg_tex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bg_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bg_tex_coords, GL_STATIC_DRAW);
		
}

void init_gl()
{
	glClearColor(0.498, 0.624 , 0.682, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	g_transform_stack_top = 1;
	reset_matrix();
	cur_color_stack_top = 0;
	set_color(hex(0xE1D8C7));
}

void reset_gl(void)
{
	reset_matrix();	
	set_color(hex(0xE1D8C7));
}

void start_background_draw(void)
{
	if (g_drawing_bg)
		death("BG", "You are already drawing to the bg!", -1);
	g_drawing_bg = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fb_bg_id);
	glViewport(0, 0, bg_width, bg_height);	
	reset_matrix();

}

void end_background_draw(void)
{
	if (g_drawing_bg) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, fb_width, fb_height);
		g_drawing_bg = 0;
	}
	reset_matrix();
	
}


void draw_background(void)
{
	glUseProgram(prog_background);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bg);
	glVertexAttribPointer(attr_aPos_bg, 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(attr_aPos_bg);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bg_tex);
	glVertexAttribPointer(attr_aTex, 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(attr_aTex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_bg_id);

	glUniform1i(unif_uFramebuffer, 0);

  	glValidateProgram(prog_background);
	glsl_check(glGetProgramiv,  glGetProgramInfoLog, prog_background, GL_VALIDATE_STATUS);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void draw_glyph_xy(Vec xy, Alignment align, int code)
{
	if (code == 32) 
		return;
	while (code < 0)
		code += num_glyphs;
	while (code >= num_glyphs)
		code -= num_glyphs;
	
	glUseProgram(prog_glyphs);
	// bind data buffers
	glBindBuffer(GL_ARRAY_BUFFER, glyphs_verts_id[code]);
	glVertexAttribPointer(attr_aPos_gl, 2, GL_UNSIGNED_SHORT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(attr_aPos_gl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyphs_tris_id[code]);
	glValidateProgram(prog_glyphs);
	glsl_check(glGetProgramiv,  glGetProgramInfoLog, prog_glyphs, GL_VALIDATE_STATUS);
	// Color
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform4fv(unif_uColor, 1, cur_color.rgba);
	// vertex
	glUniformMatrix2fv(unif_uScale, 1, 0, g_mat.scale);
	glUniform2fv(unif_uTranslate, 1, g_mat.translate);
	glUniform2fv(unif_uCharScale, 1, glyphs_sizes[code]);
	
	xy = vadd(xy, *((Vec*)glyphs_offsets[code]));
	switch(align) {
		case ALIGN_LEFT:  break;
		case ALIGN_MID: xy.x -= 0.5; break;
		case ALIGN_RIGHT: xy.x -= 1.0; break;
		case ALIGN_CENTER: xy.x -= 0.5; xy.y -= 0.5; break;
	}	
	glUniform2fv(unif_uCharTrans, 1, xy.xy);
	glDrawElements(GL_TRIANGLES, glyphs_tris_num[code], GL_UNSIGNED_BYTE, 0);
}

void draw_string_xy(Vec xy, Alignment align, char *fmt, ...)
{
	char str[256];
	int i, c = 0;
	va_list ap;
	
	va_start(ap, fmt);
	vsnprintf(str, 255, fmt, ap);
	va_end(ap);
	
	int n = strlen(str);
	i =-1; 
	while (str[++i]) {
		if (str[i] == '\n') {
			str[i++] = 0;
			if (str[i])
				draw_string_xy(vsub(xy,V(0.0, 1.5)), align, &str[i]);
			break;
		}
	}
	n = strlen(str);
	if (!n)
		return;
	
	switch(align) {
		case ALIGN_LEFT: break;
		case ALIGN_RIGHT: xy.x -= n; break;
		case ALIGN_MID: xy.x -= n/2.0; break;
		case ALIGN_CENTER: xy = vsub(xy, V(n/2.0, 0.5)); break;
	}
	
	i=-1;
	while(str[++i])
		draw_glyph_xy(vadd(xy, V(i, 0)), ALIGN_LEFT, str[i]);
}

void draw_vec(Vec v0, Vec v1)
{
	push_matrix();
	translatev(v0);
	float mag = vmag(v1);
	float ang = vangle(v1);
	scale(mag-0.25, 0.5);
	rotate(ang);
	draw_glyph_xy(V(1.0/16.0, 0), ALIGN_LEFT, 159);
	translate(mag-0.25, 0);
	scale(0.25, 0.25);
	draw_glyph_xy(V(0.0, -0.5), ALIGN_LEFT, 144);
	
	pop_matrix();
}

void draw_origin(void)
{
	push_matrix();
	push_color();
	cur_color = rgb(0.7, 0.0, 0.0); // X is red
	scale(1.0, 0.5);
	draw_glyph_xy(V(0.0, -0.5), ALIGN_LEFT, 145);
	rotate(90);
	scale(2.0, 0.5);
	cur_color = rgb(0.0, 0.7, 0.0); // Y is green
	draw_glyph_xy(V(0.0, -0.5), ALIGN_LEFT, 145);
	scale(0.5, 1.0);
	cur_color = rgb(0.0, 0.0, 0.7); // Z is blue
	draw_glyph_xy(V0, ALIGN_CENTER, 149);
	pop_color();
	pop_matrix();
}

void draw_grid(void)
{
	Vec min = V(1e100, 1e100);
	Vec max = V(-1e100, -1e100);
	
	int x, y;
	for(x=0; x < 2; ++x) for(y=0; y < 2; ++y) {
		Vec v = vtrans_inv(V(x*32.0, y*18.0));
		if (v.x < min.x)
			min.x = v.x;
		if (v.x > max.x)
			max.x = v.x;
		if (v.y < min.y)
			min.y = v.y;
		if (v.y > max.y)
			max.y = v.y;
	}
	
	printf("%.2f %.2f  / %.2f  %.2f\n", min.x, min.y, max.x, max.y);
	
	push_color();
	Color colors[] = {rgba(0.0, 0.0, 0.0, 0.7), rgba(1.0, 0.0, 0.0, 0.7), rgba(0.0, 1.0, 0.0, 0.7), rgba(0.0, 0.0, 1.0, 0.7)};
	for (x=(int)min.x - 1; x < (int)max.x + 1; ++x) {
		for (y=(int)min.y - 1; y < (int)max.y + 1; ++ y) {
			set_color(colors[ (x<0)<<1 | (y<0) ]);
			if (((x+33)%2)^((y+32)%2)) {
				draw_glyph_xy(V(x,y), ALIGN_LEFT, 0);
			}
		}
		
	}
	pop_color();
}

#ifdef __EMSCRIPTEN__  // emscripten main

#include "platform_emscripten.c"

#else // windows / linux  main

#include "platform_glfw.c"

#endif // main





