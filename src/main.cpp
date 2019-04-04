#include "basic.hpp"

// global variables
SDL_Window *myWindow = nullptr;
SDL_GLContext glContext = NULL;
ImGuiIO* io = nullptr;

int main(int argc, char *argv[])
{
    initAll();
    startLoop();
    destroyAll();
    return 0;
}