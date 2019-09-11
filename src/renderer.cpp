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
            if(ImGui::MenuItem("Models", NULL, &pMenu->displayModels))
                manager->update();
            if(ImGui::MenuItem("Shaders", NULL, &pMenu->displayShaders))
                manager->update();
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
                system("cmd /c start https://github.com/teamclouday/glModelLab");
                #endif
            }
            ImGui::MenuItem("System", NULL, &pMenu->displaySystem);
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
            if(ImGui::RadioButton(manager->models[i].c_str(), &pRender->modelID, i))
                this->loadModel();
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
            if(ImGui::RadioButton(manager->shaders[i].c_str(), &pRender->shaderID, i))
                this->loadShader();
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

    if(pMenu->displaySystem)
    {
        ImGui::SetNextWindowPos(ImVec2(0.05f*pRender->window_w, 0.1f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.9f*pRender->window_w, 0.85f*pRender->window_h), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(pRender->window_alpha);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Begin("System", &pMenu->displaySystem);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
        std::stringstream sstr;
        sstr << "Program System Info:\n\n" <<
                "External Libraries\n" <<
                "OpenGL Version: " << manager->info->OpenGL_Version << "\n" <<
                "GLSL Version: " << manager->info->OpenGL_Version << "\n" <<
                "SDL2 Version: " << manager->info->SDL2_Version << "\n" <<
                "Imgui Version: " << manager->info->ImGui_Version << "\n" <<
                "Assimp Version: " << manager->info->Assimp_Version << "\n\n" <<
                "Current FPS: " << ImGui::GetIO().Framerate << "\n" <<
                "FPS Limit: " << ((manager->fpsLimit ? "On" : "Off")) << "\n\n" <<
                "Current Scene\n" <<
                "Camera Location: " << "(" << manager->myCamera->Position.x << "," <<
                                              manager->myCamera->Position.y << "," <<
                                              manager->myCamera->Position.z << ")\n";
        ImGui::Text(sstr.str().c_str());
        ImGui::SetCursorPos(ImVec2(200, 200));
        ImGui::Checkbox("FPS Control", &manager->fpsLimit);
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

    Shader *myShader = this->myRenderConfig->shader;
    Model *myModel = this->myRenderConfig->model;

    if(myShader && myModel)
    {
        manager->myCamera->update(ImGui::GetIO().Framerate, false);
        glm::mat4 view = manager->myCamera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)pRender->window_w/(float)pRender->window_h, 0.1f, 1000.0f);
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(manager->myCamera->mv_zoom));
        myShader->use();
        glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        myModel->draw(myShader->programID);
        myShader->disuse();
    }

    this->renderMenu();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(manager->myWindow);
}

void Renderer::loadModel()
{
    if(this->myRenderConfig->model)
        delete this->myRenderConfig->model;
    this->myRenderConfig->model = new Model(std::string(MY_ROOT_DIR) + "/models/" + manager->models[this->myRenderConfig->modelID]);
}

void Renderer::loadShader()
{
    if(this->myRenderConfig->shader)
        delete this->myRenderConfig->shader;

    Shader *newShader = new Shader();

#ifdef __unix__
    DIR *d;
    struct dirent *dir;
    d = opendir((std::string(MY_ROOT_DIR) + "/shaders/" + manager->shaders[this->myRenderConfig->shaderID]).c_str());
    while((dir = readdir(d)) != NULL)
    {
        std::string filename = std::string(dir->d_name);
        if(filename[0] == '.')
            continue;
        std::string ext = filename.substr(filename.find_last_of(".") + 1);
        GLenum shaderType = -1;
        if(ext == "vert")
            shaderType = GL_VERTEX_SHADER;
        else if(ext == "frag")
            shaderType = GL_FRAGMENT_SHADER;
        else if(ext == "comp")
            shaderType = GL_COMPUTE_SHADER;
        else if(ext == "tesc")
            shaderType = GL_TESS_CONTROL_SHADER;
        else if(ext == "tese")
            shaderType = GL_TESS_EVALUATION_SHADER;
        else if(ext == "geom")
            shaderType = GL_GEOMETRY_SHADER;
        if(shaderType < 0)
            continue;
        else
            newShader->add(std::string(MY_ROOT_DIR) + "/shaders/" + manager->shaders[this->myRenderConfig->shaderID] + "/" + filename, shaderType);
    }
    closedir(d);
#endif

#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    hFind = FindFirstFile(LPCSTR((std::string(MY_ROOT_DIR) + "/shaders/" + manager->shaders[this->myRenderConfig->shaderID] + "/*").c_str()), &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string filename = std::string(fd.cFileName);
            if(filename[0] == '.')
                continue;
            std::string ext = filename.substr(filename.find_last_of(".") + 1);
            GLenum shaderType = -1;
            if(ext == "vert")
                shaderType = GL_VERTEX_SHADER;
            else if(ext == "frag")
                shaderType = GL_FRAGMENT_SHADER;
            else if(ext == "comp")
                shaderType = GL_COMPUTE_SHADER;
            else if(ext == "tesc")
                shaderType = GL_TESS_CONTROL_SHADER;
            else if(ext == "tese")
                shaderType = GL_TESS_EVALUATION_SHADER;
            else if(ext == "geom")
                shaderType = GL_GEOMETRY_SHADER;
            if(shaderType < 0)
                continue;
            else
                newShader->add(std::string(MY_ROOT_DIR) + "/shaders/" + manager->shaders[this->myRenderConfig->shaderID] + "/" + filename, shaderType);
        } while(FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
#endif

    if(!newShader->compile())
    {
        this->myRenderConfig->shader = nullptr;
        this->myRenderConfig->shaderID = -1;
    }
    else
        this->myRenderConfig->shader = newShader;
}