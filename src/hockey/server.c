#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "util.c"
#include "window_glfw.c"
#include "glhelper.c"
#include "subproc.c"
#include "share.c"

#include "iomem.h"
#include "shaders.h"

#include "vec2.c"

SharedMem g_shm;
SubProc g_subproc;
IOMem *io;
Input *input;

void event_callback(int type, float ts, int p1, int p2)
{
	printf("EVENT %d\n", type);
	
	
}

Shader g_player_shader;
Shader g_circle_shader;
Shader g_rect_shader;

GLuint g_player_vb = 0;
GLuint g_circle_vb = 0;
GLuint g_rect_vb = 0;

int g_w, g_h;

void rectangle_render(Vec2 pos, Vec2 scale, float r, float g, float b)
{
	glUniform2f(g_player_shader.args[1], g_w, g_h);
	glUniform2f(g_player_shader.args[2], scale.x, scale.y);
	glUniform2f(g_player_shader.args[3], pos.x, pos.y);
	glUniform3f(g_player_shader.args[4], r, g, b);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
}

void circle_render(Vec2 pos, float r1, float r2, float r, float g, float b)
{	
	glUniform2f(g_player_shader.args[1], g_w, g_h);
	glUniform1f(g_player_shader.args[2], r1);
	glUniform1f(g_player_shader.args[3], r2);	
	glUniform2f(g_player_shader.args[4], pos.x, pos.y);
	glUniform3f(g_player_shader.args[5], r, g, b);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
}


void player_render(Vec2 pos, float radius, float angle)
{	
	glUniform2f(g_player_shader.args[1], g_w, g_h);
	glUniform1f(g_player_shader.args[2], radius);
	glUniform2f(g_player_shader.args[3], pos.x, pos.y);
	glUniform1f(g_player_shader.args[4], M_PI*angle/180.0);
	glUniform1f(g_player_shader.args[5], 0.1);
	glUniform1f(g_player_shader.args[6], 0.1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

}


void gl_context_change(void)
{
	
	if (g_player_shader.id == 0) {
		DEBUG("First Context Change");

		if (!shader_init(&g_player_shader, V_CIRCLE, F_PLAYER, (char*[]){
			"aPos", "uSize", "uScale", "uOffset", "uAngle", 
			"uThrust","uGrab", NULL}))
			ABORT(1, "Couldn't create fb shader");
		on_exit(shader_on_exit, &g_player_shader);

		if (!shader_init(&g_circle_shader, V_CIRCLE, F_CIRCLE, (char*[]){
			"aPos", "uSize", "uScale",  "uInnerRadius","uOffset", "uColor", 
			 NULL}))
			ABORT(1, "Couldn't create fb shader");
		on_exit(shader_on_exit, &g_player_shader);
		
		if (!shader_init(&g_rect_shader, V_RECT, F_RECT, (char*[]){
			"aPos", "uSize","uOffset", "uScale", "uColor", 
			 NULL}))
			ABORT(1, "Couldn't create fb shader");
		on_exit(shader_on_exit, &g_player_shader);
	}
	
	DEBUG("Context change");
	glClearColor(0.498, 0.624 , 0.682, 1.0);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDeleteBuffers(1, &g_player_vb);
	glGenBuffers(1, &g_player_vb);
	glDeleteBuffers(1, &g_circle_vb);
	glGenBuffers(1, &g_circle_vb);
	glDeleteBuffers(1, &g_rect_vb);
	glGenBuffers(1, &g_rect_vb);
		
	GLfloat verts[] = {
		0.0, 0.0, 
		2.0/sqrt(3.0), 0.0,
		sqrt(3.0)/3.0, 1.0,
		-sqrt(3.0)/3.0, 1.0,
		-2.0/sqrt(3.0), 0.0,
		-sqrt(3.0)/3.0, -1.0,
		sqrt(3.0)/3.0, -1.0,
		2.0/sqrt(3.0), 0.0,
		};
		
	// Circle verts
	glBindBuffer(GL_ARRAY_BUFFER, g_circle_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_circle_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_circle_shader.args[0]);
 
	// player verts
	for (int i=0; i < 2*8; ++i)
		verts[i] *= 1.5;
	verts[2] *= 2.0;
	verts[14] *= 2.0;
	glBindBuffer(GL_ARRAY_BUFFER, g_circle_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_player_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_player_shader.args[0]);
	
	// rectangle verts
	GLfloat rverts[] = {0.0, 0.0, 0.0,1.0, 1.0, 0.0, 1.0, 1.0};
	glBindBuffer(GL_ARRAY_BUFFER, g_rect_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, rverts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_rect_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_rect_shader.args[0]);

}



void game_update(void)
{
	win_size(&g_w, &g_h);

	glClear(GL_COLOR_BUFFER_BIT);
	
	Vec2 pos = v2rot(v2(1.0, 0.0), 0*time());
	
	// draw the board
	//~ glUseProgram(g_rect_shader.id);
	//~ rectangle_render(v2(0.0, 0.0), v2(30.0, 12.0), 0.3, 0.0, 0.0);
	
	glUseProgram(g_circle_shader.id);
	glBindBuffer(GL_ARRAY_BUFFER, g_circle_vb);
	//~ glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, verts, GL_STATIC_DRAW);
	//~ glVertexAttribPointer(g_circle_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_circle_shader.args[0]);
	circle_render(v2(600.0, 400.0), 40.0, 30.0, 0.0, 0.3, 0.0);

	//~ glUseProgram(g_player_shader.id);
	//~ player_render(v2add(v2(400.0, 300.0), v2mult(pos, 100.0)), 100.0, 0.0*time());	
}

int main(int argc, char *argv[])
{
	// Setup shared mem
	if (!shm_init(&g_shm, sizeof(IOMem)))
		ABORT(1, "Couldn't created shared memory segment");
	on_exit(shm_on_exit, &g_shm);
	io = (IOMem*)g_shm.mem;
	input = &io->input;
	DEBUG("Zero Mem");
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
