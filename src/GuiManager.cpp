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

    LoadAllItems();
    
    //default items in inventory
    purchasedItems.push_back("boringbed");
    purchasedItems.push_back("boringlamp");
    purchasedItems.push_back("boringdesk");
    purchasedItems.push_back("boringdresser");
    purchasedItems.push_back("fridgetowel"); 
    purchasedItems.push_back("boringfloor");
    purchasedItems.push_back("fridgetowel");
}

void GuiManager::LoadAllItems(){
    //make array of structs

    struct Item items[] = {
        {"boringdesk", 0, true},
        {"boringdresser", 0, true},
        {"boringfloor", 0, true},
        {"boringlamp", 0, true},
        {"boringtowel", 0, true}, //end of borring...
        //beds
        {"patriotbed", 1000, false},
        {"patriotbed2", 1050, false},
        {"richsnitchbed", 1800, false},
        {"gamerbed", 1400, false},
        {"gamerbed2", 0, false},
        {"pridebed", 0, false},
        {"gothbed", 0, false},
        {"gothbed2", 0, false},
        {"christmasbed", 0, false},
        {"halloweenbed", 0, false},
        //lamps
        {"patriotlamp", 0, false},
        {"gamerlamp", 0, false},
        //dressers
        {"patriotdresser", 0, false},
        {"gamingdresser", 0, false},
        {"patriotfloor", 0, false},
        {"gamefloor", 0, false},
        {"coffee", 0, false},
        {"deskconsole", 0, false},
        {"deskpc", 0, false},
        {"desktv", 0, false},
        //fridge
        {"fridgefood", 0, false},
        {"fridgeramen", 0, false},
        {"fridgemicrowave", 0, false}
    };

    for(int i = 0; i < sizeof(items) / sizeof(items[0]); i++){
        allItems.push_back(items[i]);
    }
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

void GuiManager::DormShopSetter(Item item, int item_type){

    std::string item_name = item.item_name;
    std::string item_price = std::to_string(item.price);

    if(isPurchased(item_name)== false){ //only build buttons non-inventoried elements
        
        //build button name 
        //TODO: update pricing!!!!
        std::string button_name_str = item.item_name + + " " + item_price + "$";
        const char* button_name = button_name_str.c_str();
        
        if (ImGui::Button(button_name)) { 

            //button was clicked
      
            // if( isPurchased(item_name)== false){
            if(item.purchaced == false){
                        //if item not already purchased 
                if(ECS.Get<EntityManager::Money>(0).price >=1000){
                        // ECS.Get<GraphicsManager::Sprite>(2).image_name = item.item_name; //set image
                        ECS.Get<GraphicsManager::Sprite>(item_type).image_name = item.item_name; //set image
                        curr_bed = item_name;
                        printf("purchased %s\n", item.item_name.c_str()); 
                        ECS.Get<EntityManager::Money>(0).price -= item.price; 
                        purchasedItems.push_back(item.item_name);
                        item.purchaced = true; //set purchaced to true
                        PurchasedItemSound();                            
                }else{
                    printf("Can't afford %s!\n", item.item_name.c_str());
                }
            }
        }
        if (ImGui::IsItemHovered()) {
            // ECS.Get<GraphicsManager::Sprite>(2).image_name = item.item_name;    
            ECS.Get<GraphicsManager::Sprite>(item_type).image_name = item.item_name;    
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
         if( isPurchased("stonerbed")== false){
            if (ImGui::Button("Stoner-Bed 1500$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1500){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "stonerbed";   
                    printf("changed to stoner bed");
                    temp =  "stonerbed";
                    ECS.Get<EntityManager::Money>(0).price-=1500; 
                    purchasedItems.push_back("pridebed");
                }
            // printf("temp:  ",temp);
            }
         }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "stonerbed";    
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
        if( isPurchased("christmasbed")== false){
            if (ImGui::Button("Christmas-Bed 1900$")) {
                if(ECS.Get<EntityManager::Money>(0).price >=1900){
                    ECS.Get<GraphicsManager::Sprite>(2).image_name = "christmasbed";  
                    printf("changed to Christmas bed"); 
                    temp = "christmasbed";
                    ECS.Get<EntityManager::Money>(0).price-=1900; 
                    purchasedItems.push_back("christmasbed");
                    PurchasedItemSound();
                }
            }
        }
        if(ImGui::IsItemHovered()) {// allows user to preview the decor before the purchase it! 
           // printf("hovered hallowe");
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "christmasbed";     
             
        }
        // if (ImGui::Button("Christmas-Bed 1900$")) {
        
        for (int i = 0; i < allItems.size(); i++){
            Item item = allItems[i];
            if(item.item_name.find("bed") != std::string::npos){
                DormShopSetter(item, 2);
            }
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

        for (int i = 0; i < allItems.size(); i++){
            Item item = allItems[i];
            if(item.item_name.find("lamp") != std::string::npos){
                DormShopSetter(item, 3);
            }
        }

       //-------------richlamp-----------------
        if( isPurchased("richlamp")== false){
            if (ImGui::Button("Rich-B**** Lamp 590$")) {//shadai 
                if(ECS.Get<EntityManager::Money>(0).price >=590){
                        printf("changed to rich snitch lamp"); 
                        ECS.Get<GraphicsManager::Sprite>(3).image_name = "richlamp";  
                        temp2 = "richlamp";
                        ECS.Get<EntityManager::Money>(0).price-=590; 
                        purchasedItems.push_back("richlamp");
                        
                }      
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(3).image_name = "richlamp"; 
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
         if( isPurchased("stonerlamp")== false){
            if (ImGui::Button("Stoner-Lamp 560$")) { //sahdai 
            
                /*if(ECS.Get<EntityManager::Money>(0).price >=560){
                    printf("changed to stonerlamp"); 
                    ECS.Get<GraphicsManager::Sprite>(3).image_name = "stonerlamp";  
                    temp2 = "stonerlamp";
                    ECS.Get<EntityManager::Money>(0).price-=560; 
                    purchasedItems.push_back("pridelamp");
                } 
                */
                
            }
         }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "stonerlamp"; 
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
        
        for (int i = 0; i < allItems.size(); i++){
            Item item = allItems[i];
            if(item.item_name.find("desk") != std::string::npos){
                DormShopSetter(item, 4);
            }
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

         for (int i = 0; i < allItems.size(); i++){
            Item item = allItems[i];
            if(item.item_name.find("dresser") != std::string::npos){
                DormShopSetter(item, 5);
            }
        }

    ImGui::EndPopup();
    }
//----------------Kitcheen-------------------
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("FRIDGE")) {
        
         ImGui::OpenPopup("Fridge-SubMenu");
    }

    
    if (ImGui::BeginPopup("Fridge-SubMenu")) {  

        for (int i = 0; i < allItems.size(); i++){
            Item item = allItems[i];
            if(item.item_name.find("fridge") != std::string::npos){
                DormShopSetter(item, 6);
            }
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
        
        
        for (int i = 0; i < allItems.size(); i++){
            Item item = allItems[i];
            if(item.item_name.find("floor") != std::string::npos){
                DormShopSetter(item, 7);
            }
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


