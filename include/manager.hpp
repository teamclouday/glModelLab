#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <iostream>

#include "camera.hpp"

enum GlobalVariables
{
    GLOB_WINDOW,
    GLOB_CONTEXT,
    GLOB_CAMERA,
};

class GlobalManager
{
public:
    SDL_Window *myWindow;
    SDL_GLContext myContext;
    Camera *myCamera;

    GlobalManager();
    ~GlobalManager();

    bool store(GlobalVariables var, void* pointer);
    void free(GlobalVariables var);
};