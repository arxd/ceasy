#ifndef GLHELPER_H
#define GLHELPER_H

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

typedef struct s_Shader Shader;
struct s_Shader {
	GLuint id;
	GLuint argc;
	GLuint args[];
};

Shader *shader_new(const char *vert_src, const char *frag_src, char *args[]);
void clear(GLfloat r, GLfloat g, GLfloat b);

#if __INCLUDE_LEVEL__ == 0

#include <stdio.h>
#include <malloc.h>

void clear(GLfloat r, GLfloat g, GLfloat b)
{
	glClearColor(r, g , b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
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
	
	return prog;
}

#endif
#endif
