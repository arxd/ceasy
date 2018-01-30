#ifndef LAYER_PASS_C
#define LAYER_PASS_C
#include "iomem.h"

void layerpass_init(void);
void layer_pre_render(void);
void layer_render(Layer *layer);

#if __INCLUDE_LEVEL__ == 0

#include "glhelper.c"
#include "util.c"
#include "shaders.h"

FrameBuffer g_fb;
Shader g_layer_shader;
//~ Shader g_tile_shader;

Texture g_vram_tex = {0, 512, 512, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE};

extern unsigned char *vram;

void layerpass_init(void)
{
	
	char *layer_args[] = {
		"aPos","uFramebuffer", "uMap", "uMapSize", 
		"uOffset", "uTiles", "uTileSize", "uTileRowSize", 
		"uTileBits", "uPalette", "uClip", "uClamp", 
		"uFlip", NULL};
	if (!shader_init(&g_layer_shader, V_PASSTHROUGH, F_LAYER, layer_args))
		ABORT(1, "Couldn't create layer shader");
	on_exit(shader_on_exit, &g_layer_shader);

	GLuint vb, tvb;
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	// position coors
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_layer_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_layer_shader.args[0]);
	
	glActiveTexture(GL_TEXTURE0);
	tex_set(&g_vram_tex, vram);
	on_exit(tex_on_exit, &g_vram_tex);
		
}

void layer_pre_render(void)
{
	glUseProgram(g_layer_shader.id);
	glActiveTexture(GL_TEXTURE0);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	tex_set(&g_vram_tex, vram);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	
	glUniform1i(g_layer_shader.args[1], 0);
}

void layer_render(Layer *layer)
{	
	glUniform1f(g_layer_shader.args[2], layer->map.addr); // uMap
	glUniform2f(g_layer_shader.args[3], layer->map.w, layer->map.h);//uMapSize
	glUniform2f(g_layer_shader.args[4], layer->map.x, layer->map.y);//uOffset
	glUniform1f(g_layer_shader.args[5], layer->tiles.addr);//uTiles
	glUniform2f(g_layer_shader.args[6], layer->tiles.w, layer->tiles.h);//uTileSize
	glUniform1f(g_layer_shader.args[7], layer->tiles.row_bytes);//uTileRowSize
	glUniform1i(g_layer_shader.args[8], layer->tiles.bits);//uTileBits
	glUniform1f(g_layer_shader.args[9], layer->palette);//uPalette	
	glUniform2i(g_layer_shader.args[10],  1,1);// ((layer->border>>4)&0xf) == BDR_CLIP, ((layer->border)&0xf)  == BDR_CLIP); // uClip
	glUniform2i(g_layer_shader.args[11], 1,1);//(layer->border>>4)&0x1, layer->border&1); // uClamp
	glUniform2i(g_layer_shader.args[12], 1,1);//((layer->border>>4)&0xf)  == BDR_FLIP, ((layer->border)&0xf)  == BDR_FLIP); // flip
	glViewport(0, 0, 256, 144);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	gl_error_check();
}


#endif
#endif
