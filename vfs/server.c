#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "cpu.h"
#include "window.h"
#include "glhelper.c"
#include "util.c"

typedef struct s_SubProc SubProc;
struct s_SubProc {
	int pid;
	int status;
};

int subproc_init(SubProc *self)
{
	memset(self, 0, sizeof(SubProc));
	self->pid = fork();
	if (self->pid < 0) {
		self->pid = 0;
		return -1;
	}
	printf("Forked child %d\n", self->pid);
	return self->pid;
}

void subproc_fini(int status, SubProc *self)
{
	if (!self->status) {
		printf("Killing child %d\n", self->pid);
		kill(self->pid, SIGKILL);
	} else if (self->status < 0)
		printf("Child killed by signal %d\n", -self->status);
	else if (self->status == 1)
		printf("Child exited normally\n");
	else
		printf("Child exited with error %d\n", self->status);
	waitpid(self->pid, NULL, 0);
}

int subproc_status(SubProc *self)
{
	if (self->status)
		return self->status;
	int wpid = waitpid(-1, &self->status, WNOHANG);
	if (wpid < 0)
		ABORT(1, "WaitPID Failed %d", wpid);
	if (wpid == 0)
		return 0;
	
	if (WIFEXITED(self->status))
		self->status =  1 + WEXITSTATUS(self->status); 
	else if (WIFSIGNALED(self->status))
		self->status = -WTERMSIG(self->status);
	else
		self->status = -1;
	return self->status;
}

SharedMem g_shm = {0};
SubProc g_subproc = {0};

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

int child_status()
{
}

void main_child(int argc, char *argv[])
{
	char *prog = argv[1];
	int a;
	for (a = 0; a < argc-1; ++a)
		argv[a] = argv[a+1];
	argv[a] = NULL;
	argv[0] = alloca(10);
	snprintf(argv[0], 10, "%x", g_shm.shmid);
	execv(prog, argv);
	ABORT(0, "The program '%s' could not be found", prog);
}

void vram_init()
{
	io.vram = g_shm.mem;
	
}

void main_shutdown(void)
{
	
}


void main_parent()
{
	vram_init();
	
	IPC *ipc = (IPC*)(g_shm.mem + 256*256);
	ipc_init(ipc, ipc+1);
	
	//~ win_init(event_callback);
	//~ game_init();
	while(!subproc_status(&g_subproc)) {
		printf("MAIN\n");
		sleep(1);
	}
	//~ int stat = 0;
	//~ double prev_t = win_time();
	//~ while (!(stat = child_status()) && !win_should_close()) {
		//~ game_update();
		//~ win_update();
	//~ }
	
	//~ printf("Exiting\n");
	//~ win_fini();
	//~ waitpid(-1, NULL, 0);
	ABORT(0, "Goodbye");
}

int main(int argc, char *argv[])
{
	// Setup shared mem
	if (!shm_init(&g_shm, 256*256 + sizeof(IPC)*2))
		ABORT(1, "Couldn't created shared memory segment");
	on_exit(shm_fini, &g_shm);
	
	// Fork
	if (subproc_init(&g_subproc) < 0)
		ABORT(2, "Couldn't fork");
	if (g_subproc.pid) {
		on_exit(subproc_fini, &g_subproc);
		main_parent();  // doesn't return
	}
	main_child(argc, argv); // doesn't return
}
