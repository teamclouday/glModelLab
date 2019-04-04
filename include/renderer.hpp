#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

class Renderer
{
private:
    ImVec4 clearColor;
public:
    Renderer(ImVec4 clear_color);
    void startFrame();
    void render();
    void setUpImGui();
};