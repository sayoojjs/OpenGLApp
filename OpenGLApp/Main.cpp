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
#include "Shader.h"
#include "Window.h"

//Window dimentions a
const float toRadians = 3.14159265359 / 180.0f;

Window mainWindow;
std::vector<Mesh*>  meshlist;
std::vector<Shader> shaderList;


float r = 1.0f;
float g = 0.0f;
float b = 0.0f;
int colorState = 0;





/* ---------------------GLSL SHADER PROGRAM---------------------*/


// Vertex Shader MAKE CLEAR UNDERSTANDING
static const char* vShader = "Shaders/shader.vert";                                                    


//Framgment shader
static const char* fShader = "Shaders/shader.frag";



/*----------------------SHADER PROGRAM ENDS--------------------*/









/*--------------------------------------------------------------*/

//Create triangle function
void CreateObjects()
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

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);
	meshlist.push_back(obj2);

}

/*--------------------------------------------------------------*/


/*--------------------------------------------------------------*/

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

/*---------------------------- MAIN FUNCTION --------------------------------------*/


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.initialise();

	CreateObjects();
	CreateShaders();

	GLuint uniformProjection = 0, uniformModel = 0;
	//Varible for projection
	glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);


    //Loop until the window close
	while (!mainWindow.getShouldClose())
	{
	    // Get + Handle User Input
		glfwPollEvents();

		// Clear window, R G B A (0 min 1 is max), normalised RGB 255 to 0 and 1
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		                              //Combined the depth buffer too
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                             
		//glUseProgram(shader);

		//glUniform4f(uniformColor, r, g, b, 1.0f);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();

		//Model matrix (Materix 4x4, indentity materix)
		glm::mat4 model(1.0f);

		//Apply triOffset value to on the top left corner of identity matrix, if you want to make diagonal translation you could change the  from here  glm::vec3(X, Y, Z) so no need to change through shader
		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		//model = glm::rotate(model, currentAngle, glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

		//LEGACY TRANSFORM WITH UNIFORM VARIABLE
		//glUniform1f(uniformXMove, triOffset);

		//NEW TRANSFORM BASED ON MATRIX 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		meshlist[0]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		//model = glm::rotate(model, currentAngle, glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

		//LEGACY TRANSFORM WITH UNIFORM VARIABLE
		//glUniform1f(uniformXMove, triOffset);

		//NEW TRANSFORM BASED ON MATRIX 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshlist[1]->RenderMesh();

		glUseProgram(0);

		mainWindow.swapbuffers();

	}

	return 0;

}


/*--------------------------------------------------------------*/