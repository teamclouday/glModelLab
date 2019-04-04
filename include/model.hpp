#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>

#include <vector>
#include <string>
#include <dirent.h>

class Model
{
private:
    std::vector<std::string> model_path;
    int modelIdx;
public:
    Model();
    void loadList();
    void displayCheckList();
};