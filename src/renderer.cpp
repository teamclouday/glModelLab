#include "renderer.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;
extern ImGuiIO* io;

Renderer::Renderer(ImVec4 clear_color) : modelIdx(2, 0), shaderIdx(2, 0)
{
    clearColor = clear_color;
    this->myModel = nullptr;
    this->myShader = nullptr;
    this->refreshAll = false;

    this->loadModelLists();
    this->loadShaderLists();
}

Renderer::~Renderer()
{
    if(myModel != nullptr)
        delete myModel;
    if(myShader != nullptr)
        delete myShader;
    modelIdx.clear();
    modelIdx.shrink_to_fit();
    shaderIdx.clear();
    shaderIdx.shrink_to_fit();
    model_list.clear();
    model_list.shrink_to_fit();
    shader_list.clear();
    shader_list.shrink_to_fit();
}

void Renderer::startFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(myWindow);
    ImGui::NewFrame();
}

void Renderer::render()
{
    ImGui::Render();
    SDL_GL_MakeCurrent(myWindow, glContext);
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(myWindow);
    
    if(this->modelIdx[0] != this->modelIdx[1])
    {
        this->modelIdx[0] = this->modelIdx[1];
        this->refreshAll = true;
    }
    if(this->shaderIdx[0] != this->shaderIdx[1])
    {
        this->shaderIdx[0] = this->shaderIdx[1];
        this->refreshAll = true;
    }
    if(this->refreshAll && this->modelIdx[1] != 0 && this->shaderIdx[1] != 0)
        this->refresh();

    if(this->myModel != nullptr && this->myShader != nullptr)
        myModel->draw(this->myShader);
}

void Renderer::setUpImGui()
{
    ImGui::Begin("Colors");
    ImGui::ColorEdit3("Background Color",  (float*)&clearColor);
    ImGui::End();

    ImGui::Begin("Models");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Models List");
    ImGui::BeginChild("Scrolling");
    
    if(this->model_list.size() <= 2)
        ImGui::Text("No models found under Models folder!");
    else
    {
        for(int i = 2; i < this->model_list.size(); i++)
        {
            ImGui::RadioButton(model_list[i].c_str(), &this->modelIdx[1], i);
        }
    }

    ImGui::EndChild();
    ImGui::End();

    ImGui::Begin("Shaders");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Shaders List");
    ImGui::BeginChild("Scrolling");

    if(this->shader_list.size() <= 2)
        ImGui::Text("No shaders found under Shaders folder!");
    else
    {
        for(int i = 2; i < this->shader_list.size(); i++)
        {
            ImGui::RadioButton(shader_list[i].c_str(), &this->shaderIdx[1], i);
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

void Renderer::loadModelLists()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./Models");
    while((dir = readdir(d)) != NULL)
    {
        this->model_list.push_back(dir->d_name);
    }
    closedir(d);
}

void Renderer::loadShaderLists()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./Shaders");
    while((dir = readdir(d)) != NULL)
    {
        this->shader_list.push_back(dir->d_name);
    }
    closedir(d);
}

void Renderer::refresh()
{
    if(this->myModel != nullptr)
        delete this->myModel;
    if(this->myShader != nullptr)
        delete this->myShader;

    std::string shaderPath = this->shader_list[this->shaderIdx[1]] + "/" + this->shader_list[this->shaderIdx[1]];
    this->myShader = new Shader(shaderPath + ".vs", shaderPath + ".fs");

    this->myModel = new Model("./Models/"  + this->model_list[this->modelIdx[1]] + "/scene.gltf");
    this->refreshAll = false;
}