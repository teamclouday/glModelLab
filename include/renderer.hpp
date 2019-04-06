#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <dirent.h>
#include <string>
#include <vector>

#include "model.hpp"
#include "shader.hpp"

class Renderer
{
private:
    ImVec4 clearColor;

    Model *myModel;
    Shader *myShader;

    int modelIdx;
    int shaderIdx;
    std::vector<std::string> model_list;
    std::vector<std::string> shader_list;
    
    void loadModelLists();
    void loadShaderLists();
public:
    Renderer(ImVec4 clear_color);
    ~Renderer();
    void startFrame();
    void render();
    void setUpImGui();
};