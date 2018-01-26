#include <time.h>
#include <stdio.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "util.c"
#include "window_glfw.c"
#include "glhelper.c"
#include "subproc.c"
#include "share.c"

SharedMem g_shm;
SubProc g_subproc;
unsigned char *vram;

void event_callback(int type, float ts, int p1, int p2)
{
	printf("EVENT %d\n", type);
	
	
}
Texture g_tex = {0, 256,256, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE};
Shader *g_shader;

//~ void termination_handler(int signum)
//~ {
	
//~ }

//~ int
//~ main (void)
//~ {
  //~ …
  //~ if (signal (SIGINT, termination_handler) == SIG_IGN)
    //~ signal (SIGINT, SIG_IGN);
  //~ if (signal (SIGHUP, termination_handler) == SIG_IGN)
    //~ signal (SIGHUP, SIG_IGN);
  //~ if (signal (SIGTERM, termination_handler) == SIG_IGN)
    //~ signal (SIGTERM, SIG_IGN);
  //~ …
//~ }
void game_init(void)
{
	tex_set(&g_tex, vram);
	
	char *args[] = {"aPos", "uVram", "uSize", NULL};
	g_shader = shader_new("\
		#version 100\n\
		attribute vec2 aPos; \n\
		void main() \n\
		{ \n\
			gl_Position = vec4(aPos, 0.0, 1.0); \n\
		}", 
	"\
		#version 100\n\
		precision mediump float;\n\
		uniform sampler2D uVram; \n\
		uniform vec2 uSize;\n\
		void main() { \n\
			//gl_FragColor = texture2D(uVram, vTex);\n\
			//fragColor = (gl_FragCoord.x<25.0) ? vec4(uSize, 0.0, 1.0) : vec4(uSize, 1.0, 1.0);\n\
			int r = int(gl_FragCoord.y *144.0 / uSize.y);\n\
			int c = int(gl_FragCoord.x * 256.0 / uSize.x);\n\
			gl_FragColor = vec4((r==8)?1.0: 0.0, (c==10)?1.0:0.0, 0.0, 1.0);\n\
		}", args);

	// background vertexes
	GLuint rect;
	glGenBuffers(1, &rect);
	glBindBuffer(GL_ARRAY_BUFFER, rect);
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_shader->args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_shader->args[0]);

	// texture coords
	//~ glGenBuffers(1, &vbo_bg_tex);
	//~ glBindBuffer(GL_ARRAY_BUFFER, vbo_bg_tex);
	//~ glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bg_tex_coords, GL_STATIC_DRAW);
	//~ glVertexAttribPointer(bg_shader->args[1], 2, GL_FLOAT, 0, 0, 0);
	//~ glEnableVertexAttribArray(bg_shader->args[1]);
	
	// uniform vram
	//glUniform1i(g_shader->args[1], 0);

	glUseProgram(g_shader->id);

}

void game_update(void)
{
	//~ tex_set(&g_tex, io.vram);
	clear(0.0, 0.1, 0.5);
	int w,h;
	win_size(&w, &h);
	glUniform2f(g_shader->args[2],w, h);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	int err;
	if ( (err=glGetError()) )
		ABORT(3, "Error %d", err);
	
	
}

void vram_init(void)
{
	vram = g_shm.mem;
	
}

int main(int argc, char *argv[])
{
	// Setup shared mem
	if (!shm_init(&g_shm, 256*256 + sizeof(IPC)*2))
		ABORT(1, "Couldn't created shared memory segment");
	on_exit(shm_on_exit, &g_shm);
	
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
	vram_init();
	
	//~ IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	//~ ipc_init(ipc, ipc+1);
	
	if (!win_init(event_callback))
		ABORT(3, "win_init failed");
	on_exit(win_on_exit, 0);
	
	//~ while(!subproc_status(&g_subproc)) {
		//~ printf("MAIN\n");
		//~ sleep(1);
	//~ }

	ABORT(0, "Goodbye");
}
