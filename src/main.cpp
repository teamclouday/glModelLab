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

    // get system info
    std::stringstream gl_version;
    gl_version << glGetString(GL_VERSION);
    std::stringstream glsl_version;
    glsl_version << glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::stringstream imgui_version;
    imgui_version << ImGui::GetVersion();
    std::stringstream sdl_version;
    SDL_version version;
    SDL_GetVersion(&version);
    sdl_version << (int)version.major << "." << (int)version.minor << "." << (int)version.patch;
    std::stringstream assimp_version;
    assimp_version << aiGetVersionMajor() << "." << aiGetVersionMinor();

    ProgramInfo *info = new ProgramInfo();
    info->Assimp_Version = assimp_version.str();
    info->GLSL_Version = glsl_version.str();
    info->ImGui_Version = imgui_version.str();
    info->OpenGL_Version = gl_version.str();
    info->SDL2_Version = sdl_version.str();

    // init camera
    Camera *myCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    // init renderer
    Renderer *myRenderer = new Renderer();
    // create manager
    manager = new GlobalManager(info);
    manager->store(GLOB_WINDOW, (void*)myWindow);
    manager->store(GLOB_CONTEXT, (void*)myContext);
    manager->store(GLOB_CAMERA, (void*)myCamera);
    manager->store(GLOB_RENDERER, (void*)myRenderer);

    // setup fonts for ImGui
    ImGui::GetIO().Fonts->AddFontFromFileTTF((std::string(MY_ROOT_DIR) + "/external/roboto/Roboto-Regular.ttf").c_str(), 22.0f);    // menu and window titles
    ImGui::GetIO().Fonts->AddFontFromFileTTF((std::string(MY_ROOT_DIR) + "/external/roboto/Roboto-Italic.ttf").c_str(), 22.0f);     // menu and window titles
    ImGui::GetIO().Fonts->AddFontFromFileTTF((std::string(MY_ROOT_DIR) + "/external/roboto/Roboto-Bold.ttf").c_str(), 22.0f);       // menu and window titles
    ImGui::GetIO().Fonts->AddFontFromFileTTF((std::string(MY_ROOT_DIR) + "/external/roboto/Roboto-Regular.ttf").c_str(), 18.0f);    // window contents
    ImGui::GetIO().Fonts->AddFontFromFileTTF((std::string(MY_ROOT_DIR) + "/external/roboto/Roboto-Italic.ttf").c_str(), 18.0f);     // window contents
    ImGui::GetIO().Fonts->AddFontFromFileTTF((std::string(MY_ROOT_DIR) + "/external/roboto/Roboto-Bold.ttf").c_str(), 18.0f);       // window contents

    return true;
}

// clean environment before exit
void quitAll()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if(manager)
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
                    {
                        if(manager->myCamera->focus)
                        {
                            manager->myCamera->focus = false;
                            SDL_ShowCursor(SDL_TRUE);
                            SDL_CaptureMouse(SDL_FALSE);
                        }
                        else
                            return true;
                        break;
                    }
                    case SDLK_F11:
                    {
                        bool isFullScreen = SDL_GetWindowFlags(manager->myWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
#ifdef __unix__
                        SDL_SetWindowPosition(manager->myWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
#endif
                        SDL_SetWindowFullscreen(manager->myWindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
#ifdef _WIN32
                        if(isFullScreen)
                            SDL_SetWindowPosition(manager->myWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
#endif
                        break;
                    }
                    case SDLK_r:
                    {
                        if(manager->myCamera->focus)
                            manager->myCamera->reset();
                        break;
                    }
                    case SDLK_w:
                        if(manager->myCamera->focus)
                            manager->myCamera->keyMap[0] = true;
                        break;
                    case SDLK_a:
                        if(manager->myCamera->focus)
                            manager->myCamera->keyMap[1] = true;
                        break;
                    case SDLK_s:
                        if(manager->myCamera->focus)
                            manager->myCamera->keyMap[2] = true;
                        break;
                    case SDLK_d:
                        if(manager->myCamera->focus)
                            manager->myCamera->keyMap[3] = true;
                        break;
                    case SDLK_UP:
                        if(manager->myCamera->focus)
                            manager->myCamera->keyMap[4] = true;
                        break;
                    case SDLK_DOWN:
                        if(manager->myCamera->focus)
                            manager->myCamera->keyMap[5] = true;
                        break;
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if(!ImGui::IsAnyWindowHovered() && !manager->myCamera->focus)
                {
                    manager->myCamera->focus = true;
                    SDL_ShowCursor(SDL_FALSE);
                    int w, h;
                    SDL_GetWindowSize(manager->myWindow, &w, &h);
                    manager->myCamera->mousePos = {(int)(w/2), (int)(h/2)};
                    SDL_WarpMouseInWindow(manager->myWindow, manager->myCamera->mousePos[0], manager->myCamera->mousePos[1]);
                    SDL_CaptureMouse(SDL_TRUE);
                }
                break;
            }
            case SDL_KEYUP:
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_w:
                        manager->myCamera->keyMap[0] = false;
                        break;
                    case SDLK_a:
                        manager->myCamera->keyMap[1] = false;
                        break;
                    case SDLK_s:
                        manager->myCamera->keyMap[2] = false;
                        break;
                    case SDLK_d:
                        manager->myCamera->keyMap[3] = false;
                        break;
                    case SDLK_UP:
                        manager->myCamera->keyMap[4] = false;
                        break;
                    case SDLK_DOWN:
                        manager->myCamera->keyMap[5] = false;
                        break;
                }
                break;
            }
        }
    }
    if(manager->myCamera->focus)
    {
        manager->myCamera->update(0.0f, true);
    }
    return false;
}

void fpsControl(Uint32* tNow, Uint32 *tPrev)
{
    *tNow = SDL_GetTicks();
    Uint32 tDelta = *tNow - *tPrev;
    if(tDelta < (1000 / FPS) && manager->fpsLimit)
        SDL_Delay((Uint32)(1000 / FPS) - tDelta);
    *tPrev = SDL_GetTicks();
}