#include "GuiManager.h"
#include <iostream>
#include "GuiManager.h"
#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include "GraphicsManager.cpp"

//include this in the graphics manager, 
GuiManager::GuiManager()
{
}

void GuiManager::Start(GLFWwindow *window, WGPUDevice device, WGPUTextureFormat swapchainformat)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
  //  ImGui& io = ImGui::GetIO(); (void)io; 
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOther(window, true);
    WGPUTextureFormat ob = WGPUTextureFormat_Undefined; 
    ImGui_ImplWGPU_Init(device,3,swapchainformat, ob);
    //ImGui_ImplOpenGL3_Init("#version 330")
}


void GuiManager::Shutdown()
{
   
   // ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::Draw(  WGPURenderPassEncoder render_pass)
{
   

    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    ImGui::Begin("Dorm Shop!"); 
    //college student - 2 lava lamp 
    //rich-snitch - 2
    //gamer - 2 messy and high tech
    //pride - 2 
    //goth -2 - 
    //halloween -2 - 
    //christmas- 2
    //gym rat - 2
    //anime -2 - 
    //girly-pop -2
    //royal - 2
    //cat lady -2
    //nerd alert -2 
    //

   
    ImGui::Text("Explore each of the \nfurnature categories.");

//-------------beds---------------------------

    ImGui::SetWindowSize(ImVec2(200, 400));
    if (ImGui::Button("BED")) {
        
         printf("gui\n");
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Bed-SubMenu");
    }

    if (ImGui::BeginPopup("Bed-SubMenu")) {
        if (ImGui::Button("Boring-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default bed"); 
        }
        if (ImGui::Button("Patriot Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";
            
            printf("changed to patriot bed");

        
        }
    ImGui::EndPopup();
    }

//---------desk-------------
    if(ImGui::Button("Desk-Decor-I")) {
        
         printf("gui\n");
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Desk-SubMenu");
    }

    
    if (ImGui::BeginPopup("Desk-SubMenu")) {
        if (ImGui::Button("Boring-Desk-Decor-I")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default Desk Accesory1"); 
        }

        
    ImGui::EndPopup();
    }


    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render(); 
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(),render_pass);
   
}
