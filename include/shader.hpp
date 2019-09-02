#pragma once
#include <GL/glew.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

class Shader
{
public:
    Shader();
    ~Shader();
    bool add(std::string path, GLenum shaderType);
    bool compile();
    void use();
    void disuse();
    GLuint programID;
private:
    std::vector<GLuint> shaderFiles;
};