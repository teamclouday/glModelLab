#pragma once

#include "imgui.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <ShellApi.h>
#endif

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void renderScene();

private:
    void renderMenu();
};