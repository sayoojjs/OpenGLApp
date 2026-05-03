#pragma once
#include "Light.h"
class PointLight :
    public Light
{
public:
    PointLight();
    
    PointLight(GLfloat red, GLfloat green, GLfloat blue,
          GLfloat aIntensity, GLfloat dIntensity,
         GLfloat xPos, GLfloat yPos, GLfloat zPos,
         GLfloat con, GLfloat lin, GLfloat exp);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation,
        GLuint constantLocation, GLuint linearLocaction, GLuint exponentLocation);

    ~PointLight();

protected:

    // direction 
    glm::vec3 position; 

    // ax^2 + bx + c 
    GLfloat constant, linear, exponent;

   
};

