// implement some basic functions
#include "basic.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;
extern ImGuiIO* io;
extern Renderer *myRenderer;
extern Camera *camera;

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam);

void initAll(const std::string title, int width, int height)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to init SDL2\nSDL Error: %s\n", SDL_GetError());
        exit(ERROR_SDL_INIT);
    }
    // set up OpenGL
    const char* glsl_version = "#version 330";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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
}

void startLoop()
{
    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();

        myRenderer->startFrame();
        myRenderer->render();
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
            if(!myRenderer->isFocused)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        return true;
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
                }
            }   
        }
        else if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            if(!ImGui::IsAnyWindowHovered())
            {
                myRenderer->isFocused = true;
                SDL_ShowCursor(SDL_FALSE);
                SDL_CaptureMouse(SDL_TRUE);
            }
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


void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

// During init, enable debug output
