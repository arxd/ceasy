#ifndef GLHELPER_H
#define GLHELPER_H

#include <stdlib.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

typedef struct s_Shader Shader;
struct s_Shader {
	GLuint id;
	GLuint argc;
	GLuint args[16];
};

typedef struct s_Texture Texture;
struct s_Texture {
	GLuint id;
	int w, h;
	GLint internal_format;
	GLenum format;
	GLenum type;
	//~ void *data;
};

typedef struct s_FrameBuffer FrameBuffer;
struct s_FrameBuffer {
	GLuint fbid;
	GLuint txid;
	int w, h;
};

int shader_init(Shader *self, const char *vert_src, const char *frag_src, char *args[]);
void shader_fini(Shader *self);
void shader_on_exit(int status, void *arg);

void clear(GLfloat r, GLfloat g, GLfloat b);
void tex_set(Texture *self, void * pixels);
void tex_on_exit(int status, void *arg);
void tex_fini(Texture *self);


void gl_error_check(void);

int framebuffer_init(FrameBuffer *self, int w, int h);
void framebuffer_fini(FrameBuffer *self);
void framebuffer_on_exit(int status, void *arg);


#if __INCLUDE_LEVEL__ == 0

#include <stdio.h>
#include <malloc.h>
#include "util.c"

void clear(GLfloat r, GLfloat g, GLfloat b)
{
	glClearColor(r, g , b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void gl_error_check(void)
{
	int err;
	if ( (err=glGetError()) )
		ABORT(3, "GL Error %d", err);
}

void tex_set(Texture *self, void * pixels )
{
	if (!self->id) {// create a new texture
		glGenTextures(1, &self->id);
		DEBUG("Create texture %d", self->id);
		glBindTexture(GL_TEXTURE_2D, self->id);
		glTexImage2D(GL_TEXTURE_2D, 0, self->internal_format, self->w, self->h, 0, self->format, self->type,  pixels);
		gl_error_check();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else {
		glBindTexture(GL_TEXTURE_2D, self->id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, self->w, self->h, self->format, self->type,  pixels);
		
		
	}//~ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
		//~ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	
}

void tex_fini(Texture *self)
{
	if (self->id) {
		DEBUG("Unbind texture %d", self->id);
		glDeleteTextures(1, &self->id);
	}
	self->id = 0;
}

void tex_on_exit(int status, void *arg)
{
	tex_fini((Texture*)arg);
}


int glsl_check(
	void (*get_param)(GLuint,  GLenum,  GLint *),
	void (*get_log)(GLuint,  GLsizei,  GLsizei *,  GLchar *),
	GLuint prog,
	GLuint param)
{
	GLint status;
	get_param(prog, param, &status);
	if (!status) {
		GLint len = 0;
		get_param(prog, GL_INFO_LOG_LENGTH, &len);
		if ( len > 0 ) {
			char* log = alloca(sizeof(char) * len);
			get_log(prog, len, NULL, log);
			printf("Compile Error : %d : %s\n", prog, log);
		}
		return -1;
	}
	return 0;
}

void shader_fini(Shader *self)
{
	if (self->id){
		DEBUG("Program deleted %d", self->id);
		glDeleteProgram(self->id);
	}
	self->id = 0;
}

void shader_on_exit(int status, void *arg)
{
	shader_fini((Shader*)arg);
}

int shader_init(Shader *self, const char *vert_src, const char *frag_src, char *args[])
{
	self->id = 0;
	self->argc = -1;
	while (args[++self->argc]);
	
	//~ Shader *prog = malloc(sizeof(Shader) + sizeof(GLuint)*argc);
	//~ prog->argc = argc;
	ASSERT (self->argc < 16);

	GLuint vert_shader, frag_shader;
	// Compile Vertex Shader
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_src, NULL);
	glCompileShader(vert_shader);
	if(glsl_check(glGetShaderiv,  glGetShaderInfoLog, vert_shader, GL_COMPILE_STATUS))
		return 0;
	
	// Compile Fragment Shader
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_src, NULL);
	glCompileShader(frag_shader);
	if(glsl_check(glGetShaderiv,  glGetShaderInfoLog, frag_shader, GL_COMPILE_STATUS)) {
		glDeleteShader(vert_shader);
		return 0;
	}
	
	// Link the program
	self->id = glCreateProgram();
	glAttachShader(self->id, vert_shader);
	glAttachShader(self->id, frag_shader);
	glLinkProgram(self->id);
	if(glsl_check(glGetProgramiv,  glGetProgramInfoLog, self->id, GL_LINK_STATUS)) {
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);
		shader_fini(self);
		return 0;
	}	
	// figure out program locations from arguments
	for (int a =0; a < self->argc; ++a) {
		switch (args[a][0]) {
			case 'a':
				self->args[a] = glGetAttribLocation(self->id, args[a]);
				//~ glEnableVertexAttribArray(prog->args[a]);
			break;
			case 'u': self->args[a] = glGetUniformLocation(self->id, args[a]); break;
			default: printf("WARNING: Must be attribute or uniform: %s",args[a]); break;
		}
	}
	
	glDeleteShader(vert_shader); // flagged for deletion when program is deleted
	glDeleteShader(frag_shader); // flagged for deletion when program is deleted
	//~ glValidateProgram(prog->id);
	//~ if(glsl_check(glGetProgramiv,  glGetProgramInfoLog, prog->id, GL_VALIDATE_STATUS))
		//~ return NULL;

	DEBUG("Program compiled %d", self->id);
	return 1;
}


int framebuffer_init(FrameBuffer *self, int w, int h)
{
	self->w = w;
	self->h = h;
	glGenFramebuffers(1, &self->fbid);
	glBindFramebuffer(GL_FRAMEBUFFER, self->fbid);
	glGenTextures(1, &self->txid);
	glBindTexture(GL_TEXTURE_2D, self->txid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, self->w, self->h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//~ printf("glTexImage2D : %d\n", glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->txid, 0);
	printf("BG Framebuffer Ready %d (%d)\n", glCheckFramebufferStatus(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE); 
	glClearColor(0.498, 0.624 , 0.682, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // put the default framebuffer back
	return 1;
}

void framebuffer_fini(FrameBuffer *self)
{
	if (self->txid) {
		DEBUG("Delete fb texture %d", self->txid);
		glDeleteTextures(1, &self->txid);
	}
	if (self->fbid) {
		DEBUG("Delete fb %d", self->fbid);
		glDeleteFramebuffers(1, &self->fbid);
	}
	
}

void framebuffer_on_exit(int status, void *arg)
{
	framebuffer_fini((FrameBuffer*)arg);
}

#endif
#endif
