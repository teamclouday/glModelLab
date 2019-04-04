// implement some basic functions
#include "basic.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;
extern ImGuiIO* io;

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
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &(ImGui::GetIO());
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(myWindow, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void startLoop()
{
    ImVec4 clear_color = ImVec4(0.4f, 0.5f, 0.6f, 1.0f);
    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();

        // new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(myWindow);
        ImGui::NewFrame();

        ImGui::Begin("Hello, world!");
        ImGui::ColorEdit3("Clear Color",  (float*)&clear_color);
        ImGui::End();

        // render
        ImGui::Render();
        SDL_GL_MakeCurrent(myWindow, glContext);
        glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(myWindow);
    SDL_Quit();
}