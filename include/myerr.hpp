// an error list
#pragma once
#include <GL/glew.h>

#include <string>
#include <iostream>

#define ERROR_SDL_INIT          1
#define ERROR_SDL_WINDOW        2
#define ERROR_SDL_GL_CONTEXT    3
#define ERROR_GLEW_INIT         4
#define ERROR_CONTROLLER        5

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam);

void _check_gl_error(const char *file, int line);
#define check_gl_error() _check_gl_error(__FILE__, __LINE__)