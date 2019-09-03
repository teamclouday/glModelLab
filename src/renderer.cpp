#include "renderer.hpp"
#include "manager.hpp"

extern GlobalManager *manager;

Renderer::Renderer()
{
    // do nothing for now
}

Renderer::~Renderer()
{
    // do nothing for now
}

void Renderer::renderMenu()
{
    // menubar first
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Materials"))
        {
            if(ImGui::MenuItem("Models", NULL))
            {
                // render model box here
            }
            if(ImGui::MenuItem("Shaders", NULL))
            {
                // render shader box here
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Lights"))
        {
            if(ImGui::MenuItem("Point Lights", NULL))
            {
                //
            }
            if(ImGui::MenuItem("Direct Lights", NULL))
            {
                // 
            }
            if(ImGui::MenuItem("Torch Lights", NULL))
            {
                //
            }

            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Renderer"))
        {
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Help"))
        {
            if(ImGui::MenuItem("About", NULL))
            {

            }
            if(ImGui::MenuItem("Github", NULL))
            {
                #ifdef __unix__
                system("xdg-open https://github.com/teamclouday/glModelLab");
                #endif
                
                #ifdef _WIN32
                ShellExecute(NULL, "open", L"https://github.com/teamclouday/glModelLab", NULL, NULL, SW_SHOWNORMAL);
                #endif
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Renderer::renderScene()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(manager->myWindow);
    ImGui::NewFrame();

    int w, h;
    SDL_GetWindowSize(manager->myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    this->renderMenu();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(manager->myWindow);
}