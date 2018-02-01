#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "util.c"
#include "window_glfw.c"
#include "glhelper.c"
#include "subproc.c"
#include "share.c"
#include "iomem.h"
#include "shaders.h"

SharedMem g_shm;
SubProc g_subproc;
IOMem *io;
volatile uint8_t *vram;
Input *input;
Layer *layers;

void event_callback(int type, float ts, int p1, int p2)
{
	printf("EVENT %d\n", type);
	
	
}

Shader g_upscale_shader;
Shader g_layer_shader;

GLuint g_vb = 0;

FrameBuffer g_fb = {0};
Texture g_vram_tex = {0, 512, 512, GL_ALPHA, GL_UNSIGNED_BYTE};

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
	glUniform2i(g_layer_shader.args[10], ((layer->border>>4)&0x7) == BDR_CLIP, ((layer->border)&0x7)  == BDR_CLIP); // uClip
	glUniform2i(g_layer_shader.args[11], (layer->border>>4)&0x1, layer->border&1); // uClamp
	glUniform2i(g_layer_shader.args[12], ((layer->border>>4)&0x7)  == BDR_FLIP, ((layer->border)&0x7)  == BDR_FLIP); // uFlip
	glUniform2i(g_layer_shader.args[13], !!(layer->flags&LAYER_FLIPX),!!(layer->flags)&LAYER_FLIPY); // uReverse
	glUniform1f(g_layer_shader.args[14], (layer->flags&LAYER_AUX)? layer->map.aux: -1.0); // uReverse

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}


void gl_context_change(void)
{
	//GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	GLfloat bggverts[] = {0.0, 0.0, 0.0, 0.1,0.1, 0.0, 0.1,0.1};
	
	if (g_fb.id == 0) {
		DEBUG("First Context Change");
		glActiveTexture(GL_TEXTURE0);
		
		//char *upscale_args[] = {;
		if (!shader_init(&g_upscale_shader,V_PASSTHROUGH, F_NEAREST, (char*[]){"aPos", "uSize", "uFramebuffer", NULL}))
			ABORT(1, "Couldn't create fb shader");
		on_exit(shader_on_exit, &g_upscale_shader);

		char *layer_args[] = {
			"aPos","uFramebuffer", "uMap", "uMapSize", 
			"uOffset", "uTiles", "uTileSize", "uTileRowSize", 
			"uTileBits", "uPalette", "uClip", "uClamp", 
			"uFlip", "uReverse", "uAux", NULL};
		if (!shader_init(&g_layer_shader, V_PASSTHROUGH, F_LAYER, layer_args))
			ABORT(1, "Couldn't create layer shader");
		on_exit(shader_on_exit, &g_layer_shader);
		tex_set(&g_vram_tex, NULL);
		on_exit(tex_on_exit, &g_vram_tex);
	}
	
	DEBUG("Context change");
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	framebuffer_init(&g_fb, 256, 256);
	glClearColor(0.498, 0.624 , 0.682, 1.0);

	glDeleteBuffers(1, &g_vb);
	glGenBuffers(1, &g_vb);
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	glBindBuffer(GL_ARRAY_BUFFER, g_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_upscale_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_upscale_shader.args[0]);
	glVertexAttribPointer(g_layer_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_layer_shader.args[0]);
	
}



void game_update(void)
{
	int w,h;
	win_size(&w, &h);

	glBindFramebuffer(GL_FRAMEBUFFER, g_fb.id);
	glViewport(0, 0, 256, 144);
	glUseProgram(g_layer_shader.id);
	tex_set(&g_vram_tex, (uint8_t*)vram);
	glUniform1i(g_layer_shader.args[1], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	for (int i=0; i < MAX_LAYERS; ++i) {
		if (layers[i].flags & LAYER_ON)
			layer_render(&layers[i]);	
	}
	
	// upscale it
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0,w,h);
	
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, g_fb.tex.id);
	glUseProgram(g_upscale_shader.id);
	glUniform2f(g_upscale_shader.args[1], w, h);
	glUniform1i(g_upscale_shader.args[2], 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	//~ gl_error_check();
	
}

int main(int argc, char *argv[])
{
	// Setup shared mem
	if (!shm_init(&g_shm, sizeof(IOMem)))
		ABORT(1, "Couldn't created shared memory segment");
	on_exit(shm_on_exit, &g_shm);
	io = (IOMem*)g_shm.mem;
	layers = io->layers;
	vram = io->vram;
	input = &io->input;
	memset(io, 0, sizeof(IOMem));

	char *prog = argv[1];
	int a;
	for (a = 0; a < argc-1; ++a)
		argv[a] = argv[a+1];
	argv[a] = NULL;
	argv[0] = alloca(10);
	snprintf(argv[0], 10, "%x", g_shm.shmid);

	// Fork
	if (subproc_init(&g_subproc, prog, argv) < 0)
		ABORT(2, "Couldn't fork");
	on_exit(subproc_on_exit, &g_subproc);
	
	// Start initialization
	//~ IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	//~ ipc_init(ipc, ipc+1);
	
	if (!win_init(256*4,144*4, event_callback))
		ABORT(3, "win_init failed");
	on_exit(win_on_exit, 0);
		
	while(!(subproc_status(&g_subproc))) {
		game_update();
		win_update();
		subproc_signal(&g_subproc);
	}

	ABORT(0, "Goodbye");
}
