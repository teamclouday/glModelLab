#pragma once

#include "imgui.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>

#ifdef __unix__
#include <dirent.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include "model.hpp"
#include "shader.hpp"
#include "lights.hpp"
#include "shadow.hpp"

struct RENDER_CONFIG
{
    ImVec4 menu_background_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImVec4 menu_text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float window_alpha = 0.8f;
    float sight_near = 0.1f;
    float sight_far = 1000.0f;
    bool line_mode = false;
    int window_w = 100;
    int window_h = 100;
    int modelID = 0;
    int shaderID = 0;
    Model *model = nullptr;
    Shader *shader = nullptr;
    Lights *lights = nullptr;
    ShadowMap *shadow = nullptr;
    glm::vec3 model_pos = glm::vec3(0.0f);
};

struct MENU_CONFIG
{
    bool displayModels = false;
    bool displayShaders = false;
    bool pointLight = false;
    bool directLight = false;
    bool torchLight = false;
    bool displayColors = false;
    bool displayOther = false;
    bool displayAbout = false;
    bool displaySystem = false;
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
    void loadModel();
    void loadShader();
};