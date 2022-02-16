#pragma once
#include <glad/glad.h>

struct Shader
{

	GLuint id = 0;

	bool loadShaderProgramFromFile(const char* vertexShader, const char* fragmentShader);
	
	bool loadShaderProgramFromMemory(const char *vertexShader, const char *fragmentShader);
	
	bool loadShaderProgramFromFile(const char* vertexShader,
		const char* geometryShader, const char* fragmentShader);


	void bind();

	void clear();
};

GLint getUniform(GLuint id, const char* name);