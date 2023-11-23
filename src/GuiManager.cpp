#include "GuiManager.h"
#include <iostream>
// #include "GuiManager.h"
#include <imgui.h>

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include "GraphicsManager.cpp"
#include <ctime>
#include <chrono>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>

#include "Engine.h" //to play sounds when changed

// time variables for progress bar update
std::chrono::system_clock::time_point start_time;
std::chrono::system_clock::time_point end_time;

std::string saveFilePathI = "inventory.json"; 
std::string saveFilePathB = "money.json"; 

//include this in the graphics manager, 
GuiManager::GuiManager()
{
    curr_bed = "";
    curr_lamp = "";
    curr_desk = "";
    curr_dresser = "";
    curr_fridge = "";
    curr_floor = "";
    curr_wall = "";
    curr_window = "";
  ///  std::cout << "help: " << ECS.Get<GraphicsManager::Sprite>(2).image_name << std::endl;

}

void GuiManager::Start(GLFWwindow *window, WGPUDevice device, WGPUTextureFormat swapchainformat)
{
     //initialize time things...
    maxStamina = 40.0f;
    replenish_rate = 1.0f;
    start_time = std::chrono::system_clock::now(); //set init start time to when gui manager starts

    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); 
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOther(window, true);
    WGPUTextureFormat ob = WGPUTextureFormat_Undefined; 
    ImGui_ImplWGPU_Init(device,3,swapchainformat, ob);    

    //have list of all items
    //iterate over it, anything not in the purchaced items list gets put in unpurchaced

    //all items -> in loops, if not purchaced already then make button
    
    //default items in inventory
    purchasedItems.push_back("boringbed");
    purchasedItems.push_back("boringlamp");
    purchasedItems.push_back("boringdesk");
    purchasedItems.push_back("boringdresser");
    purchasedItems.push_back("fridgetowel"); 
    purchasedItems.push_back("boringfloor");
    purchasedItems.push_back("boringwall");
    purchasedItems.push_back("boringsill");
}



float GuiManager::LoadEnergy() {
    std::ifstream energyFile("energy.txt");

    if (energyFile.is_open()) {
        std::string energyString;
        std::getline(energyFile, energyString);
        energyFile.close();
        return std::stof(energyString);
    }
    else {
        std::cerr << "Unable to read from energy file." << std::endl;
        return -1; //error
    }
}

time_t GuiManager::LoadTime()
{
    //read time saved to file into 
    std::ifstream timeFile("time.txt");
    if (timeFile.is_open()) {
        std::string timeString;
        std::getline(timeFile, timeString);
        timeFile.close();

        // convert string to time_t to read to file
        std::tm time = {};
        std::istringstream stringTime(timeString);
        stringTime >> std::get_time(&time, "%a %b %d %H:%M:%S %Y");

        return std::mktime(&time);    
    }
    else {
        std::cerr << "Unable to read from time.txt." << std::endl;
        return NULL;
    }
}

void GuiManager::SaveEnergy()
{
     std::ofstream energyFile("energy.txt");

    if (energyFile.is_open()) {
        std::cout << "Current energy: " << ECS.Get<EntityManager::Health>(0).percent << std::endl;
        energyFile << ECS.Get<EntityManager::Health>(0).percent;
        energyFile.close();
        std::cout << "Saved current energy level to file." << std::endl;
    }
    else {
        std::cerr << "Unable to open energy file to write current status." << std::endl;
    }
}

void GuiManager::InitEnergyBar()
{
     time_t lastLoad = LoadTime();
    time_t now = std::chrono::system_clock::to_time_t(start_time);
    double diff = difftime(now, lastLoad);
    std::cout << "Time between last shut down and now = " << diff << std::endl;

    //set up progress bar at start up...
    //get last energy
    float lastEnergy = LoadEnergy();
    std::cout << "Energy at last load = " << lastEnergy << std::endl;
    //set health -> the progress bar
    ECS.Get<EntityManager::Health>(0).percent = lastEnergy; //set to last loaded energy   

    //if time is greater than 30 minutes, reset to max
    // or if last energy is already the max stamina -> then it will be set to max stamina so cool beans
    if (diff > 1800) {
       ECS.Get<EntityManager::Health>(0).percent = maxStamina;
    } else {
        double time_away = diff;        
        //for every 30 seconds, add 1 (until max is reached)
        while (time_away >= 30)
        {
            time_away -= 30;
            ECS.Get<EntityManager::Health>(0).percent += 1;
            // if max, diff -> 0 to end loop
            if(ECS.Get<EntityManager::Health>(0).percent >= maxStamina){
                time_away = 0;
            }
        }
    }
}

void GuiManager::SaveTime()
{
    // save time of shutdown to file
    // write to file for storage
    std::ofstream timeFile("time.txt");

    if (timeFile.is_open())
    {
        auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        timeFile << std::ctime(&timenow);
        timeFile.close();
        std::cout << "Current time has been written to time.txt" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open file to write current time." << std::endl;
    }
}


void GuiManager::Shutdown()
{
    SaveEnergy(); //save energy at shutdown
    savePurchasedItems(saveFilePathI, purchasedItems);
    saveMoney(saveFilePathB);
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::ChangedItemSound(){
    GLOBAL_ENGINE.soundManager.PlaySound("chime");
}

void GuiManager::PurchasedItemSound(){
    GLOBAL_ENGINE.soundManager.PlaySound("twinkle");
}

void GuiManager::NoMoneySound(){
    GLOBAL_ENGINE.soundManager.PlaySound("buzzer");
}

//regular one after you call the else function ONCE
void GuiManager::DormShopSetter(const char * button_name, std::string item_name, std::string curritem, int price, int entitynum){
   
    if( isPurchased(item_name)== false){
        if (ImGui::Button(button_name)) { 
            if(ECS.Get<EntityManager::Money>(0).price >= price){ //has enough money
                      curritem = item_name;
                      ECS.Get<GraphicsManager::Sprite>(entitynum).image_name = item_name; //set image
                  
                    ECS.Get<EntityManager::Money>(0).price -= price; 
                    purchasedItems.push_back(item_name);
                    PurchasedItemSound();   
            } else {
                NoMoneySound(); //not enough money
            }
         } 
    }
    //needs to be outside of isPurchaced check
    if (ImGui::IsItemHovered()&& !isPurchased(item_name)) {

        ECS.Get<GraphicsManager::Sprite>(entitynum).image_name = item_name;  
 

    }

}
//
void GuiManager::CheckHovered(const char * button_name, std::string item_name, std::string curritem, int price, int entitynum){
    if(isPurchased(item_name)== false){
        if (ImGui::Button(button_name)) { 
            if(ECS.Get<EntityManager::Money>(0).price >= price){
                 curritem = item_name; 
                 ECS.Get<GraphicsManager::Sprite>(entitynum).image_name = item_name; //set image
               
                ECS.Get<EntityManager::Money>(0).price -= price; 
                purchasedItems.push_back(item_name);
                PurchasedItemSound();                            
            }  else {
                NoMoneySound(); //not enough money
            }
        }
        
    }
    if (ImGui::IsItemHovered() && !isPurchased(item_name)) { //outside of isPurchased 
        ECS.Get<GraphicsManager::Sprite>(entitynum).image_name = item_name;
    }
    else { // DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
        ECS.Get<GraphicsManager::Sprite>(entitynum).image_name = curritem; // Revert to temp when not hovered.
    }
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
    //anime -2 - lava lamp
    //cat lady -2
    //nerd alert -2 
    
    ROOM SPRITES: 
    BED
    LAMP
    HOBBY DESK
    DRESSER DECOR- HOBBY
   FRIDge DECOER
    RUG
    WINDOW DECOR -- plants(venus fly trap, catus, potted plant, clover, weed, christmasstocking)
    */

    //------------------------------ENERGY BAR-----------------------------//
    ImGui::SetNextWindowSize(ImVec2(800, 70)); 
    ImGui::Begin("Energy");
    ImGui::SetWindowPos(ImVec2(400, 0));
    
    //code to create label for progress bar
    int health_val = ECS.Get<EntityManager::Health>(0).percent; //casting to int for lopping off the excessive 0's
    std::string energy_string = std::to_string(health_val);
    const char* energy = energy_string.c_str(); // casting to const char* bc that is what ImGui needs apparently

    //udpate progress bar
    ImGui::ProgressBar(ECS.Get<EntityManager::Health>(0).percent / maxStamina, ImVec2(-1, 0), energy);

    //calculate current time
    end_time = std::chrono::system_clock::now();

    //if time is >= 2 minutes -> update energy and reset start time
    auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    //replenish over time
    //if (time_elapsed.count() >= 120) { //2 minutes
    if (time_elapsed.count() >= 30) { //30 seconds -> update every 30 seconds
    // if (time_elapsed.count() >= 5) { //5 seconds -> for testing
        ECS.Get<EntityManager::Health>(0).percent += replenish_rate;
        start_time = std::chrono::system_clock::now(); //update to start counter over
    }

    //clamp to maxStamina
    if (ECS.Get<EntityManager::Health>(0).percent > maxStamina) {
        ECS.Get<EntityManager::Health>(0).percent = maxStamina;
    }

    
    ImGui::End();
   //-----------------------------------------------------------------//

    ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::Begin("Dorm Shop!"); 
    ImGui::SetWindowPos(ImVec2(1300, 0));
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    ImGui::Text("Explore each of the \nfurniture categories.");

   //-------------BEDS---------------------------
    if (ImGui::Button("BED")) {      
         printf("bed\n");
         ImGui::OpenPopup("Bed-SubMenu");
    }
    if (ImGui::BeginPopup("Bed-SubMenu")) {



       CheckHovered("Patriot-Bed 1000$","patriotbed", curr_bed, 1000, 2); 
       DormShopSetter("Patriot-Bed-2 1050$", "patriotbed2", curr_bed, 1050, 2);
       DormShopSetter("Rich-B**** 1800$", "richsnitchbed", curr_bed, 1800, 2);
       DormShopSetter("Gamer-Bed 1400$", "gamerbed", curr_bed, 1400, 2);
       DormShopSetter("Gamer-Bed-2 1150$", "gamerbed2", curr_bed, 1150, 2);
       DormShopSetter("Stoner-Bed 1500$", "stonerbed", curr_bed, 1500, 2);
       DormShopSetter("Goth-Bed 1550$", "gothbed", curr_bed, 1550, 2);
       DormShopSetter("Goth-Bed-2 1300$", "gothbed2", curr_bed, 1300, 2);
       DormShopSetter("Halloween-Bed 1900$", "halloweenbed", curr_bed, 1900, 2);
       DormShopSetter("Christmas-Bed 1900$", "christmasbed", curr_bed, 1900, 2);
       DormShopSetter("Anime-Bed 1799$", "animebed", curr_bed, 1799, 2);
       DormShopSetter("Cat-Lady-Bed 1200$", "catbed", curr_bed, 1200, 2);
       DormShopSetter("Pink-Bed 1000$", "pinkbed", curr_bed, 1000, 2);
     
       

        ImGui::EndPopup();
    }

  //---------lamp-------------
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("LAMPS")) {
         printf("lamp select\n");
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Lamp-SubMenu");
    }

    
    if (ImGui::BeginPopup("Lamp-SubMenu")) {

       CheckHovered("Patriot-Lamp 500$","patriotlamp", curr_lamp, 500, 3); 
       DormShopSetter("Gamer-Lamp 550$", "gamerlamp", curr_lamp, 550, 3);
       DormShopSetter("Rich-B**** 590$", "richlamp", curr_lamp, 590, 3);
       DormShopSetter("Stoner-Lamp 560$", "stonerlamp", curr_lamp, 560, 3);
       DormShopSetter("Goth-Lamp 575$", "gothlamp", curr_lamp, 575, 3); 
       DormShopSetter("Halloween-Lamp 600$", "hallowlamp", curr_lamp, 600, 3);
       DormShopSetter("Christmas-Lamp 600$", "christmaslamp", curr_lamp, 600, 3);
       DormShopSetter("Anime-Lamp 580$", "animelamp", curr_lamp, 580, 3); 
       DormShopSetter("Cat-Lady-Lamp 550$", "catlamp", curr_lamp, 550, 3);
   
      

       ImGui::EndPopup();
    }
    
    //---------------desks-------------------

    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("DESK")) {
        
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Desk-SubMenu");
    }

    
    if (ImGui::BeginPopup("Desk-SubMenu")) {
        
       CheckHovered("Gamer-Desk-PC 950$","deskpc", curr_desk, 950, 4); 
       DormShopSetter("Gamer-Desk-Console 899$", "deskconsole", curr_desk, 800, 4);
       DormShopSetter("Patriot-Desk 800$", "desktv", curr_desk, 590, 4);
       DormShopSetter("Rich-B****-Desk 990$", "richdesk", curr_desk, 999, 4);
       DormShopSetter("Stoner-Desk 850$", "stonerdesk", curr_desk, 850, 4);
       DormShopSetter("Goth-Desk 860$", "gothdesk", curr_desk, 860, 4);
       DormShopSetter("Halloween-Desk 1000$", "hallowdesk", curr_desk, 590, 4); 
       DormShopSetter("Christmas-Desk 1000$", "christdesk", curr_desk, 1000, 4); 
       DormShopSetter("Anime-Desk 875$", "animedesk", curr_desk, 875, 4); 
       DormShopSetter("Cat-Lady-Desk 850$", "catdesk", curr_desk, 850, 4);
       //nerd books bulletin board
    
     
       ImGui::EndPopup();
    }

   
    //--------------Dresser---------------
    //min: 700 max:800
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("DRESSER")) {
        
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Dresser-SubMenu");
    }
    
    if (ImGui::BeginPopup("Dresser-SubMenu")) {
        CheckHovered("Patriot-Dresser 700$","patriotdresser", curr_dresser, 950, 5); 
        DormShopSetter("Gaming-Dresser 750$", "gamingdresser", curr_dresser, 800, 5);
        DormShopSetter("Rich-B****-Dresser 790$", "richdresser", curr_dresser, 790, 5);
        DormShopSetter("Stoner-Dresser 745$", "stonerdresser", curr_dresser, 745, 5);     
        DormShopSetter("Goth-Dresser 750", "gothdresser", curr_dresser, 750, 5);
        DormShopSetter("Halloween-Dresser 800$", "hallowdresser", curr_desk, 800, 5);
        DormShopSetter("Christmas-Dresser 800$", "christdresser", curr_desk, 800, 5);
        DormShopSetter("Anime-Dresser 780$", "animedresser", curr_dresser, 780, 5);  
        DormShopSetter("Cat-Lady-Dresser 750$", "catdresser", curr_dresser, 750, 5);
        DormShopSetter("Nerd-Dresser 780$", "nerddresser", curr_dresser, 780, 5);
        DormShopSetter("Jock-Dresser 780$", "jockdresser", curr_dresser, 780, 5);
        DormShopSetter("Ally-Dresser 780$", "allydresser", curr_dresser, 780, 5);
     
       
    
       ImGui::EndPopup();
    }
//----------------Kitcheen-------------------
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("FRIDGE")) {
         ImGui::OpenPopup("Fridge-SubMenu");
    }

    
    if (ImGui::BeginPopup("Fridge-SubMenu")) {  

        CheckHovered("Food 1200$","fridgefood", curr_fridge, 1200, 6); 
        DormShopSetter("Microwave 1300$", "fridgemicrowave", curr_fridge, 1300, 6);
        DormShopSetter("Coffee-Maker 1350$", "fridgecoffee", curr_fridge, 1350, 6);
        DormShopSetter("Crock-Pot 1400$", "fridgecrock", curr_fridge, 1400, 6);
        ImGui::EndPopup();
    }
   
    //------------FLOOR--------------
    //min = 400, max = 500 

    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("FLOOR")) {    
         ImGui::OpenPopup("Floor-SubMenu");
    }
    
    if (ImGui::BeginPopup("Floor-SubMenu")) {
        
        CheckHovered("Patriot-Floor 400$","patriotfloor", curr_floor, 400, 7); 
        DormShopSetter("Gaming-Floor 480$", "gamefloor", curr_floor, 480, 7);
        DormShopSetter("Rich-B****-Floor 490$", "richfloor", curr_floor, 490, 7);
        DormShopSetter("Stoner-Floor 470$", "stonerfloor", curr_floor, 470, 7);
        DormShopSetter("Goth-Floor 450$ ", "gothfloor", curr_floor, 450, 7); 
        DormShopSetter("Halloween-Floor 500$", "hallowfloor", curr_floor, 500, 7);
        DormShopSetter("Christmas-Floor 500$", "christfloor", curr_floor, 500, 7); 
        DormShopSetter("Anime-Floor 480$", "animefloor", curr_floor, 480, 7);
        DormShopSetter("Cat-Lady-Floor 460$", "catfloor", curr_floor, 460, 7);
      
        ImGui::EndPopup();
    }

   if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("WALL")) {    
         ImGui::OpenPopup("Wall-SubMenu");
    }
    
    if (ImGui::BeginPopup("Wall-SubMenu")) {
        CheckHovered("Patriot-Wall 200$","patriotwall", curr_wall, 200, 9);  
        DormShopSetter("Gaming-Wall 250$", "gamerwall", curr_wall, 250, 9);
        DormShopSetter("Rich-B****-Wall 290$", "richwall", curr_wall, 290, 9);
        DormShopSetter("Stoner-Wall 250$", "stonerwall", curr_wall, 250, 9); 
        DormShopSetter("Goth-Wall 270$ ", "gothwall", curr_wall, 270, 9); 
        DormShopSetter("Goth-Wall-II 270$ ", "gothwall2", curr_wall, 270, 9);   
        DormShopSetter("Halloween-Wall 300$", "hallowwall", curr_wall, 300, 9); 
        DormShopSetter("Christmas-Wall 300$", "christwall", curr_wall, 300, 9); 
        DormShopSetter("Anime-Wall 280$", "animewall", curr_wall, 280, 9); 
        DormShopSetter("Cat-Lady-Wall 260$", "catwall", curr_wall, 260, 9);
        DormShopSetter("Nerd-Wall 260$", "nerdwall", curr_wall, 260, 9);
        DormShopSetter("Undertale-Wall 350$", "undertwall", curr_wall, 350, 9);
        DormShopSetter("Marvel-Wall 350$", "marvelwall", curr_wall, 350, 9);
    
        ImGui::EndPopup();
    }

    if(ImGui::Button("WINDOW")) {    
         ImGui::OpenPopup("Wind-SubMenu");
    }
    
    if (ImGui::BeginPopup("Wind-SubMenu")) {
        CheckHovered("Cactus 100$", "cactussill", curr_window, 100, 8);
        
        DormShopSetter("Rose-Sill 150$", "rosesill", curr_window, 150, 8);
        DormShopSetter("Aloe-Sill 150$", "aloesill", curr_window, 150, 8);
        DormShopSetter("Bonsei-Sill 200$", "bonseisill", curr_window, 200, 8);
   

 
    
        ImGui::EndPopup();
    }


    ImGui::End(); //end dormshop window
    
    
    //-------------money------------------
    ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::Begin("Bread!"); 
     ImGui::SetWindowPos(ImVec2(1300,600));
    ImGui::SetCursorPos(ImVec2(5, 50));
    ImGui::Text("Crumbs: $%.2f", ECS.Get<EntityManager::Money>(0).price);
     if (ImGui::Button("Favor - 5 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=5){
            ECS.Get<EntityManager::Money>(0).price+=200;
            ECS.Get<EntityManager::Health>(0).percent-=5; 
        }
        //5 energy
        //100 crumbs

     }
     if (ImGui::Button("Work-Study - 10 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=10){
            ECS.Get<EntityManager::Money>(0).price+=400;
            ECS.Get<EntityManager::Health>(0).percent-=10; 
        }
        //10 energy
        //400 crumbs
     }
     if (ImGui::Button("Part-Time-Job - 15 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=15){
            ECS.Get<EntityManager::Money>(0).price+=700;
            ECS.Get<EntityManager::Health>(0).percent-=15; 
        }
        //15 energy
        //700 crumbs
     }
     if (ImGui::Button("Internship - 35 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=35){
            ECS.Get<EntityManager::Money>(0).price+=1200;
            ECS.Get<EntityManager::Health>(0).percent-=35;
        }
        //35 energy
        //1200 crumbs
     }

    ImGui::End(); 

    //====================INVENTORY===============================//
     ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::Begin("Inventory!"); 
     ImGui::SetWindowPos(ImVec2(1300, 300));
      if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
        if (ImGui::Button("BEDS")) {      
         ImGui::OpenPopup("Bed-Inventory");
        }
        if (ImGui::BeginPopup("Bed-Inventory")) {
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("bed") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                        std::cout << "i: " <<  purchasedItems[i]<< std::endl;
                        printf("Word contains Bed!\n");
                        ECS.Get<GraphicsManager::Sprite>(2).image_name = purchasedItems[i];
                        curr_bed = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }
         ImGui::EndPopup();
        }

        if (ImGui::Button("LAMPS")) {      
            ImGui::OpenPopup("lamp-Inventory");
        }
        if (ImGui::BeginPopup("lamp-Inventory")) {
       
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("lamp") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                    
                        printf("Word contains lamp!\n");
                        ECS.Get<GraphicsManager::Sprite>(3).image_name = purchasedItems[i];
                        curr_lamp = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("DESKS")) {      
         ImGui::OpenPopup("desk-Inventory");
        }
        if (ImGui::BeginPopup("desk-Inventory")) {
       
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("desk") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                    
                        printf("Word contains desk!\n");
                        ECS.Get<GraphicsManager::Sprite>(4).image_name = purchasedItems[i];
                        curr_desk = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }

            ImGui::EndPopup();
        }
        
        if (ImGui::Button("DRESSERS")) {      
         ImGui::OpenPopup("dress-Inventory");
        }
        if (ImGui::BeginPopup("dress-Inventory")) {
       
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("dresser") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                    
                        printf("Word contains dresser!\n");
                        ECS.Get<GraphicsManager::Sprite>(5).image_name = purchasedItems[i];
                        curr_dresser = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("FRIDGES")) {      
         ImGui::OpenPopup("fridge-Inventory");
        }
        if (ImGui::BeginPopup("fridge-Inventory")) {
       
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("fridge") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                        printf("Word contains fridge!\n");
                        ECS.Get<GraphicsManager::Sprite>(6).image_name = purchasedItems[i];
                        curr_fridge = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("FLOORS")) {      
         ImGui::OpenPopup("floor-Inventory");
        }
        if (ImGui::BeginPopup("floor-Inventory")) {
       
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("floor") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                    
                        printf("Word contains floor!\n");
                        ECS.Get<GraphicsManager::Sprite>(7).image_name = purchasedItems[i];
                        curr_floor = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }

            ImGui::EndPopup();
        }

         if (ImGui::Button("WALLS")) {      
         ImGui::OpenPopup("Wall-Inventory");
        }
        if (ImGui::BeginPopup("Wall-Inventory")) {
       
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("wall") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                    
                        printf("Word contains wall!\n");
                        ECS.Get<GraphicsManager::Sprite>(9).image_name = purchasedItems[i];
                        curr_wall = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("WINDOWS")) {      
         ImGui::OpenPopup("Window-Inventory");
        }

        if (ImGui::BeginPopup("Window-Inventory")) {
       
            for(int i = 0; i< purchasedItems.size(); i++){
                std::string tempstri = purchasedItems[i];
                if(purchasedItems[i].find("sill") != std::string::npos){
                    if (ImGui::Button(tempstri.c_str())) {//Shadai 
                    
                        printf("Word contains window!\n");
                        ECS.Get<GraphicsManager::Sprite>(8).image_name = purchasedItems[i];
                        curr_window = purchasedItems[i];
                        ChangedItemSound();
                    }
                }
            }

            ImGui::EndPopup();
        }

    ImGui::End(); 
    ImGui::EndFrame(); 
    ImGui::Render(); 
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(),render_pass);
   
 }

void GuiManager::SetTemp()
{
   // std::cout << "checking: " <<  ECS.Get<GraphicsManager::Sprite>(2).image_name << std::endl;
    curr_bed =  ECS.Get<GraphicsManager::Sprite>(2).image_name ; // bed
    curr_lamp =  ECS.Get<GraphicsManager::Sprite>(3).image_name ; //lamp
    curr_desk = ECS.Get<GraphicsManager::Sprite>(4).image_name; //desk
    curr_dresser =  ECS.Get<GraphicsManager::Sprite>(5).image_name;  //dresser
    curr_fridge = ECS.Get<GraphicsManager::Sprite>(6).image_name; //fridge
    curr_floor = ECS.Get<GraphicsManager::Sprite>(7).image_name; //floor
    curr_wall = ECS.Get<GraphicsManager::Sprite>(9).image_name;
    curr_window= ECS.Get<GraphicsManager::Sprite>(8).image_name;
}
//using json = nlohmann::json;

void GuiManager::loadPurchasedItems(const std::string& filename)
{
    std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }

        // Parse the JSON data
        nlohmann::json jsonData;
        try {
                        // Check if the file is empty
            if (file.peek() == std::ifstream::traits_type::eof()) {
                std::cerr << "File is empty." << std::endl;
                file.close();
                return;
            }else{
                file >> jsonData;
            }
            
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "Error parsing JSON data: " << e.what() << std::endl;
            file.close();

        }

        file.close();

        // Check if the loaded data is an array of strings
        if (!jsonData.is_array()) {
            std::cerr << "Data is not in the expected array format." << std::endl;
        }

       jsonData.get_to(purchasedItems);
}

void GuiManager::savePurchasedItems(const std::string& filename, const std::vector<std::string>& stringList)
{
    json jsonData = stringList;

        std::ofstream file(filename);
        if (file.is_open()) {
            file << jsonData.dump(4); // Dump the JSON with indentation for readability
            file.close();
        }
}

bool GuiManager::isPurchased(std::string item)
{
    for(int j = 0; j< purchasedItems.size(); j++){
        if(purchasedItems[j]== item){
            return true; 
        }
    }
        return false;
}

void GuiManager::saveMoney(const std::string &filename)
{
       json moneyData;
       json moneyInfo;
       moneyInfo["money"] = ECS.Get<EntityManager::Money>(0).price;
       moneyData.push_back(moneyInfo);

       std::ofstream file(filename);
       if (file.is_open()) {
        file << moneyData.dump(4); // Dump the JSON with indentation for readability
        file.close();
       }
}

void GuiManager::loadMoney(const std::string &filename)
{
    std::ifstream file(filename);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open money file: " << filename << std::endl;
        return;
    }

    // Parse the JSON data
    nlohmann::json moneyData;

    try {
        file >> moneyData;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing JSON data: " << e.what() << std::endl;
        file.close();
        return;
    }

    file.close();
    if (!moneyData.is_array()) {
            std::cerr << "Sprite data is not in the expected array format." << std::endl;
            return;
    }

    for (const auto& moneyInfo : moneyData) {
        if (moneyInfo.is_object()) {
            float bread = moneyInfo["money"];
            ECS.Get<EntityManager::Money>(0).price = bread; 
        }

    }



}


