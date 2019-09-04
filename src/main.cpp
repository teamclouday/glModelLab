#include "main.hpp"

GlobalManager *manager;

int main(int argc, char *argv[])
{
    if(!initEnv())
        return 1;
    
    // program loop
    bool quit = false;
    Uint32 tNow = SDL_GetTicks();
    Uint32 tPrev = SDL_GetTicks();
    while(!quit)
    {
        quit = pollEvents();
        fpsControl(&tNow, &tPrev);
        manager->myRenderer->renderScene();
    }

    quitAll();
    return 0;
}

// init environment
bool initEnv()
{
    // init SDL2
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to init SDL2\nSDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // set SDL2 OpenGL attributes
    const char *glsl_ver = "#version 330 core";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // create window
    SDL_Window *myWindow = SDL_CreateWindow(WINDOW_TITLE,
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            WINDOW_DEFAULT_WIDTH,
                                            WINDOW_DEFAULT_HEIGHT,
                                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(!myWindow)
    {
        std::cout << "Failed to create SDL_Window\nSDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // create OpenGL context
    SDL_GLContext myContext = SDL_GL_CreateContext(myWindow);
    if(!myContext)
    {
        std::cout << "Failed to create SDL_GLContext\nSDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // set vsync
    SDL_GL_SetSwapInterval(1);
    // enable extension features based on graphics card
    // possibly not useful in this project
    glewExperimental = GL_TRUE;
    // init GLEW
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to init GLEW" << std::endl;
        return false;
    }
    // setup OpenGL defaults
    glViewport(0, 0, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    // setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(myWindow, myContext);
    ImGui_ImplOpenGL3_Init(glsl_ver);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui::GetIO().IniFilename = nullptr;
    // init camera
    Camera *myCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    // init renderer
    Renderer *myRenderer = new Renderer();
    // create manager
    manager = new GlobalManager();
    manager->store(GLOB_WINDOW, (void*)myWindow);
    manager->store(GLOB_CONTEXT, (void*)myContext);
    manager->store(GLOB_CAMERA, (void*)myCamera);
    manager->store(GLOB_RENDERER, (void*)myRenderer);

    // setup fonts for ImGui
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./externel/roboto/Roboto-Regular.ttf", 22.0f);    // menu and window titles
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./externel/roboto/Roboto-Italic.ttf", 22.0f);     // menu and window titles
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./externel/roboto/Roboto-Bold.ttf", 22.0f);       // menu and window titles
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./externel/roboto/Roboto-Regular.ttf", 18.0f);    // window contents
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./externel/roboto/Roboto-Italic.ttf", 18.0f);     // window contents
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./externel/roboto/Roboto-Bold.ttf", 18.0f);       // window contents

    return true;
}

// clean environment before exit
void quitAll()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if(!manager)
        delete manager;

    SDL_Quit();
}

// handle system events
// return true if quit program
bool pollEvents()
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
        switch(e.type)
        {
            case SDL_QUIT:
                return true;
            case SDL_KEYDOWN:
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        return true;
                    case SDLK_F11:
                    {
                        bool isFullScreen = SDL_GetWindowFlags(manager->myWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                        SDL_SetWindowPosition(manager->myWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                        SDL_SetWindowFullscreen(manager->myWindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                        break;
                    }
                }
            }
            case SDL_WINDOWEVENT:
            {
                switch(e.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        glViewport(0, 0, e.window.data1, e.window.data2);
                        break;
                }
            }
        }
    }
    return false;
}

void fpsControl(Uint32* tNow, Uint32 *tPrev)
{
    *tNow = SDL_GetTicks();
    Uint32 tDelta = *tNow - *tPrev;
    if(tDelta < (1000 / FPS))
        SDL_Delay((Uint32)(1000 / FPS) - tDelta);
    *tPrev = SDL_GetTicks();
}