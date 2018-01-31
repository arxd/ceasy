#ifndef UPSCALE_PASS_C
#define UPSCALE_PASS_C

void upscale_begin(int w, int h);
void upscale_init(void);
void upscale_draw(void);

#if __INCLUDE_LEVEL__ == 0

#include "glhelper.c"
#include "util.c"
#include "shaders.h"

Shader g_upscale_shader;

extern unsigned char *vram;

void upscale_init(void)
{
	
	// create the shader
	char *args[] = {"aPos", "uSize", "uFramebuffer", NULL};
	if (!shader_init(&g_upscale_shader,V_PASSTHROUGH, F_NEAREST, args))
		ABORT(1, "Couldn't create fb shader");
	on_exit(shader_on_exit, &g_upscale_shader);
	
	//~ GLuint vb, tvb;
	//~ GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	//~ // position coors
	//~ glGenBuffers(1, &vb);
	//~ glBindBuffer(GL_ARRAY_BUFFER, vb);
	//~ glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	//~ glVertexAttribPointer(g_upscale_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	//~ glEnableVertexAttribArray(g_upscale_shader.args[0]);
}

void upscale_begin(int w, int h)
{
	glUseProgram(g_upscale_shader.id);
	glUniform2f(g_upscale_shader.args[1], w, h);
	glUniform1i(g_upscale_shader.args[2], 0);
	glViewport(0,0,w,h);
}

void upscale_draw(void)
{
	GLuint vb;
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	// position coors
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_upscale_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_upscale_shader.args[0]);


	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDeleteBuffers(1, &vb);
	gl_error_check();
}

#endif
#endif
