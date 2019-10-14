#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <assimp/version.h>

#include <iostream>

#include "manager.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "renderer.hpp"

#define WINDOW_TITLE            "Model Lab"
#define WINDOW_DEFAULT_WIDTH    800
#define WINDOW_DEFAULT_HEIGHT   600
#define FPS                     60

bool initEnv();
void quitAll();
bool pollEvents();
void fpsControl(Uint32* tNow, Uint32 *tPrev);

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam)
{
    std::stringstream sstr;
    sstr << "GL CALLBACK ";
    bool cannot_skip = true;
    switch(type)
    {
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            sstr << "(deprecated behavior)";
            break;
        case GL_DEBUG_TYPE_ERROR:
            sstr << "(error)";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            sstr << "(performance)";
            break;
        default:
            sstr << "(other)";
            cannot_skip = false;
            break;
    }
    sstr << ":" << message;
    if(cannot_skip)
        std::cerr << sstr.str() << std::endl;
}