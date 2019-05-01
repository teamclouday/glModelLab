#include "renderer.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;
extern ImGuiIO *io;
extern Camera *camera;
extern SDL_GameController *myController;

std::string findModelName(std::string folderPath);

Renderer::Renderer(ImVec4 clear_color) : modelIdx(2, 0), shaderIdx(2, 0)
{
    clearColor = clear_color;
    this->myModel = nullptr;
    this->myShader = nullptr;
    this->refreshAll = false;
    this->isFocused = false;
    this->displayModels = false;
    this->displayShaders = false;
    this->displayInfo = false;
    this->displayConfigBack = false;
    this->displayConfigModel = false;
    this->displayConfigLight = false;
    this->enableShadow = false;
    this->quit = false;
    this->deltaTime = 0.0f;
    this->lastFrame = (float)SDL_GetTicks();
    this->xpos = (int)(io->DisplaySize.x / 2);
    this->ypos = (int)(io->DisplaySize.y / 2);
    this->zoomLevel = 0.8f;
    
    glCreateBuffers(1, &this->lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, this->lightBuffer);
    glBufferStorage(GL_UNIFORM_BUFFER, 5*sizeof(SourceLight), NULL, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, this->lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    this->loadModelLists();
    this->loadShaderLists();

    this->myShadow = new ShadowMap();
}

Renderer::~Renderer()
{
    if(myModel != nullptr)
        delete this->myModel;
    if(myShader != nullptr)
        delete this->myShader;
    modelIdx.clear();
    modelIdx.shrink_to_fit();
    shaderIdx.clear();
    shaderIdx.shrink_to_fit();
    model_list.clear();
    model_list.shrink_to_fit();
    shader_list.clear();
    shader_list.shrink_to_fit();
    myLights.clear();
    myLights.shrink_to_fit();

    delete this->myShadow;
}

void Renderer::startFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(myWindow);
    ImGui::NewFrame();
}

void Renderer::render()
{
    float currentTime = (float)SDL_GetTicks();
    this->deltaTime = currentTime - this->lastFrame;
    this->lastFrame = currentTime;

    if(this->isFocused)
        this->handleMouse();

    if(myController)
        this->handleController();

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
        this->glRenderAll();
    
    this->setUpImGui();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(myWindow);
}

void Renderer::glRenderAll()
{
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(45.0f, (io->DisplaySize.x/io->DisplaySize.y), 0.1f, 1000.0f);
    glm::mat4 model(1.0f);
    model = glm::scale(model, glm::vec3(this->zoomLevel));
    this->myShader->use();
    for(unsigned i = 0; i < this->myLights.size(); i++)
        glNamedBufferSubData(this->lightBuffer, i*sizeof(SourceLight), sizeof(SourceLight), myLights[i]);
    glUniformMatrix4fv(glGetUniformLocation(this->myShader->programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(this->myShader->programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(this->myShader->programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(this->myShader->programID, "CameraPos"), 1, glm::value_ptr(camera->Position));
    glUniform3fv(glGetUniformLocation(this->myShader->programID, "CameraDir"), 1, glm::value_ptr(camera->Front));
    glUniform1i(glGetUniformLocation(this->myShader->programID, "LightNum"), (GLint)(this->myLights.size()));
    myModel->draw(this->myShader);
}

void Renderer::setUpImGui()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));
    ImGui::BeginMainMenuBar();

    if(ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("Models", NULL, &this->displayModels);
        ImGui::MenuItem("Shaders", NULL, &this->displayShaders);
        ImGui::MenuItem("Cubemaps", NULL);
        // TODO: add cubemap support
        if(ImGui::MenuItem("Quit", "ECS"))
            this->quit = true;
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Configs"))
    {
        ImGui::MenuItem("Background", NULL, &this->displayConfigBack);
        ImGui::MenuItem("Model", NULL, &this->displayConfigModel);
        ImGui::MenuItem("SourceLight", NULL, &this->displayConfigLight);
        ImGui::MenuItem("Shadow", NULL, &this->enableShadow);
        
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Help"))
    {
        if(ImGui::MenuItem("Toggle Fullscreen", "F11"))
        {
            bool isFullScreen = SDL_GetWindowFlags(myWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
            SDL_SetWindowFullscreen(myWindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
            if(isFullScreen)
                SDL_SetWindowPosition(myWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
        ImGui::MenuItem("Info", "H", &this->displayInfo);
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
    ImGui::PopStyleVar();

    if(this->displayInfo)
    {
    ImGui::SetNextWindowPos(ImVec2(io->DisplaySize.x*0.6f - 20.0f, io->DisplaySize.y - 220.0f));
    ImGui::SetNextWindowSize(ImVec2(io->DisplaySize.x*0.4f, 200.0f));
    ImGui::Begin("Info");

    ImGui::Text("Current FPS: %.1f", io->Framerate);
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera->Position.x, camera->Position.y, camera->Position.z);
    ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", camera->Front.x, camera->Front.y, camera->Front.z);
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Text("About this project:");
    ImGui::Text("ModelLab -- a project made to experiment with OpenGL");
    ImGui::Text("Author: teamclouday  Email: teamclouday@gmail.com");
    ImGui::Text("Built with glew, sdl2, imgui, assimp, glm and soil");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Text("System Info:");
    ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
    ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
    ImGui::Text("glsl version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    ImGui::End();
    }

    if(this->displayConfigBack ||
       this->displayConfigModel ||
       this->displayConfigLight)
    {
    ImGui::SetNextWindowPos(ImVec2(20.0f, io->DisplaySize.y - 220.0f));
    ImGui::SetNextWindowSize(ImVec2(io->DisplaySize.x*0.45f, 200.0f));
    ImGui::Begin("Configs");

    if(this->displayConfigBack)
    {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Background Configs");
        ImGui::Spacing();
        ImGui::ColorEdit3("Background Color",  (float*)&clearColor);
    }
    if(this->displayConfigModel)
    {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Model Configs");
        ImGui::Spacing();
        ImGui::DragFloat("Model Zoom Level", &this->zoomLevel, 0.001f, 0.0f, 1.5f);
    }
    if(this->displayConfigLight)
    {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Light Configs");
        ImGui::Spacing();
        if(this->myLights.size() < 5)
        {
            if(ImGui::Button("Add Light"))
            {
                SourceLight *newLight = new SourceLight;
                newLight->position = glm::vec4(10.0f, 10.0f, 10.0f, 0.0f);
                newLight->color = glm::vec4(1.0f);
                newLight->attenuation = 0.2f;
                newLight->ambientCoeff = 0.5f;
                newLight->coneAngle = 0.0f;
                newLight->specCoeff = 32.0f;
                this->myLights.push_back(newLight);
            }
        }
        if(this->myLights.size() > 0)
        {
            if(ImGui::Button("Delete Light"))
            {
                SourceLight *lastLight = this->myLights[myLights.size()-1];
                myLights.pop_back();
                delete lastLight;
            }
        }
        for(unsigned i = 0; i < this->myLights.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::Spacing();
            ImGui::Text("Light %u", i+1);
            ImGui::Spacing();
            ImGui::InputFloat4("Light Position", (float*)glm::value_ptr(myLights[i]->position));
            ImGui::ColorEdit3("Light Color", (float*)glm::value_ptr(myLights[i]->color));
            ImGui::DragFloat("Light Attenuation", &(myLights[i]->attenuation), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Light Ambient Coefficient", &(myLights[i]->ambientCoeff), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Light Cone Angle", &(myLights[i]->coneAngle), 0.1f, 0.0f, 80.0f);
            ImGui::DragFloat("Light Specular Coefficient", &(myLights[i]->specCoeff), 0.5f, 8.0f, 64.0f);
            ImGui::PopID();
        }
    }

    ImGui::End();
    }

    if(this->displayModels)
    {
    ImGui::SetNextWindowPos(ImVec2(io->DisplaySize.x - 280.0f, 40.0f));
    ImGui::SetNextWindowSize(ImVec2(250.0f, io->DisplaySize.y * 0.35f));
    ImGui::Begin("Models");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Models List");
    ImGui::Spacing();
    
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
    }

    if(this->displayShaders)
    {
    ImGui::SetNextWindowPos(ImVec2(io->DisplaySize.x - 280.0f, io->DisplaySize.y*0.4f + 40.0f));
    ImGui::SetNextWindowSize(ImVec2(250.0f, io->DisplaySize.y*0.25f));
    ImGui::Begin("Shaders");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Shaders List");
    ImGui::Spacing();

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
        if(dir->d_name == std::string("shadow"))
            continue;
        this->shader_list.push_back(dir->d_name);
    }
    closedir(d);
}

void Renderer::refresh()
{
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

void Renderer::handleMouse()
{
    // handle keyboard motion
    if(io->KeysDown[SDL_SCANCODE_W])
        camera->ProcessKeyboard(FORWARD, this->deltaTime);
    if(io->KeysDown[SDL_SCANCODE_S])
        camera->ProcessKeyboard(BACKWARD, this->deltaTime);
    if(io->KeysDown[SDL_SCANCODE_A])
        camera->ProcessKeyboard(LEFT, this->deltaTime);
    if(io->KeysDown[SDL_SCANCODE_D])
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

void Renderer::handleController()
{
    if(SDL_GameControllerGetButton(myController, SDL_CONTROLLER_BUTTON_DPAD_UP))
        this->zoomLevel += 0.001f;
    if(SDL_GameControllerGetButton(myController, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
        this->zoomLevel -= 0.001f;
    if(this->zoomLevel > 1.0f)
        this->zoomLevel = 1.0f;
    if(this->zoomLevel < 0.0f)
        this->zoomLevel = 0.0f;

    Sint16 lx, ly, rx, ry;
    lx = SDL_GameControllerGetAxis(myController, SDL_CONTROLLER_AXIS_LEFTX);
    ly = SDL_GameControllerGetAxis(myController, SDL_CONTROLLER_AXIS_LEFTY);
    rx = SDL_GameControllerGetAxis(myController, SDL_CONTROLLER_AXIS_RIGHTX);
    ry = SDL_GameControllerGetAxis(myController, SDL_CONTROLLER_AXIS_RIGHTY);

    if(lx > SDL_CONTROLLERAXIS_LEFT_DEADZONE)
        camera->ProcessKeyboard(RIGHT, this->deltaTime);
    if(lx < -SDL_CONTROLLERAXIS_LEFT_DEADZONE)
        camera->ProcessKeyboard(LEFT, this->deltaTime);
    if(ly > SDL_CONTROLLERAXIS_LEFT_DEADZONE)
        camera->ProcessKeyboard(BACKWARD, this->deltaTime);
    if(ly < -SDL_CONTROLLERAXIS_LEFT_DEADZONE)
        camera->ProcessKeyboard(FORWARD, this->deltaTime);
    if(rx < SDL_CONTROLLERAXIS_RIGHT_DEADZONE && rx > -SDL_CONTROLLERAXIS_RIGHT_DEADZONE)
        rx = 0;
    if(ry < SDL_CONTROLLERAXIS_RIGHT_DEADZONE && ry > -SDL_CONTROLLERAXIS_RIGHT_DEADZONE)
        ry = 0;
    camera->ProcessMouseMovement((float)(rx / 800.0f), (float)(ry / 800.0f));
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