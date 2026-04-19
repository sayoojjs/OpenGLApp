#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::initialise()
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

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("Window Creation failed!");
		glfwTerminate();
		return 1;
	}
	//Get Buffer size info
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);


	//Set context for GLEW to use, use the current or alternative window to draw to opengl fucntions
	glfwMakeContextCurrent(mainWindow);

	// Handle key + mouse input

	CreateCallbacks();

	//Disabling the mouse cursor
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Allow mordern extention features 
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initalization failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//To enable depth buffer - Z buffer
	glEnable(GL_DEPTH_TEST);

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));


	//Closing the Window by pressing ESCAPE key
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			
		}
	}
}

//Initating the calls 
void Window::CreateCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;

	// invert the coordinates by flipping the subtraction of eg: yPos - theWindow->yChange
	theWindow->yChange = theWindow->lastY - yPos;

	// Assigning the value
	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

	printf("x:%.6f, y:%.6f\n", theWindow->xChange, theWindow->yChange);
}

GLfloat Window::getXchange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYchange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}