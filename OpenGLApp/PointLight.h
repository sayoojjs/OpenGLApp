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

    void  SetPosition(float x, float y, float z) 
    { 
             position = glm::vec3(x, y, z); 
    }
    void  SetAmbientIntensity(float i) 
    { 
           ambientIntensity = i;
    }
    void  SetDiffuseIntensity(float i) 
    { 
        diffuseIntensity = i; 
    }
    glm::vec3 GetPosition()         const 
    { 
        return position; 
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


    ~PointLight();

private:

    // direction 
    glm::vec3 position; 

    // ax^2 + bx + c 
    GLfloat constant, linear, exponent;

   
};

