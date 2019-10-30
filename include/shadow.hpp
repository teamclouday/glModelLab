#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <string>

#define SHADOW_WIDTH    1024
#define SHADOW_HEIGHT   1024

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap();

    void bind();
    void unbind();

    void texBind();
    void texUnbind();

private:
    GLuint FBO;
    GLuint depthTex;
    GLuint programID;

    void createShader();
};