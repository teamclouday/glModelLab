#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <string>
#include <iostream>
#include <vector>

#define SHADOW_WIDTH    1024
#define SHADOW_HEIGHT   1024

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap();

    void bind(glm::mat4& model);
    void unbind();

    void texBind();
    void texUnbind();

    glm::vec3 &lightPos;
    GLuint programID;

private:
    GLuint FBO;
    GLuint depthTex;

    void createShader();
};