#ifndef LAYER_PASS_C
#define LAYER_PASS_C

void layerpass_begin(void);
void layerpass_init(void);
void layerpass_draw(void);


#if __INCLUDE_LEVEL__ == 0

#include "glhelper.c"
#include "util.c"
#include "shaders.h"

FrameBuffer g_fb;
Shader g_layer_shader;
Texture g_vram_tex = {0, 256,256, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE};

extern unsigned char *vram;

void layerpass_init(void)
{
	
	// create the shader
	char *args[] = {"aPos", "uFramebuffer", NULL};
	if (!shader_init(&g_layer_shader, V_PASSTHROUGH, F_VRAM, args))
		ABORT(1, "Couldn't create fb shader");
	on_exit(shader_on_exit, &g_layer_shader);
	
	GLuint vb, tvb;
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	// position coors
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_layer_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_layer_shader.args[0]);

	// Texture for the vram
	//~ int i;
	//~ for (i=0; i < 256*256; ++i) {
		//~ vram[i] = rand()%256;
	//~ }
	//vram[256*0+1] = 254;
	
	glActiveTexture(GL_TEXTURE0);
	tex_set(&g_vram_tex, vram);
	DEBUG("SET UP TEXTURE %d", vram[321]);
	on_exit(tex_on_exit, &g_vram_tex);
	
}

void layerpass_begin(void)
{
	// start drawing to the 256x256 fb
	

	
	glUseProgram(g_layer_shader.id);
	glActiveTexture(GL_TEXTURE0);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	tex_set(&g_vram_tex, vram);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	glUniform1i(g_layer_shader.args[1], 0);
	glViewport(0, 0, 256, 144);
}

void layerpass_draw(void)
{
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	gl_error_check();
}

#endif
#endif
