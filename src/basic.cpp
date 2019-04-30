// implement some basic functions
#include "basic.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;
extern ImGuiIO* io;
extern Renderer *myRenderer;
extern Camera *camera;

void initAll(const std::string title, int width, int height)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to init SDL2\nSDL Error: %s\n", SDL_GetError());
        exit(ERROR_SDL_INIT);
    }
    // set up OpenGL
    const char* glsl_version = "#version 450";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // create window
    myWindow = SDL_CreateWindow(title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                width,
                                height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    // create context from window
    glContext = SDL_GL_CreateContext(myWindow);
    if(!glContext)
    {
        printf("Failed to create context\nSDL Error: %s\n", SDL_GetError());
        exit(ERROR_SDL_GL_CONTEXT);
    }
    SDL_GL_SetSwapInterval(1); // Enable vsync
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        printf("Failed to init GLEW\n");
        exit(ERROR_GLEW_INIT);
    }
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(MessageCallback, 0);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &(ImGui::GetIO());
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(myWindow, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
    io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io->IniFilename = NULL;
}

void timer(Uint32 *now, Uint32 *prev)
{
    *now = SDL_GetTicks();
    Uint32 delta = *now - *prev;
    if(delta < (1000 / FPS))
        SDL_Delay((Uint32)(1000 / FPS) - delta);
    *prev = SDL_GetTicks();
}

bool pollEvents()
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if(e.type == SDL_QUIT)
            return true;
        else if(e.type == SDL_KEYDOWN)
        {
            io->KeysDown[e.key.keysym.scancode] = true;
            if(!myRenderer->isFocused)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        return true;
                    case SDLK_F11:
                    {
                        bool isFullScreen = SDL_GetWindowFlags(myWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                        SDL_SetWindowFullscreen(myWindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                        if(isFullScreen)
                            SDL_SetWindowPosition(myWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                        break;
                    }
                }
            }
            else
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    {
                        myRenderer->isFocused = false;
                        SDL_ShowCursor(SDL_TRUE);
                        SDL_CaptureMouse(SDL_FALSE);
                        break;
                    }
                    case SDLK_r:
                    {
                        camera->reset();
                        break;
                    }
                    case SDLK_F11:
                    {
                        bool isFullScreen = SDL_GetWindowFlags(myWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                        SDL_SetWindowFullscreen(myWindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                        if(isFullScreen)
                            SDL_SetWindowPosition(myWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                        break;
                    }
                }
            }   
        }
        else if(e.type == SDL_KEYUP)
            io->KeysDown[e.key.keysym.scancode] = false;
        else if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            if(!ImGui::IsAnyWindowHovered())
            {
                myRenderer->isFocused = true;
                SDL_ShowCursor(SDL_FALSE);
                myRenderer->xpos = (int)(io->DisplaySize.x / 2);
                myRenderer->ypos = (int)(io->DisplaySize.y / 2);
                SDL_WarpMouseInWindow(myWindow, myRenderer->xpos, myRenderer->ypos);
                SDL_CaptureMouse(SDL_TRUE);
            }
        }
        else if(e.type == SDL_WINDOWEVENT)
        {
            switch(e.window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    glViewport(0, 0, e.window.data1, e.window.data2);
            }
        }
        else if(e.type == SDL_MOUSEWHEEL)
        {
            if(e.wheel.y > 0)
                myRenderer->zoomLevel += 0.005f;
            else
                myRenderer->zoomLevel -= 0.005f;
            if(myRenderer->zoomLevel > 1.0f)
                myRenderer->zoomLevel = 1.0f;
            if(myRenderer->zoomLevel < 0.0f)
                myRenderer->zoomLevel = 0.0f;
        }
    }
    return false;
}

void destroyAll()
{
    delete myRenderer;
    delete camera;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(myWindow);
    SDL_Quit();
}