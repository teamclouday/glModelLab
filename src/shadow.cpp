#include "shadow.hpp"

ShadowMap::ShadowMap()
{
    this->lightPos = glm::vec3(1.0);

    this->createShader();

    glGenFramebuffers(1, &this->FBO);

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, this->depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap()
{
    glDeleteTextures(1, &this->depthTex);
    glDeleteFramebuffers(1, &this->FBO);
    glDeleteProgram(this->programID);
}

void ShadowMap::createShader()
{
    std::stringstream sstr;
    sstr << "#version 330 core\n"
         << "layout (location = 0) in vec3 position;\n"
         << "uniform mat4 lightProj;\n"
         << "uniform mat4 model;\n"
         << "void main()\n"
         << "{\n"
         << "gl_Position = lightProj * model * vec4(position, 1.0);\n"
         << "}\n";
    std::string vertShaderStr = sstr.str();
    const char *vertShaderSrc = vertShaderStr.c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    sstr.str(std::string());
    sstr.clear();
    sstr << "#version 330 core\n"
         << "out vec4 FragColor;\n"
         << "in vec2 TexCoords;\n"
         << "uniform sampler2D depthMap;\n"
         << "void main()\n"
         << "{\n"
         << "float depthVal = texture(depthMap, TexCoords).r;\n"
         << "FragColor = vec4(vec3(depthVal), 1.0);\n"
         << "}\n";
    std::string fragShaderStr = sstr.str();
    const char *fragShaderSrc = fragShaderStr.c_str();
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    this->programID = glCreateProgram();
    glAttachShader(this->programID, fragShader);
    glAttachShader(this->programID, vertShader);
    glLinkProgram(this->programID);

    glDeleteShader(fragShader);
    glDeleteShader(vertShader);
}

void ShadowMap::bind(glm::mat4& model)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glUseProgram(this->programID);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    glm::mat4 lightView = glm::lookAt(this->lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightMat = lightProj * lightView;
    glUniformMatrix4fv(glGetUniformLocation(this->programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(this->programID, "lightProj"), 1, GL_FALSE, glm::value_ptr(lightMat));
}

void ShadowMap::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void ShadowMap::texBind()
{
    glBindTexture(GL_TEXTURE_2D, this->depthTex);
}

void ShadowMap::texUnbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}