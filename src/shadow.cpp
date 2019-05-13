#include "shadow.hpp"

extern SDL_Window *myWindow;

ShadowMap::ShadowMap()
{
    this->myShader = new Shader("shadow/shadow.vs", "shadow/shadow.fs");

    glCreateFramebuffers(1, &this->fbo);
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    this->update(w, h);
}

ShadowMap::~ShadowMap()
{
    delete this->myShader;
    glDeleteTextures(1, &this->shadowMap);
    glDeleteFramebuffers(1, &this->fbo);
}

void ShadowMap::update(int screenW, int screenH)
{
    if(this->shadowMap != 0)
        glDeleteTextures(1, &this->shadowMap);
    glCreateTextures(GL_TEXTURE_2D, 1, &this->shadowMap);
    glBindTexture(GL_TEXTURE_2D, this->shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenW, screenH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
        printf("Framebuffer Error: 0x%x\n", status);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}