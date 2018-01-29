#ifndef LAYER_PASS_C
#define LAYER_PASS_C
#include "iomem.h"

void layerpass_begin(void);
void layerpass_init(void);
void layerpass_draw(void);

void layer_pre_render(void);
void object_render(Object *obj);
void layer_render(Layer *layer);

#if __INCLUDE_LEVEL__ == 0

#include "glhelper.c"
#include "util.c"
#include "shaders.h"

FrameBuffer g_fb;
Shader g_layer_shader;
//~ Shader g_tile_shader;

Texture g_vram_tex = {0, 256, 256, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE};

extern unsigned char *vram;

void layerpass_init(void)
{
	
	// create the shader
	//~ char *vram_args[] = {"aPos", "uFramebuffer", NULL};
	//~ if (!shader_init(&g_layer_shader, V_PASSTHROUGH, F_VRAM, vram_args))
		//~ ABORT(1, "Couldn't create fb shader");
	//~ on_exit(shader_on_exit, &g_layer_shader);
	
	//~ char *tile_args[] = {"aPos", "uAddress", "uSize", "uOffset", "uFramebuffer", "uClip", "uClamp", "uFlip", NULL};
	//~ if (!shader_init(&g_tile_shader, V_PASSTHROUGH, F_TILE, tile_args))
		//~ ABORT(1, "Couldn't create tile shader");
	//~ on_exit(shader_on_exit, &g_tile_shader);
	
	char *layer_args[] = {"aPos","uFramebuffer", "uMap", "uMapSize", "uOffset", 
			"uTiles", "uTileSize", "uTileRowSize", "uTileBits", "uPalette", "uClip", "uClamp", "uFlip", NULL};
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

//~ void object_pre_render(void)
//~ {
	//~ glUseProgram(g_tile_shader.id);
	//~ glActiveTexture(GL_TEXTURE0);
	//~ tex_set(&g_vram_tex, vram);
//~ }

//~ void object_render(Object *obj)
//~ {
	//~ ///~ printf("%d, (%dx%d), (%d, %d) %x/%x\n", obj->address, obj->width, obj->height, obj->xoff, obj->yoff, obj->xBounds, obj->yBounds);
	
	//~ glUniform1f(g_tile_shader.args[1], 256.0*180.0 + obj->address*2.0);//address
	//~ glUniform2f(g_tile_shader.args[2], obj->width, obj->height);//size
	//~ glUniform2f(g_tile_shader.args[3], obj->xoff, obj->yoff);//offset
	//~ glUniform1i(g_tile_shader.args[4], 0); // framebuffer
	//~ glUniform2i(g_tile_shader.args[5], obj->xBounds == OBJ_CLIP, obj->yBounds == OBJ_CLIP); // clip
	//~ glUniform2i(g_tile_shader.args[6], obj->xBounds&1, obj->yBounds&1); // clamp
	//~ glUniform2i(g_tile_shader.args[7], obj->xBounds == OBJ_FLIP, obj->yBounds == OBJ_FLIP); // flip
	//~ glViewport(0, 0, 256, 144);
	//~ glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//~ gl_error_check();
//~ }




//~ void layerpass_begin(void)
//~ {
	// start drawing to the 256x256 fb
	

	
	//~ glUseProgram(g_layer_shader.id);
	//~ glActiveTexture(GL_TEXTURE0);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	//~ tex_set(&g_vram_tex, vram);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	//~ glUniform1i(g_layer_shader.args[1], 0);
	//~ glViewport(0, 0, 256, 144);
//~ }

//~ void layerpass_draw(void)
//~ {
	//~ glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//~ gl_error_check();
//~ }

void layer_pre_render(void)
{
	glUseProgram(g_layer_shader.id);
	glActiveTexture(GL_TEXTURE0);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	tex_set(&g_vram_tex, vram);
	//~ glBindTexture(GL_TEXTURE_2D, g_vram_tex.id);
	
	glUniform1i(g_layer_shader.args[1], 0);
	glViewport(0, 0, 256, 144);
	
	
}

void layer_render(Layer *layer)
{
	int bits = layer->tileBits;
	if (bits != 1 || bits != 2 || bits != 4)
		bits = 1;
	int bytes_per_row = layer->tileW * bits;
	bytes_per_row = (bytes_per_row/8) + !!(bytes_per_row%8);
	
	glUniform1f(g_layer_shader.args[2], layer->map); // map address
	glUniform2f(g_layer_shader.args[3], layer->mapW, layer->mapH);//size
	glUniform2f(g_layer_shader.args[4], layer->mapX, layer->mapY);//offset
	glUniform1f(g_layer_shader.args[5], layer->tiles);//offset
	glUniform2f(g_layer_shader.args[6], layer->tileW, layer->tileH);//offset
	glUniform1f(g_layer_shader.args[7], bytes_per_row);//offset
	glUniform1i(g_layer_shader.args[8], bits);//offset
	glUniform1f(g_layer_shader.args[9], layer->palette);//offset	
	glUniform2i(g_layer_shader.args[10], ((layer->border>>4)&0xf) == BDR_CLIP, ((layer->border)&0xf)  == BDR_CLIP); // clip
	glUniform2i(g_layer_shader.args[11], (layer->border>>4)&0x1, layer->border&1); // clamp
	glUniform2i(g_layer_shader.args[12], ((layer->border>>4)&0xf)  == BDR_FLIP, ((layer->border)&0xf)  == BDR_FLIP); // flip
	glViewport(0, 0, 256, 144);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	gl_error_check();
}

#endif
#endif
