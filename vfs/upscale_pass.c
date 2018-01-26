#ifndef UPSCALE_PASS_C
#define UPSCALE_PASS_C

void upscale_begin(int w, int h);
void upscale_init(void);
void upscale_draw(void);


#if __INCLUDE_LEVEL__ == 0

#include "glhelper.c"
#include "util.c"

//~ FrameBuffer g_fb;
Shader g_upscale_shader;
//~ Texture g_vram_tex = {0, 256,256, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE};

extern unsigned char *vram;

void upscale_init(void)
{
	
	// create the shader
	char *args[] = {"aPos", "uSize", "uFramebuffer", NULL};
	if (!shader_init(&g_upscale_shader, "\
		#version 100 \n\
		attribute vec2 aPos; \n\
		void main() \n\
		{ \n\
			gl_Position = vec4(aPos, 0.0, 1.0); \n\
		}", "\
		#version 100 \n\
		precision mediump float;\n\
		uniform sampler2D uFramebuffer; \n\
		uniform vec2 uSize;\n\
		void main() { \n\
			int c = int(gl_FragCoord.x *256.0 / uSize.x);\n\
			int r = int(gl_FragCoord.y * 144.0/uSize.y);\n\
			vec4 s =  texture2D(uFramebuffer, vec2((float(c)+0.5)/256.0, (float(r)+0.5)/256.0));\n\
			gl_FragColor = s;\n\
		}", args))
		ABORT(1, "Couldn't create fb shader");
	on_exit(shader_on_exit, &g_upscale_shader);
	
	GLuint vb, tvb;
	GLfloat bgverts[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0};
	// position coors
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, bgverts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_upscale_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_upscale_shader.args[0]);

	// Texture for the vram
	//~ int i;
	//~ for (i=0; i < 256*256; ++i) {
		//~ vram[i] = rand()%256;
	//~ }
	//vram[256*0+1] = 254;
	
	//~ glActiveTexture(GL_TEXTURE0);
	//~ tex_set(&g_vram_tex, vram);
	//~ DEBUG("SET UP TEXTURE %d", vram[321]);
	//~ on_exit(tex_on_exit, &g_vram_tex);
	
}

void upscale_begin(int w, int h)
{
	// start drawing to the 256x256 fb
	

	
	glUseProgram(g_upscale_shader.id);
	glUniform2f(g_upscale_shader.args[1], w, h);
	glUniform1i(g_upscale_shader.args[2], 0);
	glViewport(0,0,w,h);
	//~ glViewport(0, 0, 256, 144);
}

void upscale_draw(void)
{
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	gl_error_check();
}

#endif
#endif
