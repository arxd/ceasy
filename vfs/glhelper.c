#ifndef GLHELPER_H
#define GLHELPER_H

#include <stdlib.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

typedef struct s_Shader Shader;
struct s_Shader {
	GLuint id;
	GLuint argc;
	GLuint args[];
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

Shader *shader_new(const char *vert_src, const char *frag_src, char *args[]);
void clear(GLfloat r, GLfloat g, GLfloat b);
void tex_set(Texture *self, void * pixels);

#if __INCLUDE_LEVEL__ == 0

#include <stdio.h>
#include <malloc.h>

static void gl_death(const char *title, const char *err_str, int errid)
{	
	if (title)
		printf("%s:%d: %s\n", title, errid, err_str);
	exit(errid);
}

void clear(GLfloat r, GLfloat g, GLfloat b)
{
	glClearColor(r, g , b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}


void tex_set(Texture *self, void * pixels )
{
	if (!self->id) {// create a new texture
		glGenTextures(1, &self->id);
		glBindTexture(GL_TEXTURE_2D, self->id);
		glTexImage2D(GL_TEXTURE_2D, 0, self->internal_format, self->w, self->h, 0, self->format, self->type,  pixels);
		int err;
		if ( (err=glGetError()) )
			gl_death("glTexImage2d", "Error", err);

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
			char* log = malloc(sizeof(char) * len);
			get_log(prog, len, NULL, log);
			printf("Shader Error : %d : %s", prog, log);
			return -1;
		}
	}
	return 0;
}

Shader *shader_new(const char *vert_src, const char *frag_src, char *args[])
{
	int argc = -1;
	while (args[++argc]);
		
	Shader *prog = malloc(sizeof(Shader) + sizeof(GLuint)*argc);
	prog->argc = argc;
	
	GLuint vert_shader, frag_shader;
	// Compile Vertex Shader
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_src, NULL);
	glCompileShader(vert_shader);
	if(glsl_check(glGetShaderiv,  glGetShaderInfoLog, vert_shader, GL_COMPILE_STATUS))
		return NULL;
	// Compile Fragment Shader
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_src, NULL);
	glCompileShader(frag_shader);
	if(glsl_check(glGetShaderiv,  glGetShaderInfoLog, frag_shader, GL_COMPILE_STATUS))
		return NULL;
	// Link the program
	prog->id = glCreateProgram();
	glAttachShader(prog->id, vert_shader);
	glAttachShader(prog->id, frag_shader);
	glLinkProgram(prog->id);
	if(glsl_check(glGetProgramiv,  glGetProgramInfoLog, prog->id, GL_LINK_STATUS))
		return NULL;
	
	for (int a =0; a < argc; ++a) {
		switch (args[a][0]) {
			case 'a':
				prog->args[a] = glGetAttribLocation(prog->id, args[a]);
				glEnableVertexAttribArray(prog->args[a]);
			break;
			case 'u': prog->args[a] = glGetUniformLocation(prog->id, args[a]); break;
			default: printf("must be attribute or uniform: %s",args[a]); return NULL;
		}
	}
	glValidateProgram(prog->id);
	glsl_check(glGetProgramiv,  glGetProgramInfoLog, prog->id, GL_VALIDATE_STATUS);

	return prog;
}





#endif
#endif
