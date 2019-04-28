#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "myerr.hpp"
#include "renderer.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "camera.hpp"

#define WINDOW_TITLE "Lab"
#define WINDOW_WIDTH  1366
#define WINDOW_HEIGHT 768

#define FPS 60

// functions
void initAll(const std::string title=WINDOW_TITLE, int width=WINDOW_WIDTH, int height=WINDOW_HEIGHT);
void timer(Uint32 *now, Uint32 *prev);
bool pollEvents();
void destroyAll();