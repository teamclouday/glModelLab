#include "shader.hpp"

Shader::Shader()
{
    
}

Shader::~Shader()
{
    glDeleteProgram(this->programID);
}

bool Shader::add(std::string path, GLenum shaderType)
{
    GLuint shader;

    std::ifstream ifs(path);
    if(!ifs.is_open())
    {
        std::cout << "Cannot find shader file: " << path << std::endl;
        return false;
    }
    std::stringstream sstr;
    sstr << ifs.rdbuf();
    std::string cont = sstr.str();
    const char *content = cont.c_str();
    ifs.close();

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &content, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetShaderInfoLog(shader, infoLen, NULL, &infoLog[0]);
        std::cout << "Shader " << path << " failed to compile!\nInfo Log: " << &infoLog[0] << std::endl;
        return false;
    }

    this->shaderFiles.push_back(shader);
    return true;
}

bool Shader::compile()
{
    this->programID = glCreateProgram();

    for(unsigned i = 0; i < this->shaderFiles.size(); i++)
    {
        glAttachShader(this->programID, this->shaderFiles[i]);
    }
    
    glLinkProgram(this->programID);

    GLint success;
    glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetProgramiv(this->programID, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetProgramInfoLog(this->programID, infoLen, NULL, &infoLog[0]);
        std::cout << "Failed to link shaders!\nInfo Log: " << &infoLog[0] << std::endl;
        return false;
    }

    for(unsigned i = 0; i < this->shaderFiles.size(); i++)
    {
        glDeleteShader(this->shaderFiles[i]);
    }
    this->shaderFiles.clear();
    return true;
}

void Shader::use()
{
    glUseProgram(this->programID);
}

void Shader::disuse()
{
    glUseProgram(0);
}