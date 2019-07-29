#pragma once
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>

class Shader
{
public:
    GLuint programID;
    Shader(const std::string vertName, const std::string fragName);
    ~Shader();
    void use();
    void disuse();
};