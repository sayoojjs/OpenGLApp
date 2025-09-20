#include "Shader.h"
#include <GLFW/glfw3.h>



Shader::Shader()
{
	shaderID = 0;
	uniformProjection = 0;
	uniformModel = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) 
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
      shaderID = glCreateProgram();

	if (!shaderID)
	{
		printf("Error creating shader\n");
		glfwTerminate();
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);


	//Picking up error 
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error Linking program: '%s'\n", eLog);
		return;

	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error Linking program: '%s'\n", eLog);
		return;

	}

	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformColor = glGetUniformLocation(shaderID, "inColor");
	uniformProjection = glGetUniformLocation(shaderID, "projection");
}

void  Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum ShaderType)
{

}

Shader::~Shader()
{

}