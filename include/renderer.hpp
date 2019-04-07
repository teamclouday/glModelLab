#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <dirent.h>
#include <string>
#include <vector>
#include <sstream>

#include "model.hpp"
#include "shader.hpp"
#include "camera.hpp"

class Renderer
{
private:
    ImVec4 clearColor;
    bool refreshAll;
    bool isModelOn;

    Model *myModel;
    Shader *myShader;

    std::vector<int> modelIdx;
    std::vector<int> shaderIdx;
    std::vector<std::string> model_list;
    std::vector<std::string> shader_list;

    void loadModelLists();
    void loadShaderLists();
    void refresh();
    void setUpImGui();
public:
    Renderer(ImVec4 clear_color);
    ~Renderer();
    void startFrame();
    void render();
};