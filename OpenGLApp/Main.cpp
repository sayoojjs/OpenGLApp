/*-----------OPENGL LEARNING APPLICATION-----------------------*/

//STANDARD libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <vector>

//GLEW AND GLFW libs
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//GLM Libraries 
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"

//Window dimentions 
const GLint WIDTH = 1800, HEIGHT = 1600;
const float toRadians = 3.14159265359 / 180.0f;

std::vector<Mesh*>  meshlist;


GLuint Shader, uniformModel, uniformColor, uniformProjection;

bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.005f;

float currentAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float MaxSize = 0.7f;
float MinSize = 0.1f;

float r = 1.0f;
float g = 0.0f;
float b = 0.0f;
int colorState = 0;





/* ---------------------GLSL SHADER PROGRAM---------------------*/


// Vertex Shader MAKE CLEAR UNDERSTANDING
static const char* vShader = "                   \n\
#version 330                                     \n\
                                                 \n\
layout (location = 0) in vec3 pos;               \n\
                                                 \n\
out vec4 vCol;                                   \n\
                                                 \n\
uniform mat4 model;                              \n\
uniform mat4 projection;                         \n\
                                                 \n\
void main()                                      \n\
{                                                \n\
	gl_Position = projection * model * vec4(pos, 1.0);        \n\
    vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);   \n\
}";                                                    


//Framgment shader
static const char* fShader = "                   \n\
#version 330                                     \n\
                                                 \n\
in vec4 vCol;                                    \n\
                                                 \n\
                                                 \n\
out vec4 colour;                                 \n\
uniform vec4  inColor;                           \n\
                                                 \n\
void main()                                      \n\
{                                                \n\
	colour = vCol;                               \n\
}";



/*----------------------SHADER PROGRAM ENDS--------------------*/









/*--------------------------------------------------------------*/

//Create triangle function
void CreateTriangle()
{
	//Index for which point place in which order
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 1, 0,
		0, 1, 2

	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f 
	
	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshlist.push_back(obj1);


}


/*--------------------------------------------------------------*/









/*--------------------------------------------------------------*/



//For adding the shader to the program
void AddShader(GLuint theProgram, const char* shaderCode, GLenum ShaderType)
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


/*--------------------------------------------------------------*/









/*--------------------------------------------------------------*/


//For compiling shaders 
void CompileShaders()
{

	Shader = glCreateProgram();

	if (!Shader) 
	{
		printf("Error creating shader\n");
		glfwTerminate();
		return;
	 }

	AddShader(Shader, vShader,GL_VERTEX_SHADER);
	AddShader(Shader, fShader, GL_FRAGMENT_SHADER);


	//Picking up error 
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(Shader);
	glGetProgramiv(Shader, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(Shader, sizeof(eLog), NULL, eLog);
		printf("Error Linking program: '%s'\n", eLog);
		return;

	}

	glValidateProgram(Shader);
	glGetProgramiv(Shader, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(Shader, sizeof(eLog), NULL, eLog);
		printf("Error Linking program: '%s'\n", eLog);
		return;

	}

	uniformModel = glGetUniformLocation(Shader, "model");
	uniformColor = glGetUniformLocation(Shader, "inColor");
	uniformProjection = glGetUniformLocation(Shader, "projection");

}

/*--------------------------------------------------------------*/



/*---------------------------- MAIN FUNCTION --------------------------------------*/


int main()
{

	//intialize GLFW
	if (!glfwInit())
	{
		printf("GLFW intialization failed!");

		// this will close anything GLFW intialized 
		glfwTerminate();

	    //standard number to return  
		return 1;
	}

	//Setup GLFW Window properties
	//OpenGL Version 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Core profile = No Backwards compatible
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Allow forward compatiablity
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	GLFWwindow* mainwindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainwindow)
	{
		printf("Window Creation failed!");
		glfwTerminate();
		return 1;
	}
	//Get Buffer size info
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainwindow, &bufferWidth, &bufferHeight);


	//Set context for GLEW to use, use the current or alternative window to draw to opengl fucntions
	glfwMakeContextCurrent(mainwindow);

	//Allow mordern extention features 
	glewExperimental = GL_TRUE;
    
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initalization failed!");
		glfwDestroyWindow(mainwindow);
		glfwTerminate();
		return 1;
	}
     
	//To enable depth buffer - Z buffer
	glEnable(GL_DEPTH_TEST);

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferWidth);

	CreateTriangle();
	CompileShaders();

	//Varible for projection
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);


    //Loop until the window close
	while (!glfwWindowShouldClose(mainwindow))
	{
		//Get + Handle user input input events, move and resize window things
		glfwPollEvents();
		if (direction)
		{
			triOffset += triIncrement;
		}
		else
		{
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxoffset)
		{
			direction = !direction;

			// Cycle through colors each time edge is hit
			colorState = (colorState + 1) % 4; // 4 colors
			switch (colorState)
			{
			case 0: r = 1.0f; g = 0.0f; b = 0.0f; break; // Red
			case 1: r = 0.0f; g = 1.0f; b = 0.0f; break; // Green
			case 2: r = 0.0f; g = 0.0f; b = 1.0f; break; // Blue
			case 3: r = 1.0f; g = 1.0f; b = 0.0f; break; // Yellow
			}
		}

		//For constant rotation

		currentAngle += 0.01f;

		// if current angle went beyond 360, (it does because of the loop can beyond 360 to 361) , and to keep the value under 360
		if (currentAngle >= 360)
		{
			currentAngle -= 360;
		}

		if (sizeDirection)
		{
			curSize += 0.001f;
		}
		else {
			curSize -= 0.001f;
		}

		if (curSize >= MaxSize || curSize <= MinSize)
		{
			sizeDirection = !sizeDirection;
		}




		// Clear window, R G B A (0 min 1 is max), normalised RGB 255 to 0 and 1
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		                              //Combined the depth buffer too
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                             
		glUseProgram(Shader);

		glUniform4f(uniformColor, r, g, b, 1.0f);

		//Model matrix (Materix 4x4, indentity materix)
		glm::mat4 model;

		//Apply triOffset value to on the top left corner of identity matrix, if you want to make diagonal translation you could change the  from here  glm::vec3(X, Y, Z) so no need to change through shader
		
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));
		//model = glm::rotate(model, currentAngle, glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

		//LEGACY TRANSFORM WITH UNIFORM VARIABLE
		//glUniform1f(uniformXMove, triOffset);

		//NEW TRANSFORM BASED ON MATRIX 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		meshlist[0]->RenderMesh();


		glUseProgram(0);

		glfwSwapBuffers(mainwindow);

	}

}


/*--------------------------------------------------------------*/