#include "basic.hpp"

// global variables
SDL_Window *myWindow = nullptr;
SDL_GLContext glContext = NULL;
ImGuiIO* io = nullptr;
Renderer *myRenderer = nullptr;
Camera *camera = nullptr;

int main(int argc, char *argv[])
{
    initAll();

    camera = new Camera(glm::vec3(0.0f, 1.0f, 5.0f));
    myRenderer = new Renderer(ImVec4(0.4f, 0.5f, 0.4f, 1.0f));

    Uint32 prev = SDL_GetTicks();
    Uint32 now = SDL_GetTicks();

    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();
        myRenderer->startFrame();

        myRenderer->render();
        timer(&now, &prev);
    }

    destroyAll();
    return 0;
}