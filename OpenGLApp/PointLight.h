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

    void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
        GLfloat diffuseIntensityLocation, GLfloat positionLocation,
        GLfloat constantLocation, GLfloat linearLocaction, GLfloat exponentLocation);

    ~PointLight();

private:

    // direction 
    glm::vec3 position; 

    // ax^2 + bx + c 
    GLfloat constant, linear, exponent;

   
};

