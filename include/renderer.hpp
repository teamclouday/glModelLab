#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "model.hpp"

class Renderer
{
private:
    ImVec4 clearColor;
    Model *myModel;
public:
    Renderer(ImVec4 clear_color);
    ~Renderer();
    void startFrame();
    void render();
    void setUpImGui();
};