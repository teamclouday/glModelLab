#include "renderer.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;
extern ImGuiIO *io;
extern Camera *camera;
extern std::vector<bool> keys;

std::string findModelName(std::string folderPath);

Renderer::Renderer(ImVec4 clear_color) : modelIdx(2, 0), shaderIdx(2, 0)
{
    clearColor = clear_color;
    this->myModel = nullptr;
    this->myShader = nullptr;
    this->refreshAll = false;
    this->isModelOn = false;
    this->isFocused = false;
    this->deltaTime = 0.0f;
    this->lastFrame = 0.0f;
    this->xpos = (int)(io->DisplaySize.x / 2);
    this->ypos = (int)(io->DisplaySize.y / 2);
    this->zoomLevel = 0.8f;

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
    float currentTime = SDL_GetTicks();
    this->deltaTime = currentTime - this->lastFrame;
    this->lastFrame = currentTime;

    this->handleMouse(this->isFocused);

    SDL_GL_MakeCurrent(myWindow, glContext);
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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
    {
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(45.0f, (io->DisplaySize.x/io->DisplaySize.y), 0.1f, 1000.0f);
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(this->zoomLevel));
        this->myShader->use();
        glUniformMatrix4fv(glGetUniformLocation(this->myShader->programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(this->myShader->programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(this->myShader->programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        myModel->draw(this->myShader);
    }
    
    this->setUpImGui();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(myWindow);
}

void Renderer::setUpImGui()
{
    ImGui::Begin("Colors");
    ImGui::ColorEdit3("Background Color",  (float*)&clearColor);
    ImGui::End();

    ImGui::Begin("Models");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Models List");
    
    if(this->model_list.size() <= 2)
        ImGui::Text("No models found under Models folder!");
    else
    {
        for(unsigned i = 2; i < this->model_list.size(); i++)
        {
            ImGui::RadioButton(model_list[i].c_str(), &this->modelIdx[1], i);
        }
    }

    ImGui::End();

    ImGui::Begin("Shaders");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Shaders List");

    if(this->shader_list.size() <= 2)
        ImGui::Text("No shaders found under Shaders folder!");
    else
    {
        for(unsigned i = 2; i < this->shader_list.size(); i++)
        {
            ImGui::RadioButton(shader_list[i].c_str(), &this->shaderIdx[1], i);
        }
    }

    ImGui::End();

    if(this->isModelOn)
    {
        ImGui::Begin("Configs");
        ImGui::DragFloat("Model Zoom Level", &this->zoomLevel, 0.001f, 0.0f, 1.5f);
        ImGui::End();
    }
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
    if(!this->isModelOn)
        this->isModelOn = true;
    if(this->myModel != nullptr)
        delete this->myModel;
    if(this->myShader != nullptr)
    {
        glDeleteProgram(this->myShader->programID);
        delete this->myShader;
    }

    std::string shaderPath = this->shader_list[this->shaderIdx[1]] + "/" + this->shader_list[this->shaderIdx[1]];
    this->myShader = new Shader(shaderPath + ".vs", shaderPath + ".fs");

    std::string modelName = findModelName("./Models/"  + this->model_list[this->modelIdx[1]]);

    this->myModel = new Model("./Models/"  + this->model_list[this->modelIdx[1]] + "/" + modelName);
    this->refreshAll = false;
}

void Renderer::handleMouse(bool isfocused)
{
    if(!isfocused)
        return;
    // handle keyboard motion
    if(keys[SDL_SCANCODE_W])
        camera->ProcessKeyboard(FORWARD, this->deltaTime);
    if(keys[SDL_SCANCODE_S])
        camera->ProcessKeyboard(BACKWARD, this->deltaTime);
    if(keys[SDL_SCANCODE_A])
        camera->ProcessKeyboard(LEFT, this->deltaTime);
    if(keys[SDL_SCANCODE_D])
        camera->ProcessKeyboard(RIGHT, this->deltaTime);
    // handle mouse motion
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    float xoffset = (float)(mx - this->xpos);
    float yoffset = (float)(my - this->ypos);
    this->xpos = (int)(io->DisplaySize.x / 2);
    this->ypos = (int)(io->DisplaySize.y / 2);
    camera->ProcessMouseMovement(xoffset, yoffset);
    SDL_WarpMouseInWindow(myWindow, this->xpos, this->ypos);
}

std::string findModelName(std::string folderPath)
{
    std::string model = "";
    DIR *dir;
    struct dirent *ent;
    dir = opendir(folderPath.c_str());
    while ((ent = readdir (dir)) != NULL) {
        std::string filename = std::string(ent->d_name);
        std::string ext = filename.substr(filename.find_last_of(".") + 1);
        if(ext == "obj" ||
           ext == "3ds" ||
           ext == "gltf" ||
           ext == "fbx" ||
           ext == "dae" ||
           ext == "dxf" ||
           ext == "stl")
        {
            model = filename;
            break;
        }
    }
    closedir (dir);
    return model;
}