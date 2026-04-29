#include "Material.h"

Material::Material()
{
    specularIntensity = 0.0f;
    Shininess = 0.0f;
}

Material::Material(GLfloat sIntensity, GLfloat shine)
{
    specularIntensity = sIntensity;
    Shininess = shine;
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform1f(shininessLocation, Shininess);
}

Material::~Material()
{

}