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
    curr_bed = "";
    curr_lamp = "";
    curr_desk = "";
    curr_dresser = "";
    curr_fridge = "";
    curr_floor = "";
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

    //set unpurchased items here
    unpurchasedItems.push_back("patriotbed");
    unpurchasedItems.push_back("patriotbed2");
    unpurchasedItems.push_back("richsnitchbed");
    unpurchasedItems.push_back("gamerbed");
    unpurchasedItems.push_back("gamerbed2");
    unpurchasedItems.push_back("pridebed");
    unpurchasedItems.push_back("gothbed");
    unpurchasedItems.push_back("gothbed2");
    unpurchasedItems.push_back("halloweenbed");
    unpurchasedItems.push_back("christmasbed");
    
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

//have item map
    //{item_name: price, button_name}

void GuiManager::DormShopSetter(std::string item_name){

    if(isPurchased(item_name)== false){ //only build buttons non-inventoried elements
        
        //build button name 
        //TODO: update pricing!!!!
        std::string button_name_str = item_name + " 1000$";
        const char* button_name = button_name_str.c_str();
        
        if (ImGui::Button(button_name)) { 

            //button was clicked
        // if (ImGui::Button("Patriot-Bed 1000$" )) {
            if( isPurchased(item_name)== false){
                        //if item not already purchased 
                if(ECS.Get<EntityManager::Money>(0).price >=1000){
                        ECS.Get<GraphicsManager::Sprite>(2).image_name = item_name; //set image
                        curr_bed = item_name;
                        printf("purchased %s\n", item_name.c_str()); 
                        ECS.Get<EntityManager::Money>(0).price-=1000; 
                        purchasedItems.push_back(item_name);
                        PurchasedItemSound();                            
                }else{
                            printf("Can't afford %s!\n", item_name.c_str());
                }
            }
        }

        // CheckHovered(item_name); //not sure if this is right...

        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = item_name;    
        }
    }
}

void GuiManager::CheckHovered(std::string item_name){
    if (ImGui::IsItemHovered()) {
        ECS.Get<GraphicsManager::Sprite>(2).image_name = item_name;    
    }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
        ECS.Get<GraphicsManager::Sprite>(2).image_name = curr_bed; // Revert to temp when not hovered.
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
        
        for (int i = 0; i < unpurchasedItems.size(); i++)
        {
            //hover is lowkey broken...
            std::string item_name = unpurchasedItems[i];
            DormShopSetter(item_name);
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

        //set to current after all buttons are createe
        //ECS.Get<GraphicsManager::Sprite>(2).image_name = curr_bed; //set to currently set bed

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
                    curr_lamp = "patriotlamp";
                    ECS.Get<EntityManager::Money>(0).price-=500; 
                    purchasedItems.push_back("patriotlamp");
                    PurchasedItemSound();
                }
            }
         }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(3).image_name = "patriotlamp"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(3).image_name = curr_lamp; // Revert to temp when not hovered.
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
                    curr_lamp = "gamerlamp";
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
                    curr_desk = "desktv";
                    ECS.Get<EntityManager::Money>(0).price-=800; 
                    purchasedItems.push_back("desktv");
                    PurchasedItemSound();
                }        
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(4).image_name = "desktv"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(4).image_name = curr_desk; // Revert to temp when not hovered.
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
                    curr_desk = "deskpc";
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
                    curr_desk = "deskconsole";
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
                        curr_dresser = "patriotdresser";
                        ECS.Get<EntityManager::Money>(0).price-=700; 
                        purchasedItems.push_back("patriotdresser");
                        PurchasedItemSound();
                } 
                    
                
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(5).image_name = "patriotdresser"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(5).image_name = curr_dresser; // Revert to temp when not hovered.
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
                    curr_dresser = "gamingdresser";
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
                        curr_fridge = "fridgefood";
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
            ECS.Get<GraphicsManager::Sprite>(6).image_name = curr_fridge; // Revert to temp when not hovered.
        }  
       //---------microwave--------


        if( isPurchased("fridgemicrowave")== false){
            if (ImGui::Button("Microwave 1300$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1300){
                        ECS.Get<GraphicsManager::Sprite>(6).image_name = "fridgemicrowave";  
                        printf("changed to microwave"); 
                        curr_fridge = "fridgemicrowave";
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
                curr_floor = "patriotfloor";
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
                curr_floor = "gamefloor";
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

    //====================INVENTORY===============================//

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


