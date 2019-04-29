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

    bool displayModels;
    bool displayShaders;
    bool displayInfo;

    bool displayConfigBack;
    bool displayConfigModel;
    bool displayConfigLight;

    float deltaTime;
    float lastFrame;

    Model *myModel;
    Shader *myShader;

    std::vector<int> modelIdx;
    std::vector<int> shaderIdx;
    std::vector<std::string> model_list;
    std::vector<std::string> shader_list;

    std::vector<SourceLight*> sourceLights;

    void loadModelLists();
    void loadShaderLists();
    void refresh();
    void setUpImGui();
public:
    float zoomLevel;
    int xpos, ypos;
    bool isFocused;

    Renderer(ImVec4 clear_color);
    ~Renderer();
    
    void startFrame();
    void render();
    void handleMouse(bool isfocused);
};