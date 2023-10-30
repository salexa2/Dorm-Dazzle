#include "GuiManager.h"
#include <iostream>
#include "GuiManager.h"
#include <imgui.h>

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include "GraphicsManager.cpp"
#include <chrono>
#include <thread>



//include this in the graphics manager, 
GuiManager::GuiManager()
{
    temp = "";
  ///  std::cout << "help: " << ECS.Get<GraphicsManager::Sprite>(2).image_name << std::endl;

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
        //------------------boring bed--------------------------
        if (ImGui::Button("Boring-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed"; 
            temp =   "bed"; 
            printf("changed to default bed\n"); 
        }
        if(ImGui::IsItemHovered()) {
            printf("hoveredborin\n");
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "bed";    
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
             printf("not hovered - goth\n");
             ECS.Get<GraphicsManager::Sprite>(2).image_name = temp; // Revert to temp when not hovered.
        }
        //-------------------patriotbed-----------------
        if (ImGui::Button("Patriot-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";
            temp = "patriotbed";
            printf("changed to patriot bed\n"); 
           
        } if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";    
        }
        //-------------------Patriotbed2------------------------
        if (ImGui::Button("Patriot-Bed-II")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";   
            printf("changed to patriot bed2");
            temp = "patriotbed2";
        }if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";    
        }
        //--------------------richsnitch---------------------
        if (ImGui::Button("Rich-Snitch")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";   
            printf("changed to rich snitch");
            temp = "richsnitchbed";
        }if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";    
        }
        //--------------------------Gamerbed-----------------------
         if (ImGui::Button("Gamer-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed";   
            printf("changed to gamer bed");
            temp = "gamerbed";
        }if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed";    
        }
        //----------------------Gamerbed2------------------------
         if (ImGui::Button("Gamer-Bed-II")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed2";   
            printf("changed to gamer bed2");
            temp = "gamerbed2";
         }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed2";    
        }
        //-----------------rainbowbed------------------------------
        if (ImGui::Button("Rainbow-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "pridebed";   
            printf("changed to pride bed");
            temp =  "pridebed";
           // printf("temp:  ",temp);
        }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "pridebed";    
        }
        //---------------Gothbed-----------------------------
        if (ImGui::Button("Goth-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed";   
            printf("changed to goth bed");
            temp = "gothbed";
        }
        if(ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed";    
        }
        //-----------------Gothbed2---------------------------
         if (ImGui::Button("Goth-Bed-II")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed2";   
            printf("changed to goth bed2");
            temp = "gothbed2";
        }
        if (ImGui::IsItemHovered()) {
            //printf("hovered");
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed2";    
        }
      
        //----------------------Halloween-------------------
        if (ImGui::Button("Halloween-Bed")) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "halloweenbed";  
            printf("changed to Halloween bed"); 
            temp = "halloweenbed";
        }
        if(ImGui::IsItemHovered()) {// allows user to preview the decor before the purchase it! 
           // printf("hovered hallowe");
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "halloweenbed";     
             
         }
      
        //----------------Christmas---------------------------

        if (ImGui::Button("Christmas-Bed")) {
        
            printf("changed to Christmas bed");
            //temp = "christmasbed";
        }
        if (ImGui::Button("Gym-Rat-Bed")) {
        
            printf("changed to Gym Rat bed"); 
            //temp = "gymratbed";
        }
        if (ImGui::Button("Anime-Bed")) {
        
            printf("changed to anime bed"); 
            //temp = "animebed";
        }
        if (ImGui::Button("Cat-Lady-Bed")) {
        
            printf("changed to cat lady bed"); 
           // temp = "catladybed";
        }
        if (ImGui::Button("Nerdy-Bed")) {
        
            printf("changed to nerdy lamp"); 
            //temp = "nerdybed";
        }

        
    ImGui::EndPopup();
    }
  
  
//---------desk-------------
        ImGui::SetNextWindowSize(ImVec2(400, 100));
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("LAMPS")) {
        
         printf("lamp select\n");
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Lamp-SubMenu");
    }

    
    if (ImGui::BeginPopup("Lamp-SubMenu")) {
        if (ImGui::Button("Boring-Lamp")) {
            ECS.Get<GraphicsManager::Sprite>(3).image_name = "boringlamp";  
            printf("changed to default lamp"); 
        }
        if (ImGui::Button("Patriot-Lamp")) {
         ;  
            printf("changed to patriot lamp"); 
        }
        if (ImGui::Button("Rich-Snitch-Lamp")) {
          
            printf("changed to rich snitch lamp"); 
        }
        if (ImGui::Button("Gaming-Lamp")) {
          
            printf("changed to gaming lamp"); 
        }
        if (ImGui::Button("Rainbow-Lamp")) {
        
            printf("changed to rainbow lamp"); 
        }
         if (ImGui::Button("Goth-Lamp")) {
        
            printf("changed to gothlamp"); 
        }
         if (ImGui::Button("Halloween-Lamp")) {
        
            printf("changed to Halloween lamp"); 
        }
        if (ImGui::Button("Christmas-Lamp")) {
        
            printf("changed to Christmas lamp"); 
        }
        if (ImGui::Button("Gym-Rat-Lamp")) {
        
            printf("changed to Gym Rat lamp"); 
        }
        if (ImGui::Button("Anime-Lamp")) {
        
            printf("changed to anime lamp"); 
        }
        if (ImGui::Button("Cat-Lady-Lamp")) {
        
            printf("changed to cat lady"); 
        }
        if (ImGui::Button("Nerdy-Lamp")) {
        
            printf("changed to nerdy lamp"); 
        }
    ImGui::EndPopup();
    }
    ImGui::End(); 
    //-------------money------------------
    ImGui::Begin("Bread!"); 
    ImGui::SetCursorPos(ImVec2(5, 50));
    ImGui::Text("Crumbs: $%.2f", ECS.Get<EntityManager::Money>(0).price);
     if (ImGui::Button("Favor")) {
        //5 energy
        //100 crumbs

     }
     if (ImGui::Button("Work-Study")) {
        //10 energy
        //200 crumbs
     }
     if (ImGui::Button("Part-Time-Job")) {
        //15 energy
        //300 crumbs
     }
     if (ImGui::Button("Internship")) {
        //25 energy
        //800 crumbs
     }

    ImGui::End(); 

    ImGui::EndFrame(); 
    ImGui::Render(); 
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(),render_pass);
   
}

void GuiManager::SetTemp()
{
    std::cout << "checking: " <<  ECS.Get<GraphicsManager::Sprite>(2).image_name << std::endl;
    temp =  ECS.Get<GraphicsManager::Sprite>(2).image_name ;
}
