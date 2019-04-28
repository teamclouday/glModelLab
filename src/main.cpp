#include "basic.hpp"

// global variables
SDL_Window *myWindow = nullptr;
SDL_GLContext glContext = NULL;
ImGuiIO* io = nullptr;
Renderer *myRenderer = nullptr;
Camera *camera = nullptr;
std::vector<bool> keys(256, false);

int main(int argc, char *argv[])
{
    initAll();

    camera = new Camera(glm::vec3(0.0f, 1.0f, 5.0f));
    myRenderer = new Renderer(ImVec4(0.4f, 0.5f, 0.4f, 1.0f));

    Uint32 prev = SDL_GetTicks();
    Uint32 now = SDL_GetTicks();

    GLuint timeBuffer;
    glCreateBuffers(1, &timeBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, timeBuffer);
    glBufferStorage(GL_UNIFORM_BUFFER, 2*sizeof(GLfloat), NULL, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, timeBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();
        myRenderer->startFrame();

        float tc = (float)(SDL_GetTicks() / 1000.0f);
        float varSin = sin(tc)*0.4f + 0.5f;
        float varCos = cos(tc)*0.4f + 0.5f;
        glNamedBufferSubData(timeBuffer, 0, sizeof(GLfloat), &varSin);
        glNamedBufferSubData(timeBuffer, sizeof(GLfloat), sizeof(GLfloat), &varCos);

        myRenderer->render();
        timer(&now, &prev);
    }

    destroyAll();
    return 0;
}