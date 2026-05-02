#pragma once

#include "Light.h"

class DirectionalLight : 
	public Light
{
public:
	DirectionalLight();


	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
		            GLfloat aIntensity, GLfloat dIntensity,
		            GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	void SetDirection(float x, float y, float z) 
	{ 
		direction = glm::vec3(x, y, z);
	}
	void SetAmbientIntensity(float i) 
	{ 
		ambientIntensity = i;
	}
	void SetDiffuseIntensity(float i) 
	{
		diffuseIntensity = i; 
	}
	glm::vec3 GetDirection()      const 
	{ 
		return direction; 
	}
	float     GetAmbientIntensity() const 
	{
		return ambientIntensity;
	}
	float     GetDiffuseIntensity() const
	{
		return diffuseIntensity; 
	}
	void SetColour(float r, float g, float b)
	{
		colour = glm::vec3(r, g, b);
	}
	glm::vec3 GetColour() const
	{
		return colour;
	}


	~DirectionalLight();

private:
	glm::vec3 direction;

};

