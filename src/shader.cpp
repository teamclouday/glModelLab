#include "shader.hpp"

Shader::Shader(const std::string vertname, const std::string fragname)
{
    // load context of vertex shader file
    std::ifstream file;
    file.open(("./Shaders/" + vertname).c_str());
    if(!file.is_open())
    {
        printf("Cannot load shader file source: %s\n", vertname.c_str());
        this->programID = 0;
        return;
    }
    std::stringstream sstr;
    sstr << file.rdbuf();
    std::string vertsourceS = sstr.str();
    const char *vertsource = vertsourceS.c_str();
    file.close();
    file.clear();
    sstr.str("");

    // load context of fragment shader file
    file.open(("./Shaders/" + fragname).c_str());
    if(!file.is_open())
    {
        printf("Cannot load shader file source: %s\n", fragname.c_str());
        this->programID = 0;
        return;
    }
    sstr << file.rdbuf();
    std::string fragsourceS = sstr.str();
    const char *fragsource = fragsourceS.c_str();
    file.close();

    // create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertsource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetShaderInfoLog(vertexShader, infoLen, NULL, &infoLog[0]);
        printf("Vertex Shader %s failed to compile!\nInfo Log: %s\n", vertname.c_str(), &infoLog[0]);
        this->programID = 0;
        return;
    }

    // create fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragsource, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetShaderInfoLog(fragShader, infoLen, NULL, &infoLog[0]);
        printf("Fragment shader %s failed to compile!\nInfo Log: %s\n", fragname.c_str(), &infoLog[0]);
        this->programID = 0;
        return;
    }

    // create program
    this->programID = glCreateProgram();

    glAttachShader(this->programID, vertexShader);
    glAttachShader(this->programID, fragShader);
    glLinkProgram(this->programID);

    glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetProgramiv(this->programID, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetProgramInfoLog(this->programID, infoLen, NULL, &infoLog[0]);
        printf("Failed to link shaders into program!\nInfo Log: %s\n", &infoLog[0]);
        this->programID = 0;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
}

void Shader::use()
{
    if((this->programID))
    {
        glUseProgram(this->programID);
    }
}

bool Shader::exits()
{
    return this->programID != 0;
}

void Shader::setf1(const std::string uniformName, float x)
{
    GLint loc = glGetUniformLocation(this->programID, uniformName.c_str());
    glUniform1f(loc, x);
}

void Shader::setf3(const std::string uniformName, float x, float y, float z)
{
    GLint loc = glGetUniformLocation(this->programID, uniformName.c_str());
    glUniform3f(loc, x, y, z);
}

void Shader::setMatrix4fv(const std::string uniformName, const GLfloat *value)
{
    GLint loc = glGetUniformLocation(this->programID, uniformName.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, value);
}