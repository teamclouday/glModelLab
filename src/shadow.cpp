#include "shadow.hpp"

ShadowMap::ShadowMap()
{
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
         << "out FragColor;\n"
         << "in vec2 TexCoords;\n"
         << "uniform sampler2D depthMap;\n"
         << "void main()\n"
         << "{\n"
         << "float depthVal = texture(depthMap, TexCoords).r;\n"
         << "FragColor = vec4(vec3(depthVal), 1.0);\n"
         << "}\n";
    std::string shaderStr = sstr.str();
    const char *shaderSrc = shaderStr.c_str();
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    if(this->programID)
        glDeleteProgram(this->programID);
    this->programID = glCreateProgram();
    glAttachShader(this->programID, shader);
    glLinkProgram(this->programID);
    glDeleteShader(shader);
}

void ShadowMap::bind()
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::texBind()
{
    glBindTexture(GL_TEXTURE_2D, this->depthTex);
}

void ShadowMap::texUnbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}