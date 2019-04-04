// implement some basic functions
#include "basic.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;

void initAll(const std::string title, int width, int height)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to init SDL2\nSDL Error: %s\n", SDL_GetError());
        exit(ERROR_SDL_INIT);
    }
    // create window
    myWindow = SDL_CreateWindow(title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                width,
                                height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    // create context from window
    glContext = SDL_GL_CreateContext(myWindow);
    if(!glContext)
    {
        printf("Failed to create context\nSDL Error: %s\n", SDL_GetError());
        exit(ERROR_SDL_GL_CONTEXT);
    }
    // set up OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        printf("Failed to init GLEW\n");
        exit(ERROR_GLEW_INIT);
    }
    glViewport(0, 0, width, height);
}

void startLoop()
{
    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();
        glClearColor(0.01f, 0.5f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SDL_GL_SwapWindow(myWindow);
    }
}

bool pollEvents()
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        if(e.type == SDL_QUIT)
            return true;
        else if(e.type == SDL_KEYDOWN)
        {
            switch(e.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    return true;
            }
        }
    }
    return false;
}

void destroyAll()
{
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(myWindow);
    SDL_Quit();
}