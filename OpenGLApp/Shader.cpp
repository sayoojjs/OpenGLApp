#include "Shader.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>


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
 
void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream filestream(fileLocation, std::ios::in);

	if (!filestream.is_open()) 
	{
		printf("Failed gto read %s! File doesnt exist", fileLocation);
		return "";
	}

	std::string line = "";
	while (!filestream.eof())
	{
		std::getline(filestream, line);
		content.append(line + "\n");
	}

	filestream.close();
	return content;
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

GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}
GLuint Shader::GetModelLocation()
{
	return uniformModel;
}



void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	 }
	uniformModel = 0;
	uniformProjection = 0;

}

void  Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum ShaderType)
{

	GLuint theShader = glCreateShader(ShaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	//Error checking / pickup errors
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling  the shader %d: '%s'\n", ShaderType, eLog);
		return;

	}

	glAttachShader(theProgram, theShader);

}

Shader::~Shader()
{
	ClearShader();
}