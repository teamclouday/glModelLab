#include "basic.hpp"

// global variables
SDL_Window *myWindow = nullptr;
SDL_GLContext glContext = NULL;
ImGuiIO* io = nullptr;
Renderer *myRenderer = nullptr;

int main(int argc, char *argv[])
{
    initAll();

    myRenderer = new Renderer(ImVec4(0.4f, 0.5f, 0.4f, 1.0f));

    startLoop();
    destroyAll();
    return 0;
}