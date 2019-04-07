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
    bool exits();
    // helper functions
    void setf1(const std::string uniformName, float x);
    void setf3(const std::string uniformName, float x, float y, float z);
    void setMatrix4fv(const std::string uniformName, const GLfloat *value);
};