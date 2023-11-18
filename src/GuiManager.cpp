#include "GuiManager.h"
#include <iostream>
// #include "GuiManager.h"
#include <imgui.h>

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include "GraphicsManager.cpp"
#include <ctime>
#include <chrono>
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
    temp = "";
    temp2 = "";
    temp3 = "";
    temp4 = "";
    temp5 = "";
    temp6 = "";
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
    
    //default items in inventory
    purchasedItems.push_back("boringbed");
    purchasedItems.push_back("boringlamp");
    purchasedItems.push_back("boringdesk");
    purchasedItems.push_back("boringdresser");
    purchasedItems.push_back("fridgetowel"); 
    purchasedItems.push_back("boringfloor");
    purchasedItems.push_back("fridgetowel");
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
   FRIDge DECOER
    RUG
    WINDOW DECOR -- plants(venus fly trap, catus, potted plant, clover, weed, christmasstocking)
    */

    //------------------------------ENERGY BAR-----------------------------//
    ImGui::Begin("Energy");
    ImGui::SetNextWindowSize(ImVec2(200, 30));
    ImGui::SetWindowPos(ImVec2(500, 0));
    
    //code to create label for progress bar
    int health_val = ECS.Get<EntityManager::Health>(0).percent; //casting to int for "rouding down"
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
    // if (time_elapsed.count() >= 30) { //30 seconds -> update every 30 seconds
    if (time_elapsed.count() >= 5) { //5 seconds -> for testing
        ECS.Get<EntityManager::Health>(0).percent += replenish_rate;
        start_time = std::chrono::system_clock::now(); //update to start counter over
    }

    //clamp to maxStamina
    if (ECS.Get<EntityManager::Health>(0).percent > maxStamina) {
        ECS.Get<EntityManager::Health>(0).percent = maxStamina;
    }

    
    ImGui::End();
   //-----------------------------------------------------------------//

    ImGui::SetNextWindowSize(ImVec2(200, 400));
    ImGui::Begin("Dorm Shop!"); 
    
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    ImGui::Text("Explore each of the \nfurnature categories.");

   //-------------BEDS---------------------------
    if (ImGui::Button("BED")) {      
         printf("bed\n");
         ImGui::OpenPopup("Bed-SubMenu");
    }
    if (ImGui::BeginPopup("Bed-SubMenu")) {
        //-------------------patriotbed-----------------
         if( isPurchased("patriotbed")== false){
            if (ImGui::Button("Patriot-Bed 1000$" )) {
                if( isPurchased("patriotbed")== false){
                            //if item not already purchased 
                    if(ECS.Get<EntityManager::Money>(0).price >=1000){
                            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";
                            temp = "patriotbed";
                            printf("purchased patriot bed\n"); 
                            ECS.Get<EntityManager::Money>(0).price-=1000; 
                            purchasedItems.push_back("patriotbed");
                            PurchasedItemSound();                            
                    }else{
                                printf("Can't afford patriot bed!\n");
                    }
                 }
            }

        } 
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";    
        }
        else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
             ECS.Get<GraphicsManager::Sprite>(2).image_name = temp; // Revert to temp when not hovered.
        }
        //-------------------Patriotbed2------------------------
        if( isPurchased("patriotbed2")== false){
            if (ImGui::Button("Patriot-Bed-II 1050$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1050){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";   
                    printf("changed to patriot bed2");
                    temp = "patriotbed2";
                    ECS.Get<EntityManager::Money>(0).price-=1050; 
                    purchasedItems.push_back("patriotbed2");
                    PurchasedItemSound();
                }
                else{
                    printf("Can't afford patriotbed2");
                }

            }
        }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";    
        }
        //--------------------richsnitch---------------------
        if( isPurchased("richsnitchbed")== false){
            if (ImGui::Button("Rich-B**** 1800$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1800){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";   
                    printf("changed to rich snitch");
                    temp = "richsnitchbed";
                    ECS.Get<EntityManager::Money>(0).price-=1800; 
                     purchasedItems.push_back("richsnitchbed");
                     PurchasedItemSound();
                }
            }
        }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";    
        }
        //--------------------------Gamerbed-----------------------
        if( isPurchased("gamerbed")== false){
            if (ImGui::Button("Gamer-Bed 1400$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1150){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed";   
                    printf("changed to gamer bed");
                    temp = "gamerbed";
                    ECS.Get<EntityManager::Money>(0).price-=1150; 
                     purchasedItems.push_back("gamerbed");
                     PurchasedItemSound();
                }
            }
        }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed";    
        }
        //----------------------Gamerbed2------------------------
         if( isPurchased("gamerbed2")== false){
            if (ImGui::Button("Gamer-Bed-II 1150$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1250){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed2";   
                    printf("changed to gamer bed2");
                    temp = "gamerbed2";
                    ECS.Get<EntityManager::Money>(0).price-=1250; 
                    purchasedItems.push_back("gamerbed2");
                    PurchasedItemSound();
                }

            }
         }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed2";    
        }
        //-----------------rainbowbed------------------------------
         if( isPurchased("pridebed")== false){
            if (ImGui::Button("Rainbow-Bed 1500$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1500){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "pridebed";   
                    printf("changed to pride bed");
                    temp =  "pridebed";
                    ECS.Get<EntityManager::Money>(0).price-=1500; 
                    purchasedItems.push_back("pridebed");
                    PurchasedItemSound();
                }
            // printf("temp:  ",temp);
            }
         }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "pridebed";    
        }
        //---------------Gothbed-----------------------------
        if( isPurchased("gothbed")== false){
            if (ImGui::Button("Goth-Bed 1550$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1300){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed";   
                    printf("changed to goth bed");
                    temp = "gothbed";
                    ECS.Get<EntityManager::Money>(0).price-=1300; 
                    purchasedItems.push_back("gothbed");
                    PurchasedItemSound();
                }
            }
        }
        if(ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed";    
        }
        //-----------------Gothbed2---------------------------
         if( isPurchased("gothbed2")== false){
            if (ImGui::Button("Goth-Bed-II 1300$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1500){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed2";   
                    printf("changed to goth bed2");
                    temp = "gothbed2";
                    ECS.Get<EntityManager::Money>(0).price-=1500;
                    purchasedItems.push_back("gothbed2");
                    PurchasedItemSound();
                }
            }
        }
        if (ImGui::IsItemHovered()) {
            //printf("hovered");
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed2";    
        }
      
        //----------------------Halloween-------------------
         if( isPurchased("halloweenbed")== false){
            if (ImGui::Button("Halloween-Bed 1900$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1900){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "halloweenbed";  
                    printf("changed to Halloween bed"); 
                    temp = "halloweenbed";
                    ECS.Get<EntityManager::Money>(0).price-=1900; 
                    purchasedItems.push_back("halloweenbed");
                    PurchasedItemSound();
                }
            }
        }
        if(ImGui::IsItemHovered()) {// allows user to preview the decor before the purchase it! 
           // printf("hovered hallowe");
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "halloweenbed";     
             
        }
      
        //----------------Christmas---------------------------
        /*
        TIPS: GET CREATIVE WITH IT, YOU CAN UPLOAD PNGS TO THE WEBSITE TO DECORATE THE SPRITES OR YOU CAN 
        JUST USE WHATEVERS ALREADY ON THE SITE TO MAKE SOMETHING, FOR EX: IF YOU'RE WORKING ON AN ANIME LAMP WHICH ISUGGEST GETTING A LAVA
        LAMP FOR SEARCH UP 2D OR CARTOON OR ISOMETRIC LAVA LAMP PNGS. SAME WITH ADDING PATTERNS AND SUCH!, REMEMBER DO NOT MOVE THE SPRITE
        IN THE JSON FILE, WE WANNA MAKE SURE ALL THE SPRITES HAVE THE SAME POSITION! 
        */
        if (ImGui::Button("Christmas-Bed 1900$")) {
        
            printf("changed to Christmas bed");
            //temp = "christmasbed";
        }
        if (ImGui::Button("Gym-Rat-Bed 1300$")) {
        
            printf("changed to Gym Rat bed"); 
            //temp = "gymratbed";
        }
        if (ImGui::Button("Anime-Bed 1800$")) {
        
            printf("changed to anime bed"); 
            //temp = "animebed";
        }
        if (ImGui::Button("Cat-Lady-Bed 1550$")) {
        
            printf("changed to cat lady bed"); 
           // temp = "catladybed";
        }
        if (ImGui::Button("Nerdy-Bed 1400$")) {
        
            printf("changed to nerdy bed"); 
            //temp = "nerdybed";
        }

        
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
        //-------------patriotlamp----------------
         if( isPurchased("patriotlamp")== false){
            if (ImGui::Button("Patriot-Lamp 500$")) { //shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=500){
                    ECS.Get<GraphicsManager::Sprite>(3).image_name = "patriotlamp";  
                    printf("changed to patriot lamp"); 
                    temp2 = "patriotlamp";
                    ECS.Get<EntityManager::Money>(0).price-=500; 
                    purchasedItems.push_back("patriotlamp");
                    PurchasedItemSound();
                }
            }
         }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(3).image_name = "patriotlamp"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(3).image_name = temp2; // Revert to temp when not hovered.
        }

       //-------------richlamp-----------------
        if( isPurchased("richlamp")== false){
            if (ImGui::Button("Rich-B**** Lamp 590$")) {//shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=590){
                        /*
                        printf("changed to rich snitch lamp"); 
                        ECS.Get<GraphicsManager::Sprite>(3).image_name = "richlamp";  
                        temp2 = "richlamp";
                        ECS.Get<EntityManager::Money>(0).price-=590; 
                        purchasedItems.push_back("richlamp");
                        PurchacedItemSound();
                        */
                }      
            }
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "richlamp"; 
        }

        //-------------Gamer lamp--------------------
        if( isPurchased("gamerlamp")== false){
            if (ImGui::Button("Gaming-Lamp 550$")) {//shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=550){
                    printf("changed to gaming lamp"); 
                    ECS.Get<GraphicsManager::Sprite>(3).image_name = "gamerlamp";  
                    temp2 = "gamerlamp";
                    ECS.Get<EntityManager::Money>(0).price-=550; 
                    purchasedItems.push_back("gamerlamp");
                    PurchasedItemSound();
                }

            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(3).image_name = "gamerlamp"; 
        }

        //------------------Rainbow lamp------------------
         if( isPurchased("pridelamp")== false){
            if (ImGui::Button("Rainbow-Lamp 560$")) { //sahdai 
            
                //rainbow cloud lamp 
                /*if(ECS.Get<EntityManager::Money>(0).price >=560){
                    printf("changed to rainbow lamp"); 
                    ECS.Get<GraphicsManager::Sprite>(3).image_name = "pridelamp";  
                    temp2 = "pridelamp";
                    ECS.Get<EntityManager::Money>(0).price-=560; 
                    purchasedItems.push_back("pridelamp");
                    PurchacedItemSound();
                } 
                */
                
            }
         }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "pridelamp"; 
        }
          
        //------------Goth Lamp------------
         if( isPurchased("gothlamp")== false){
            if (ImGui::Button("Goth-Lamp 575$")) {//shadai 
                //candles
                /*if(ECS.Get<EntityManager::Money>(0).price >=565){
                    printf("changed to goth lamp"); 
                    ECS.Get<GraphicsManager::Sprite>(3).image_name = "gothlamp";  
                    temp2 = "gothlamp";
                    ECS.Get<EntityManager::Money>(0).price-=565; 
                    purchasedItems.push_back("gothlamp");
                } 
                */
                
            }
         }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "gothlamp"; 
        }


        //--------------Halloween Lamp --------------
         if( isPurchased("hallowlamp")== false){
            if (ImGui::Button("Halloween-Lamp 600$")) { //shadai 
                //jackolantern
                /*if(ECS.Get<EntityManager::Money>(0).price >=600){
                    printf("changed to halloween lamp"); 
                    ECS.Get<GraphicsManager::Sprite>(3).image_name = "hallowlamp";  
                    temp2 = "hallowlamp";
                    ECS.Get<EntityManager::Money>(0).price-=600; 
                    purchasedItems.push_back("hallowlamp");
                } 
                */
                
            }
         }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "hallowlamp"; 
        }
        

        
       /* READ ME- pick prices, minimum price = 500 max = 600, special holiday items should be the max price*/
       //---------------Christmas Lamp -----------------
        if (ImGui::Button("Christmas-Lamp")) {
        
            printf("changed to Christmas lamp");  //jen
        }
        if (ImGui::Button("Gym-Rat-Lamp")) { //jen
        
            printf("changed to Gym Rat lamp"); 
        }
        if (ImGui::Button("Anime-Lamp")) {//jen
         //lave lamp
            printf("changed to anime lamp"); 
        }
        if (ImGui::Button("Cat-Lady-Lamp")) {//jen
        
            printf("changed to cat lady"); 
        }
        if (ImGui::Button("Nerdy-Lamp")) {//jen
         //theres a sticky note object on the website totes put that behind the lamp!
            printf("changed to nerdy lamp"); //jen
        }
       ImGui::EndPopup();
    }
    
    //---------------desks-------------------

    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("DESK")) {
        
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Desk-SubMenu");
    }

    
    if (ImGui::BeginPopup("Desk-SubMenu")) {
         if( isPurchased("desktv")== false){
            if (ImGui::Button("Patriot-Desk 800$")) { // shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=800){
                    printf("changed to patriot Desk"); 
                    ECS.Get<GraphicsManager::Sprite>(4).image_name = "desktv"; 
                    temp3 = "desktv";
                    ECS.Get<EntityManager::Money>(0).price-=800; 
                    purchasedItems.push_back("desktv");
                    PurchasedItemSound();
                }        
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(4).image_name = "desktv"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(4).image_name = temp3; // Revert to temp when not hovered.
        }     
        //--------------richdesk-------
         if( isPurchased("richdesk")== false){
            if (ImGui::Button("Rich-B*****-Desk 850$")) {//shadai 
                /*
                if(ECS.Get<EntityManager::Money>(0).price >=850){
                    printf("changed to gaming desk 2"); 
                    ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk";  
                    temp3 = "richdesk";
                    ECS.Get<EntityManager::Money>(0).price-=850; 
                    purchasedItems.push_back("richdesk");
                } 
                */
                
            }
         }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk"; 
        }
        
        //--------Gaming desk 1 ------------
         if( isPurchased("deskpc")== false){
            if (ImGui::Button("Gaming-Desk-I 950$")) {//shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=950){
                    printf("changed to gaming Desk 1"); 
                    ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskpc"; 
                    temp3 = "deskpc";
                    ECS.Get<EntityManager::Money>(0).price-=950; 
                    purchasedItems.push_back("deskpc");
                    PurchasedItemSound();
                }        
            }
         }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskpc";
        }

        //-----------Gaming desk 2------------
        if( isPurchased("deskconsole")== false){
            if (ImGui::Button("Gaming-Desk-II 875$")) { //shadai 
                //Console
                if(ECS.Get<EntityManager::Money>(0).price >=875){
                    printf("changed to gaming desk2"); 
                    ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskconsole"; 
                    temp3 = "deskconsole";
                    ECS.Get<EntityManager::Money>(0).price-=875; 
                    purchasedItems.push_back("deskconsole");
                    PurchasedItemSound();
                }        
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskconsole"; 
        }
        
        //------------Rainbow desk----------------------
         if( isPurchased("richdesk")== false){
            if (ImGui::Button("Rainbow-Desk 860$")) { //shadai 
                /*
                if(ECS.Get<EntityManager::Money>(0).price >=850){
                    printf("changed to rainbow desk"); 
                    ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk";  
                    temp3 = "richdesk";
                    ECS.Get<EntityManager::Money>(0).price-=850; 
                    purchasedItems.push_back("richdesk");
                } 
                */
                
            }
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk"; 
        }
        //------------goth desk---------------
         if( isPurchased("gothdesk")== false){
            if (ImGui::Button("Goth-Desk 870$")) { // shadai 
                //maybe - ritual - skull withe pencils paper, a thick book 
                /*
                if(ECS.Get<EntityManager::Money>(0).price >=870){
                    printf("changed to goth desk"); 
                    ECS.Get<GraphicsManager::Sprite>(4).image_name = "gothdesk";  
                    temp3 = "gothdesk";
                    ECS.Get<EntityManager::Money>(0).price-=870; 
                    purchasedItems.push_back("gothdesk");
                } 
                */
                
            }
         }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(4).image_name = "gothdesk"; 
        }
        //----------Halloween desk--------------
         if( isPurchased("hallowdesk")== false){
            if (ImGui::Button("Halloween-Desk")) { //shadai 
                //potion brew
            /*
                if(ECS.Get<EntityManager::Money>(0).price >=1000){
                    printf("changed to halloween desk"); 
                    ECS.Get<GraphicsManager::Sprite>(4).image_name = "hallowdesk";  
                    temp3 = "hallowdesk";
                    ECS.Get<EntityManager::Money>(0).price-=1000; 
                    purchasedItems.push_back("hallowdesk");
                } 
                */
                
            }
         }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(4).image_name = "hallowdesk"; 
        }
        
          
        /* READ ME-  pick prices, minimum price = 800 max = 1000, special holiday items should be the max price*/
        //------------------Christmas Desk----------------
       
        if (ImGui::Button("Christmas-Desk")) { //jen 
            //bunch of presents
            printf("changed to Christmas desk"); 
        }
        if (ImGui::Button("Gym-Rat-Desk")) { //jen 
            //dumbbell set and a phone ?
            printf("changed to Gym Rat desk"); 
        }
        if (ImGui::Button("Anime-Desk")) {
            //ipad, bunch of manga books + white pencil  + robot ref figure(On the website)  //jen
            printf("changed to anime desk"); 
        }
        if (ImGui::Button("Cat-Lady-desk")) {
            //knitting gear plus cat figure //jen 
        
            printf("changed to cat lady"); 
        }
        if (ImGui::Button("Nerdy-Desk")) {
            //whiteboard+ calculator, text books stacked //jen
            printf("changed to nerdy desk"); 
        }
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
        if( isPurchased("patriotdresser")== false){
            if (ImGui::Button("Patriot-Dresser 700$")) { // shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=700){
                        printf("changed to patriotdresser"); 
                        ECS.Get<GraphicsManager::Sprite>(5).image_name = "patriotdresser";  
                        temp4 = "patriotdresser";
                        ECS.Get<EntityManager::Money>(0).price-=700; 
                        purchasedItems.push_back("patriotdresser");
                        PurchasedItemSound();
                } 
                    
                
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(5).image_name = "patriotdresser"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(5).image_name = temp4; // Revert to temp when not hovered.
        }  
        //--------rich snitch--------
        if( isPurchased("richdresser")== false){
            if (ImGui::Button("Rich-Snitch-Dresser 790$")) {//shadai
             /*if(ECS.Get<EntityManager::Money>(0).price >=790){
                        printf("changed to rich Desk"); 
                        ECS.Get<GraphicsManager::Sprite>(5).image_name = "richdresser";  
                        temp4 = "richdresser";
                        ECS.Get<EntityManager::Money>(0).price-=890; 
                        purchasedItems.push_back("richdresser");
                }  
                // jewlery */
               
            }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(5).image_name = "richdresser"; 
        }
        //--------gaming dresser----------------
        if( isPurchased("gamingdresser")== false){
            if (ImGui::Button("Gaming-Dresser-I 750$")) {//shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=750){
                    ECS.Get<GraphicsManager::Sprite>(5).image_name = "gamingdresser";
                    printf("changed to gaming Desk"); 
                    temp4 = "gamingdresser";
                    ECS.Get<EntityManager::Money>(0).price-=750; 
                    purchasedItems.push_back("gamingdresser");
                    PurchasedItemSound();
                } 
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(5).image_name = "gamingdresser"; 
        }
       //-------rainbow dresser----------
        if( isPurchased("pridedresser")== false){
            if (ImGui::Button("Rainbow-Dresser 750$")) {//shadai
             /*if(ECS.Get<EntityManager::Money>(0).price >=750){
                        printf("changed to rainbow dresser"); 
                        ECS.Get<GraphicsManager::Sprite>(5).image_name = "pridedresser";  
                        temp4 = "pridedresser";
                        ECS.Get<EntityManager::Money>(0).price-=750; 
                        purchasedItems.push_back("pridedresser");
                }  
               */
            }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(5).image_name = "pridedresser"; 
        }
        //----------goth dresser----------------------
        //ritual items/alter, skull 
         if( isPurchased("gothdresser")== false){
            if (ImGui::Button("Goth-Dresser 780$")) {//shadai
             /*if(ECS.Get<EntityManager::Money>(0).price >=780){
                        printf("changed to goth dresser"); 
                        ECS.Get<GraphicsManager::Sprite>(5).image_name = "gothdresser";  
                        temp4 = "gothdresser";
                        ECS.Get<EntityManager::Money>(0).price-=780; 
                        purchasedItems.push_back("gothdresser");
                }  
               */
            }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(5).image_name = "hallowdresser"; 
        }
        //----------halloween dresser-----------------
        //ouija board
           if( isPurchased("hallowdresser")== false){
            if (ImGui::Button("Halloween-Dresser 800$")) {//shadai
             /*if(ECS.Get<EntityManager::Money>(0).price >=800){
                        printf("changed to rainbow dresser"); 
                        ECS.Get<GraphicsManager::Sprite>(5).image_name = "hallowdresser";  
                        temp4 = "hallowdresser";
                        ECS.Get<EntityManager::Money>(0).price-=800; 
                        purchasedItems.push_back("hallowdresser");
                }  
               */
            }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(5).image_name = "hallowdresser"; 
        }
        //----------christmas------------
        if (ImGui::Button("Christmas-Dresser")) { //jenn
            //bunch of presents
            printf("changed to Christmas dresser"); 
        }
        if (ImGui::Button("Gym-Rat-Dresser")) { //jenn
            //dumbbell set and a phone ?
            printf("changed to Gym Rat lamp"); 
        }
        if (ImGui::Button("Anime-Dresser")) {//jenn
            //anime figure
            printf("changed to anime dresser"); 
        }
        if (ImGui::Button("Cat-Lady-Desk")) {//jen
            //yarn + cat snacks
        
            printf("changed to cat lady dresser"); 
        }
        if (ImGui::Button("Nerdy-Dresser")) {//jen
            //printer
            printf("changed to nerdy lamp"); 
        }
        if (ImGui::Button("Nerdy-Dresser-II")) {//jen
            //action figures
            printf("changed to nerdy lamp"); 
        }
    ImGui::EndPopup();
    }
//----------------Kitcheen-------------------
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("FRIDGE")) {
        
         ImGui::OpenPopup("Fridge-SubMenu");
    }

    
    if (ImGui::BeginPopup("Fridge-SubMenu")) {  
         if( isPurchased("fridgefood")== false){
            if (ImGui::Button("Food 1200$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1200){
                        ECS.Get<GraphicsManager::Sprite>(6).image_name = "fridgefood";  
                        printf("changed to food"); 
                        temp5 = "fridgefood";
                        ECS.Get<EntityManager::Money>(0).price-=1200; 
                        purchasedItems.push_back("fridgefood");
                        PurchasedItemSound();
                }
               
            } 
         }
        if(ImGui::IsItemHovered()){
                    // ECS.Get<GraphicsManager::Sprite>(5).image_name = "fridgefood"; 
        }
       else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(6).image_name = temp5; // Revert to temp when not hovered.
        }  
       //---------microwave--------


        if( isPurchased("fridgemicrowave")== false){
            if (ImGui::Button("Microwave 1300$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1300){
                        ECS.Get<GraphicsManager::Sprite>(6).image_name = "fridgemicrowave";  
                        printf("changed to microwave"); 
                        temp5 = "fridgemicrowave";
                        ECS.Get<EntityManager::Money>(0).price-=1300; 
                        purchasedItems.push_back("fridgemicrowave");
                        PurchasedItemSound();
                }
               
            } 
         }
        if(ImGui::IsItemHovered()){
                    // ECS.Get<GraphicsManager::Sprite>(5).image_name = "fridgemicrowave"; 
       }



        //-------------mini-crockpot-----------
        if (ImGui::Button("Mini-Crockpot")) { //jenn
            // minicrockpot plus seasonings
            printf("changed to crockpot"); 
        }
        if (ImGui::Button("Coffee-Maker")) { //jen
            //coffee maker
            printf("changed to coffeemaker"); 
        }
        if (ImGui::Button("Pizza-Box")) { //jen
            //Open pizza box
            printf("changed to sweettooth"); 
        }
        ImGui::EndPopup();
    }
   
    //------------FLOOR--------------
    //min =400, max = 500 

    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("FLOOR")) {
        
         ImGui::OpenPopup("Floor-SubMenu");
    }
    if (ImGui::BeginPopup("Floor-SubMenu")) {
 
      
        if( isPurchased("patriotfloor")== false){
            if (ImGui::Button("Patriot-Floor 400$")) { //Shadai 
                ECS.Get<GraphicsManager::Sprite>(7).image_name = "patriotfloor"; 
                //green rug 
                printf("changed to patriot floor"); 
                temp6 = "patriotfloor";
                ECS.Get<EntityManager::Money>(0).price-=400; 
                purchasedItems.push_back("patriotfloor");
                PurchasedItemSound();
            }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(7).image_name = "patriotfloor"; 
        }
         
        //---------rich snitch floor------------
        
        if( isPurchased("richfloor")== false){
            if (ImGui::Button("Rich-B*****-Floor 495$")) { //Shadai 
             /*   ECS.Get<GraphicsManager::Sprite>(7).image_name = "richfloor"; 
                //green rug 
                printf("changed to rich floor"); 
                temp6 = "richfloor";
                ECS.Get<EntityManager::Money>(0).price-=495; 
                purchasedItems.push_back("richfloor");
            }*/
            }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(7).image_name = "richfloor"; 
        }

        //-----------gaming floor---------
  
         if( isPurchased("gamefloor")== false){
            if (ImGui::Button("Gaming-Floor 480$")) { //Shadai 
               ECS.Get<GraphicsManager::Sprite>(7).image_name = "gamefloor"; 
                printf("changed to game floor"); 
                temp6 = "gamefloor";
                ECS.Get<EntityManager::Money>(0).price-=480; 
                purchasedItems.push_back("gamefloor");
                PurchasedItemSound();
            }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(7).image_name = "gamefloor"; 
        }


        //-------rainbow floor----------
        //cherry floor
        if( isPurchased("pridefloor")== false){
            if (ImGui::Button("Rainbow-Floor 480$")) { //Shadai 
          /*  
               ECS.Get<GraphicsManager::Sprite>(7).image_name = "pridefloor"; 
                printf("changed to rainbow floor"); 
                temp6 = "pridefloor";
                ECS.Get<EntityManager::Money>(0).price-=480; 
                purchasedItems.push_back("pridefloor");
           */ 
          }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(7).image_name = "pridefloor"; 
        }

        //----------goth floor------------

        if( isPurchased("gothfloor")== false){ 
            if (ImGui::Button("Goth-Floor 485$")) { //Shadai 
            /*
           
               ECS.Get<GraphicsManager::Sprite>(7).image_name = "gothfloor"; 
                printf("changed to game floor"); 
                temp6 = "gothfloor";
                ECS.Get<EntityManager::Money>(0).price-=485; 
                purchasedItems.push_back("gothfloor");
            */
           }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(7).image_name = "gothfloor"; 
        }

        //---------halloween floor--------
        //bat rug or web design 
       if( isPurchased("hallowfloor")== false){
         if (ImGui::Button("Halloween-Floor 500$")) { //Shadai 
        /*
           
               ECS.Get<GraphicsManager::Sprite>(7).image_name = "hallowfloor"; 
                printf("changed to hallow floor"); 
                temp6 = "hallowfloor";
                ECS.Get<EntityManager::Money>(0).price-=500; 
                purchasedItems.push_back("hallowfloor");
            */
           }
        }
        if(ImGui::IsItemHovered()){
            // ECS.Get<GraphicsManager::Sprite>(7).image_name = "hallowfloor"; 
        }



        //----------Christmas floor------------- 
        if (ImGui::Button("Christmas-Floor")) {//jen
            //tiny tree + presents  
        }
        if (ImGui::Button("Gym-Rat-Floor")) {
            //yoga mat with exercise ball //jen
        }
        if (ImGui::Button("Anime-Floor")) { //jen/dai
            //random manga panel
        }
        if (ImGui::Button("Cat-Lady-Floor")) { //jen
            //fluffy Rug + Cat???
        
            
        }
        if (ImGui::Button("Nerdy-Floor")) { //jen
            //comic book rug
        
        }
        ImGui::EndPopup();
    }

    
    ImGui::End(); //end dormshop window
    
    //-------------money------------------
    ImGui::Begin("Bread!"); 
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


    ImGui::Begin("Inventory!"); 

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
                        temp = purchasedItems[i];
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
                        temp2 = purchasedItems[i];
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
                        temp3 = purchasedItems[i];
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
                        temp4 = purchasedItems[i];
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
                        temp5 = purchasedItems[i];
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
                        temp6 = purchasedItems[i];
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
    temp =  ECS.Get<GraphicsManager::Sprite>(2).image_name ; // bed
    temp2 =  ECS.Get<GraphicsManager::Sprite>(3).image_name ; //lamp
    temp3 = ECS.Get<GraphicsManager::Sprite>(4).image_name; //desk
    temp4 =  ECS.Get<GraphicsManager::Sprite>(5).image_name;  //dresser
    temp5 = ECS.Get<GraphicsManager::Sprite>(6).image_name; //fridge
    temp6 = ECS.Get<GraphicsManager::Sprite>(7).image_name; //floor
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


