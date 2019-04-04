#include "renderer.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext glContext;
extern ImGuiIO* io;

Renderer::Renderer(ImVec4 clear_color)
{
    clearColor = clear_color;
    this->myModel = new Model();
}

Renderer::~Renderer()
{
    delete myModel;
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
}

void Renderer::setUpImGui()
{
    ImGui::Begin("Colors");
    ImGui::ColorEdit3("Background Color",  (float*)&clearColor);
    ImGui::End();

    ImGui::Begin("Models");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Model List (in Models folder)");
    ImGui::BeginChild("Scrolling");
    
    this->myModel->displayCheckList();

    ImGui::EndChild();
    ImGui::End();
}