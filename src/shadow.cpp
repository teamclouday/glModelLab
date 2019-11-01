#include "shadow.hpp"

ShadowMap::ShadowMap()
{
    this->pointer = {-1, -1};
    this->createShader();

    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, this->depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderC[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderC);

    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTex, 0);
    glDrawBuffer(GL_NONE);
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
         << "uniform mat4 lightMat;\n"
         << "uniform mat4 model;\n"
         << "void main()\n"
         << "{\n"
         << "gl_Position = lightMat * model * vec4(position, 1.0);\n"
         << "}\n";
    std::string vertShaderStr = sstr.str();
    const char *vertShaderSrc = vertShaderStr.c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    sstr.str(std::string());
    sstr.clear();
    sstr << "#version 330 core\n"
         << "void main()\n"
         << "{\n"
         << "// gl_FragDepth = gl_FragCoord.z;\n"
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

void ShadowMap::bind(glm::mat4& model, glm::vec3& lightPos, glm::vec3& lightDir)
{
    glUseProgram(this->programID);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 1000.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
    this->lightMat = lightProj * lightView;
    glUniformMatrix4fv(glGetUniformLocation(this->programID, "lightMat"), 1, GL_FALSE, glm::value_ptr(this->lightMat));
    glUniformMatrix4fv(glGetUniformLocation(this->programID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glCullFace(GL_FRONT);
}

void ShadowMap::unbind()
{
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void ShadowMap::texBind()
{
    glBindTexture(GL_TEXTURE_2D, this->depthTex);
}