#pragma once

#include <GL\glew.h>
class Mesh
{
public:
	Mesh();
                //refernce, indices, number of vertices
	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numofVertices, unsigned int numberofIndices);
	void RenderMesh();
	void ClearMesh();

	~Mesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

