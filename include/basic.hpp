#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "myerr.hpp"

#define WINDOW_TITLE "Lab"
#define WINDOW_WIDTH  1366
#define WINDOW_HEIGHT 768

// functions
void initAll(const std::string title=WINDOW_TITLE, int width=WINDOW_WIDTH, int height=WINDOW_HEIGHT);
void startLoop();
bool pollEvents();
void destroyAll();