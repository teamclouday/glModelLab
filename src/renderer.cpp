#include "renderer.hpp"
#include "manager.hpp"

extern GlobalManager *manager;

Renderer::Renderer()
{
    this->myRenderConfig = new RENDER_CONFIG();
    this->myMenuConfig = new MENU_CONFIG();
}

Renderer::~Renderer()
{
    delete this->myRenderConfig;
    delete this->myMenuConfig;
}

void Renderer::renderMenu()
{
    MENU_CONFIG *pMenu = this->myMenuConfig;
    RENDER_CONFIG *pRender = this->myRenderConfig;

    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, pRender->menu_background_color);
    ImGui::PushStyleColor(ImGuiCol_Text, pRender->menu_text_color);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

    // menubar first
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Materials"))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            ImGui::MenuItem("Models", NULL, &pMenu->displayModels);
            ImGui::MenuItem("Shaders", NULL, &pMenu->displayShaders);
            ImGui::PopFont();
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Lights"))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            ImGui::MenuItem("Point Lights", NULL, &pMenu->pointLights);
            ImGui::MenuItem("Direct Lights", NULL, &pMenu->directLight);
            ImGui::MenuItem("Torch Lights", NULL, &pMenu->torchLight);
            ImGui::PopFont();
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Renderer"))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            ImGui::MenuItem("Colors", NULL, &pMenu->displayColors);
            ImGui::PopFont();
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Help"))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            ImGui::MenuItem("About", NULL, &pMenu->displayAbout);
            if(ImGui::MenuItem("Github", NULL))
            {
                #ifdef __unix__
                system("xdg-open https://github.com/teamclouday/glModelLab");
                #endif
                
                #ifdef _WIN32
                ShellExecute(NULL, "open", L"https://github.com/teamclouday/glModelLab", NULL, NULL, SW_SHOWNORMAL);
                #endif
            }
            ImGui::PopFont();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    // render actual windows
    if(pMenu->displayModels)
    {
        ImGui::SetNextWindowPos(ImVec2(0.05f*pRender->window_w, 0.1f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.9f*pRender->window_w, 0.85f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(pRender->window_alpha);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Begin("Models", &pMenu->displayModels);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
        for(unsigned i = 0; i < manager->models.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::RadioButton(manager->models[i].c_str(), &pRender->modelID, i);
            ImGui::PopID();
        }
        if(!manager->models.size())
            ImGui::Text("Where're the models under the \"models\" folder?");
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    if(pMenu->displayShaders)
    {
        ImGui::SetNextWindowPos(ImVec2(0.05f*pRender->window_w, 0.1f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.9f*pRender->window_w, 0.85f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(pRender->window_alpha);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Begin("Shaders", &pMenu->displayShaders);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
        for(unsigned i = 0; i < manager->shaders.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::RadioButton(manager->shaders[i].c_str(), &pRender->shaderID, i);
            ImGui::PopID();
        }
        if(!manager->shaders.size())
            ImGui::Text("Where're the shaders under the \"shaders\" folder?");
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    if(pMenu->displayColors)
    {
        ImGui::SetNextWindowPos(ImVec2(0.05f*pRender->window_w, 0.1f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.9f*pRender->window_w, 0.85f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(pRender->window_alpha);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Begin("Color Options", &pMenu->displayColors);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
        ImGui::ColorEdit4("Background", &pRender->background_color[0]);
        ImGui::DragFloat("Window Alpha", &pRender->window_alpha, 0.001f, 0.2f, 1.0f, "%.3f");
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    if(pMenu->displayAbout)
    {
        ImGui::SetNextWindowPos(ImVec2(0.05f*pRender->window_w, 0.1f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.9f*pRender->window_w, 0.85f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(pRender->window_alpha);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Begin("About", &pMenu->displayAbout);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
        std::stringstream sstr;
        sstr << "This program aims to create a platform for experimenting different lighting effects\n" <<
                "on OpenGL, with easy model loading and configuration\n\n" <<
                "The author of this program is currently learning shading and shadowing techniques,\n" <<
                "thus hopefully this program will be upgraded to a versatile and robost system";
        ImGui::Text(sstr.str().c_str());
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }
}

void Renderer::renderScene()
{
    RENDER_CONFIG *pRender = this->myRenderConfig;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(manager->myWindow);
    ImGui::NewFrame();

    SDL_GetWindowSize(manager->myWindow, &pRender->window_w, &pRender->window_h);
    glViewport(0, 0, pRender->window_w, pRender->window_h);
    glClearColor(this->myRenderConfig->background_color.x,
                 this->myRenderConfig->background_color.y,
                 this->myRenderConfig->background_color.z,
                 this->myRenderConfig->background_color.w);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    this->renderMenu();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(manager->myWindow);
}