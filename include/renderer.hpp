#pragma once

#include "imgui.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>

#include <string>
#include <sstream>

#ifdef _WIN32
#include <ShellApi.h>
#endif

struct RENDER_CONFIG
{
    ImVec4 menu_background_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImVec4 menu_text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float window_alpha = 0.8f;
    bool line_mode = false;
    int window_w = 100;
    int window_h = 100;
    int modelID = 0;
    int shaderID = 0;
};

struct MENU_CONFIG
{
    bool displayModels = false;
    bool displayShaders = false;
    bool pointLights = false;
    bool directLight = false;
    bool torchLight = false;
    bool displayColors = false;
    bool displayAbout = false;
};

class Renderer
{
private:
    RENDER_CONFIG *myRenderConfig;
    MENU_CONFIG *myMenuConfig;

public:
    Renderer();
    ~Renderer();

    void renderScene();

private:
    void renderMenu();
};