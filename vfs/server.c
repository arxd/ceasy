#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "cpu.h"
#include "window.h"
#include "glhelper.c"

void event_callback(int type, float ts, int p1, int p2)
{
	printf("EVENT %d\n", type);
	
	
}
Texture g_tex = {0, 256,256, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE};
Shader *g_shader;


void game_init(void)
{
	tex_set(&g_tex, io.vram);
	
	char *args[] = {"aPos", "uVram", "uSize", NULL};
	g_shader = shader_new("\
		#version 300 es\n\
		in vec2 aPos; \n\
		void main() \n\
		{ \n\
			gl_Position = vec4(aPos, 0.0, 1.0); \n\
		}", 
	"\
		#version 300 es\n\
		precision mediump float;\n\
		uniform lowp usampler2D uVram; \n\
		uniform vec2 uSize;\n\
		out vec4 fragColor;\n\
		void main() { \n\
			//gl_FragColor = texture2D(uVram, vTex);\n\
			//fragColor = (gl_FragCoord.x<25.0) ? vec4(uSize, 0.0, 1.0) : vec4(uSize, 1.0, 1.0);\n\
			int r = int(gl_FragCoord.y *144.0 / uSize.y);\n\
			int c = int(gl_FragCoord.x * 256.0 / uSize.x);\n\
			fragColor = vec4(bool(r%8)?1.0: 0.0, bool(c%8)?1.0:0.0, 0.0, 1.0);\n\
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
		death("glError", "Error", err);
	
	
}

int child_status()
{
	int status;
	int wpid = waitpid(-1, &status, WNOHANG);
	if (wpid < 0)
		death("WaitPID Failed", "", wpid);
	if (wpid == 0)
		return 0;
	
	if (WIFEXITED(status))
		return 1 + WEXITSTATUS(status); 
	
	if (WIFSIGNALED(status))
		return  -WTERMSIG(status);
	
	return -1;
}

int main_child(int shmid, int argc, char *argv[])
{
	char *prog = argv[1];
	int a;
	for (a = 0; a < argc-1; ++a)
		argv[a] = argv[a+1];
	argv[a] = NULL;
	argv[0] = alloca(10);
	snprintf(argv[0], 10, "%x", shmid);
	execv(prog, argv);
	printf("The program '%s' could not be found", prog);
	death(NULL, NULL, -128);
	return 0;
}

void vram_init(uint8_t *mem)
{
	io.vram = mem;
	
}

int main_parent(uint8_t *mem)
{
	vram_init(mem);
	IPC *ipc = (IPC*)(mem + 256*256);
	ipc_init(ipc, ipc+1);
	win_init(event_callback);
	game_init();
	
	int stat = 0;
	double prev_t = win_time();
	while (!(stat = child_status()) && !win_should_close()) {
		game_update();
		win_update();
	}
	
	printf("Exiting\n");
	if (!stat) {
		printf("Killing child\n");
		kill(-1, SIGKILL);
	} else if (stat < 0)
		printf("Child killed by signal %d\n", -stat);
	else if (stat == 1)
		printf("Child exited normally\n");
	else
		printf("Child exited with error %d\n", stat);
	win_fini();
	waitpid(-1, NULL, 0);
	//~ death(NULL, NULL, 0);
	return 0;
}


int main(int argc, char *argv[])
{
	int shmid;
	uint8_t *mem = shalloc(&shmid, 256*256 + sizeof(IPC)*2);

	if (fork())
		return main_parent(mem);
	return main_child(shmid, argc, argv);
	
}
