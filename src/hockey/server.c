#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "logging.c"
#include "gl.c"
#include "subproc.c"
#include "share.c"

#include "iomem.h"
#include "shaders.h"

#include "vec2.c"

SharedMem g_shm;
SubProc g_subproc;
IOMem *io;
Input *input;

Shader g_player_shader;
Shader g_circle_shader;
Shader g_rect_shader;

GLuint g_player_vb = 0;
GLuint g_circle_vb = 0;
GLuint g_rect_vb = 0;

const char *gl_name = "Hockey";


void rectangle_render(Vec2 pos, Vec2 scale, float angle, Vec3 color)
{
	glUniformMatrix3fv(g_rect_shader.args[1], 1, GL_FALSE, &GW.vmat[0][0]);// uScreen
	glUniform2f(g_rect_shader.args[2], pos.x, pos.y); // uTranslate
	glUniform2f(g_rect_shader.args[3], scale.x, scale.y); //uScale
	glUniform1f(g_rect_shader.args[4], angle); //uScale
	glUniform3fv(g_rect_shader.args[5], 1, color.rgb); //uColor
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
}

void circle_render(Vec2 pos, float r1, float r2, Vec3 color)
{	
	glUniform2f(g_circle_shader.args[1], GW.w, GW.h);
	glUniform2f(g_circle_shader.args[2], pos.x, pos.y);
	glUniform2f(g_circle_shader.args[3], r1, r1);
	glUniform1f(g_circle_shader.args[4], r2);	
	glUniform3fv(g_circle_shader.args[5], 1, color.rgb);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
}


void player_render(Vec2 pos, float radius, float angle)
{	
	glUniform2f(g_player_shader.args[1], GW.w, GW.h);
	glUniform1f(g_player_shader.args[2], radius);
	glUniform2f(g_player_shader.args[3], pos.x, pos.y);
	glUniform1f(g_player_shader.args[4], M_PI*angle/180.0);
	glUniform1f(g_player_shader.args[5], 0.1);
	glUniform1f(g_player_shader.args[6], 0.1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

}


void gl_init(void)
{
	
	if (g_player_shader.id == 0) {
		INFO("First Context Change");

		ASSERT(shader_init(&g_player_shader, V_PLAYER, F_PLAYER, (char*[]){
			"aPos", "uSize", "uScale", "uOffset", "uAngle", 
			"uThrust","uGrab", NULL}), "Couldn't create fb shader");
		on_exit(shader_on_exit, &g_player_shader);

		ASSERT(shader_init(&g_circle_shader, V_RECT, F_CIRCLE, (char*[]){
			"aPos", "uSize","uOffset", "uScale", "uInnerRadius", "uColor", 
			 NULL}), "Couldn't create fb shader");
		on_exit(shader_on_exit, &g_circle_shader);
		
		ASSERT(shader_init(&g_rect_shader, V_TRANSCALE, F_SOLID, (char*[]){
			"aPos", "uScreen","uTranslate", "uScale",  "uAngle", "uColor", 
			 NULL}), "Couldn't create fb shader");
		on_exit(shader_on_exit, &g_rect_shader);
	}
	GW.zoomx = GW.zoomy = 100.0;

	INFO("Context change");
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDeleteBuffers(1, &g_player_vb);
	glGenBuffers(1, &g_player_vb);
	glDeleteBuffers(1, &g_circle_vb);
	glGenBuffers(1, &g_circle_vb);
	glDeleteBuffers(1, &g_rect_vb);
	glGenBuffers(1, &g_rect_vb);
		
		
	// Circle verts
	GLfloat cverts[] = {
		0.0, 0.0, 
		2.0/sqrt(3.0), 0.0,
		sqrt(3.0)/3.0, 1.0,
		-sqrt(3.0)/3.0, 1.0,
		-2.0/sqrt(3.0), 0.0,
		-sqrt(3.0)/3.0, -1.0,
		sqrt(3.0)/3.0, -1.0,
		2.0/sqrt(3.0), 0.0,
		};
	glBindBuffer(GL_ARRAY_BUFFER, g_circle_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, cverts, GL_STATIC_DRAW);

	// player verts
	for (int i=0; i < 2*8; ++i)
		cverts[i] *= 1.5;
	cverts[2] *= 2.0;
	cverts[14] *= 2.0;
	glBindBuffer(GL_ARRAY_BUFFER, g_player_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, cverts, GL_STATIC_DRAW);
	
	// rectangle verts
	GLfloat rverts[] = {0.0, 0.0, 0.0,1.0, 1.0, 0.0, 1.0, 1.0};
	glBindBuffer(GL_ARRAY_BUFFER, g_rect_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, rverts, GL_STATIC_DRAW);

}

void draw_rink(Vec2 pos, float scale)
{
	Rink *rink = &io->rink;
	
	float w = scale * io->rink.w;
	float h = scale * io->rink.h;
	float r = scale * io->rink.r;
	
	Vec3 rink_color = v3(0.698, 0.824 , 0.882);
	Vec3 goal_color = v3(0.4, 0.1, 0.4);
	Vec3 goal_warn = v3(0.498, 0.624 , 0.682);
	Vec3 star_color = v3(0.2, 0.2 , 0.2);
	
	glUseProgram(g_rect_shader.id);
	glBindBuffer(GL_ARRAY_BUFFER, g_rect_vb);
	glVertexAttribPointer(g_rect_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_rect_shader.args[0]);
	rectangle_render(v2(pos.x+r, pos.y), v2(w - 2.0*r, h), 0.0, rink_color);
	rectangle_render(v2(pos.x, pos.y+r), v2(w, h - 2.0*r), 0.0, rink_color);
	// corners
	glUseProgram(g_circle_shader.id);
	glBindBuffer(GL_ARRAY_BUFFER, g_circle_vb);
	glVertexAttribPointer(g_circle_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_circle_shader.args[0]);
	circle_render(v2(pos.x+r, pos.y+r), r, 0.0, rink_color);
	circle_render(v2(pos.x+w-r, pos.y+r), r, 0.0, rink_color);
	circle_render(v2(pos.x+w-r, pos.y+h-r), r, 0.0, rink_color);
	circle_render(v2(pos.x+r, pos.y+h-r), r, 0.0, rink_color);
	
	
	// goals
	for (int i=0; i < NUM_GOALS; ++i) {
		Vec2 gxy = v2add(v2mult(io->rink.goals[i].xy, scale), pos); 
		circle_render(gxy, scale * io->rink.goals[i].rmax, scale * io->rink.goals[i].rmax -5.0, goal_warn);
		circle_render(gxy, scale * io->rink.goals[i].rmin, 0.0, goal_warn);
		circle_render(gxy, scale * io->rink.goals[i].rgoal, 0.0, goal_color);
	}
	// stars
	for (int i=0; i < io->rink.n_stars; ++i) {
		Vec2 sxy = v2add(v2mult(io->rink.stars[i].xy, scale), pos); 
		circle_render(sxy, scale * io->rink.stars[i].r, 0.0, star_color);
	}
	
}

int gl_frame(void)
{
	static float angle = 0.0;
	
	if (input->alpha & KALPHA_A)
		GW.camx -= 10.0;
	if (input->alpha & KALPHA_D) 
		GW.camx += 10.0;
	if (input->alpha & KALPHA_S) 
		GW.camy -= 10.0;
	if (input->alpha &KALPHA_W) 
		GW.camy += 10.0;
	if (input->alpha & KALPHA_Q) 
		angle += 0.06;
	if (input->alpha & KALPHA_E) 
		angle -= 0.06;
	if (input->alpha & KALPHA_R) 
		GW.zoomx *= 1.1;
	if (input->alpha & KALPHA_F) 
		GW.zoomx /= 1.1;
	GW.zoomy = GW.zoomx;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	Vec2 pos = v2rot(v2(1.0, 0.0), 0*time());
	
	// draw the board
	//~ draw_rink(v2(0, 0), (g_w-20.0) / io->rink.w);
	//~ build_viewmat();
	
	glUseProgram(g_rect_shader.id);
	glBindBuffer(GL_ARRAY_BUFFER, g_rect_vb);
	glVertexAttribPointer(g_rect_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_rect_shader.args[0]);
	rectangle_render(v2(0.0, 0.0), v2(0.5, 0.9), angle, v3(0.2,0.4,0.8));
	rectangle_render(v2(1.0, 1.0), v2(0.5, 0.9), angle, v3(0.2,0.4,0.8));
	rectangle_render(v2(1.0, 0.0), v2(0.5, 0.9), angle, v3(0.2,0.4,0.8));
	rectangle_render(v2(0.0, 1.0), v2(0.5, 0.9), angle, v3(0.2,0.4,0.8));
	//~ glUseProgram(g_player_shader.id);
	//~ glBindBuffer(GL_ARRAY_BUFFER, g_player_vb);
	//~ glVertexAttribPointer(g_player_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	//~ glEnableVertexAttribArray(g_player_shader.args[0]);
	//~ player_render(v2add(v2(400.0, 300.0), v2mult(pos, 100.0)), 100.0, 0.0*time());
	input->getchar = key_pop();
	if (GW.cmd & KCMD_ESCAPE)
		input->status |= STATUS_CLOSE;
	input->cmd = GW.cmd;
	input->alpha = GW.alpha;
	input->num = GW.num;
	input->function = GW.function;
	input->cmd = GW.cmd;
	input->mod = GW.mod;

	subproc_signal(&g_subproc);
	return !subproc_status(&g_subproc);
	
}

int main_init(int argc, char *argv[])
{
	// Setup shared mem
	ASSERT(shm_init(&g_shm, sizeof(IOMem)), "Couldn't created shared memory segment");
	on_exit(shm_on_exit, &g_shm);
	io = (IOMem*)g_shm.mem;
	input = &io->input;

	memset(io, 0, sizeof(IOMem));

	io->rink.w =32;
	io->rink.h = 18;
	io->rink.r = 9;
	io->rink.goals[0] = (Goal){{9, 9}, 0.5, 2, 4 };
	io->rink.goals[1] = (Goal){ {32-9,9}, 0.5,2,4};
	io->rink.n_stars = 1;
	io->rink.stars[0] = (Circle){ {16.0,9.0}, 2.0};
	
	char *prog = argv[1];
	int a;
	for (a = 0; a < argc-1; ++a)
		argv[a] = argv[a+1];
	argv[a] = NULL;
	argv[0] = alloca(10);
	snprintf(argv[0], 10, "%x", g_shm.shmid);

	// Fork
	ASSERT(subproc_init(&g_subproc, prog, argv) >= 0, "Couldn't fork");
	on_exit(subproc_on_exit, &g_subproc);
	return 0;
}
