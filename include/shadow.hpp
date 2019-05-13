#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "shader.hpp"

class ShadowMap
{
private:
    GLuint fbo, shadowMap;
    Shader *myShader;
public:
    ShadowMap();
    ~ShadowMap();
    void update(int screenW, int screenH);
};