#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <string>
#include <vector>

#define SHADOW_WIDTH    1024
#define SHADOW_HEIGHT   1024

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap();

    void bind(glm::mat4& model, glm::vec3& lightPos, glm::vec3& lightDir);
    void unbind();

    void texBind();

    GLuint programID;
    std::vector<int> pointer;
    glm::mat4 lightMat;

private:
    GLuint FBO;
    GLuint depthTex;

    void createShader();
};