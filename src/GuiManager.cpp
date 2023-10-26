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
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOther(window, true);
    WGPUTextureFormat ob = WGPUTextureFormat_Undefined; 
    ImGui_ImplWGPU_Init(device,3,swapchainformat, ob);
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

    /*-----------------DORM SHOP--------------------
        CATEGORIES/IDEAS
    //rich-snitch - 2
    //gamer - 2 messy and high tech
    //pride - 2 
    //goth -2 - 
    //halloween -2 - 
    //christmas- 2
    //gym rat - 2
    //anime -2 - lava lamp
    //cat lady -2
    //nerd alert -2 
    
    ROOM SPRITES: 
    BED
    LAMP
    HOBBY DESK
    DRESSER DECOR- HOBBY
    KITCHEN DECOER
    RUG
    WINDOW DECOR
    */
    
    ImGui::Begin("Dorm Shop!"); 
    ImGui::SetNextWindowSize(ImVec2(400, 100));
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    ImGui::Text("Explore each of the \nfurnature categories.");

//-------------BEDS---------------------------

    ImGui::SetWindowSize(ImVec2(200, 400));
    if (ImGui::Button("BED")) {
        
         printf("bed\n");
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Bed-SubMenu");

    }
    
    if (ImGui::BeginPopup("Bed-SubMenu")) {
        if (ImGui::Button("Boring-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default bed\n"); 
        }
        if (ImGui::Button("Patriot-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";
            printf("changed to patriot bed\n"); 
        }
        if (ImGui::Button("Patriot-Bed-II")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";   
            printf("changed to patriot bed2");
        }
        if (ImGui::Button("Rich-Snitch")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";   
            printf("changed to rich snitch");
        }
         if (ImGui::Button("Gamer-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed";   
            printf("changed to gamer bed");
        }
         if (ImGui::Button("Gamer-Bed-II")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed2";   
            printf("changed to gamer bed2");
        }
        if (ImGui::Button("Rainbow-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "pridebed";   
            printf("changed to pride bed");
        }
        if (ImGui::Button("Goth-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed";   
            printf("changed to goth bed");
        }
          if (ImGui::Button("Goth-Bed-II")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed2";   
            printf("changed to goth bed");
        }
    ImGui::EndPopup();
    }

//---------desk-------------
        ImGui::SetNextWindowSize(ImVec2(400, 100));
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("Desk-Decor-I")) {
        
         printf("desk select\n");
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Desk-SubMenu");
    }

    
    if (ImGui::BeginPopup("Desk-SubMenu")) {
        if (ImGui::Button("Boring-Desk-Lamp")) {
            //ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default Desk Accesory1"); 
        }
        if (ImGui::Button("Patriot-Lamp")) {
          //  ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default Desk Accesory1"); 
        }
        if (ImGui::Button("Rich-Snitch-Lamp")) {
          //  ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default Desk Accesory1"); 
        }
        if (ImGui::Button("Gaming-Lamp")) {
          //  ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default Desk Accesory1"); 
        }
        if (ImGui::Button("Pride-Lamp")) {
          //  ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";  
            printf("changed to default Desk Accesory1"); 
        }
    ImGui::EndPopup();
    }

       

    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render(); 
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(),render_pass);
   
}
