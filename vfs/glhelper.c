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

int shader_init(Shader *self, const char *vert_src, const char *frag_src, char *args[]);
void shader_fini(Shader *self);
void shader_on_exit(int status, void *arg);

void clear(GLfloat r, GLfloat g, GLfloat b);
void tex_set(Texture *self, void * pixels);
void error_check(void);

#if __INCLUDE_LEVEL__ == 0

#include <stdio.h>
#include <malloc.h>
#include "util.c"

void clear(GLfloat r, GLfloat g, GLfloat b)
{
	glClearColor(r, g , b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void error_check(void)
{
	int err;
	if ( (err=glGetError()) )
		ABORT(3, "GL Error %d", err);
}

void tex_set(Texture *self, void * pixels )
{
	if (!self->id) {// create a new texture
		glGenTextures(1, &self->id);
		glBindTexture(GL_TEXTURE_2D, self->id);
		glTexImage2D(GL_TEXTURE_2D, 0, self->internal_format, self->w, self->h, 0, self->format, self->type,  pixels);
		int err;
		if ( (err=glGetError()) )
			ABORT(20, "glTexImage2d", "Error", err);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else {
		glBindTexture(GL_TEXTURE_2D, self->id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, self->w, self->h, self->format, self->type,  pixels);
		
		
	}//~ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
		//~ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	
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





#endif
#endif
