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
#include "layer_pass.c"
#include "upscale_pass.c"
#include "iomem.h"

SharedMem g_shm;
SubProc g_subproc;
IOMem *io;
//~ Voice *voices;
//~ Object *objects;
uint8_t *vram;
//~ Color *palette;
//~ Sprite *sprites;
//~ Mapel *maps;
Layer *layers;

void event_callback(int type, float ts, int p1, int p2)
{
	printf("EVENT %d\n", type);
	
	
}
//~ Shader g_shader;
//~ Shader g_upscale_shader;

FrameBuffer g_fb;

void game_init(void)
{
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// create the frame buffer
	glActiveTexture(GL_TEXTURE0);
	framebuffer_init(&g_fb, 256, 256);
	on_exit(framebuffer_on_exit, &g_fb);
	layerpass_init();
	upscale_init();
	
}


void game_update(void)
{
	int r,c;
	win_size(&r, &c);

	glBindFramebuffer(GL_FRAMEBUFFER, g_fb.fbid);
	glViewport(0, 0, 256, 144);
	
	glClearColor(0.498, 0.624 , 0.682, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	layer_pre_render();
	for (int i=0; i < MAX_LAYERS; ++i) {
		if (layers[i].flags & LAYER_ON)
			layer_render(&layers[i]);	
	}
	// upscale it
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_fb.txid);
	upscale_begin(r,c);
	upscale_draw();
	gl_error_check();
	
}

void iomem_init(void *mem)
{
	io = (IOMem*)mem;
	layers = io->layers;
	vram = io->vram;
	
}

int main(int argc, char *argv[])
{
	// Setup shared mem
	if (!shm_init(&g_shm, sizeof(IOMem)))
		ABORT(1, "Couldn't created shared memory segment");
	on_exit(shm_on_exit, &g_shm);
	iomem_init(g_shm.mem);
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
	
	game_init();
	
	while(!(subproc_status(&g_subproc) || win_should_close())) {
		game_update();
		win_update();
		subproc_signal(&g_subproc);
	}

	ABORT(0, "Goodbye");
}
