#include "GuiManager.h"

//include this in the graphics manager, 
GuiManager::GuiManager()
{
}

void GuiManager::Start(GLFWwindow *window, WGPUDevice device, WGPUTextureFormat swapchainformat)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_Init(device,3,swapchainformat, WGPUTextureFormat_Undefined);
}


void GuiManager::Shutdown()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::Draw()
{
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();



    //gui commands imbetween
    ImGui::EndFrame();
    ImGui::Render(); 
}
