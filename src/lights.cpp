#include "lights.hpp"

Lights::Lights()
{

}

Lights::~Lights()
{
    for(unsigned i = 0; i < this->directL.size(); i++)
        delete this->directL[i];
    for(unsigned i = 0; i < this->pointL.size(); i++)
        delete this->pointL[i];
    for(unsigned i = 0; i < this->spotL.size(); i++)
        delete this->spotL[i];
}

void Lights::loadShader()
{
    // load source
    std::stringstream sstr;
    sstr << "#version 330 core\n"
         << "const vec3[] pos = vec3[](vec3( 1.0, -1.0, 1.0),\n"
         << "                          vec3( 1.0, -1.0, 1.0),\n"
         << "                          vec3(-1.0,  1.0, 1.0),\n"
         << "                          vec3( 1.0,  1.0, 1.0));\n"
         << "mat4 mvp;\n"
         << "void main(){\n"
         << "gl_Position = mvp * vec4(pos, 1.0);\n"
         << "}\n";
}

void Lights::addDirectLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color)
{
    DirectLight *newLight = new DirectLight;
    newLight->color = color;
    newLight->direction = direction;
    newLight->position = position;
    this->directL.push_back(newLight);
}

void Lights::addPointLight(const glm::vec3& position, const glm::vec4& color)
{
    PointLight *newLight = new PointLight;
    newLight->color = color;
    newLight->position = color;
    this->pointL.push_back(newLight);
}

void Lights::addSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec4& color)
{
    SpotLight *newLight = new SpotLight;
    newLight->position = position;
    newLight->direction = direction;
    newLight->cutoff = cutoff;
    newLight->color = color;
    this->spotL.push_back(newLight);
}