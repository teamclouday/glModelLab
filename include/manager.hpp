#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <vector>

#ifdef __unix__
#include <dirent.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include "camera.hpp"
#include "renderer.hpp"

#ifndef MY_ROOT_DIR
#define MY_ROOT_DIR "."
#endif

enum GlobalVariables
{
    GLOB_WINDOW,
    GLOB_CONTEXT,
    GLOB_CAMERA,
    GLOB_RENDERER,
};

class GlobalManager
{
public:
    SDL_Window *myWindow;
    SDL_GLContext myContext;
    Camera *myCamera;
    Renderer *myRenderer;

    std::vector<std::string> models;
    std::vector<std::string> shaders;

    GlobalManager();
    ~GlobalManager();

    bool store(GlobalVariables var, void* pointer);
    void free(GlobalVariables var);

    void update();

private:
    void load_models();
    void load_shaders();
};