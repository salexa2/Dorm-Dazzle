#include "GuiManager.h"
#include <iostream>
#include "GuiManager.h"
#include <imgui.h>

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include "GraphicsManager.cpp"
#include <chrono>
#include <thread>
#include <ctime>
#include <nlohmann/json.hpp>

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

    ImGui_ImplWGPU_Init(device,3,swapchainformat, ob);
    //default items in inventory
    purchasedItems.push_back("boringbed");
    purchasedItems.push_back("boringlamp");
    purchasedItems.push_back("boringdesk");
    purchasedItems.push_back("boringdresser");
    purchasedItems.push_back("fridgetowel"); 
    purchasedItems.push_back("boringfloor");

}

time_t GuiManager::LoadTime() {
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

void GuiManager::InitEnergyBar(){
    
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
        while (time_away >= 30 || ECS.Get<EntityManager::Health>(0).percent < maxStamina)
        {
            time_away -= 30;
            ECS.Get<EntityManager::Health>(0).percent += 1;
        }
    }
}

void GuiManager::Shutdown()
{

    SaveEnergy(); //save energy at shutdown
   // ImGui_ImplOpenGL3_Shutdown();

   
    savePurchasedItems(saveFilePathI, purchasedItems);
    saveMoney(saveFilePathB);

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::SaveEnergy() {
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

void GuiManager::SaveTime() //called by engine for more exact shutdown time
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
   //-----------JENNIFER ENERGY GRAPHICS IS HERE-----------
   //every 2 minutes add an energy?
   //add energy when player is away, so get the time before they quit, save it, then get the time when they get on, get the difference and increase energy
   //if the sum of the difference and the current energy exceeds 40, just set the energy to 40. 
 
    ImGui::Begin("Energy");
    ImGui::SetNextWindowSize(ImVec2(200, 30));
    ImGui::SetWindowPos(ImVec2(500, 0));


    //udpate progress bar
    ImGui::ProgressBar(ECS.Get<EntityManager::Health>(0).percent / maxStamina, ImVec2(-1, 0), "");

    //calculate current time
    end_time = std::chrono::system_clock::now();

    //if time is >= 2 minutes -> update energy and reset start time
    auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    //replenish over time
    //if (time_elapsed.count() >= 120) { //2 minutes
    if (time_elapsed.count() >= 30) { //30 seconds -> update every 30 seconds
        ECS.Get<EntityManager::Health>(0).percent += replenish_rate;
        start_time = std::chrono::system_clock::now(); //update to start counter over
    }

    //clamp to maxStamina
    if (ECS.Get<EntityManager::Health>(0).percent > maxStamina) {
        ECS.Get<EntityManager::Health>(0).percent = maxStamina;
    }

    ImGui::End();
   //-----------------------------------------------------------------

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
        if (ImGui::Button("Patriot-Bed-II 1050$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1050){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";   
                printf("changed to patriot bed2");
                temp = "patriotbed2";
                ECS.Get<EntityManager::Money>(0).price-=1050; 
             }
             else{
                printf("Can't afford patriotbed2");
             }

        }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";    
        }
        //--------------------richsnitch---------------------
        if (ImGui::Button("Rich-B**** 1800$")) {
            if(ECS.Get<EntityManager::Money>(0).price >=1800){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";   
                printf("changed to rich snitch");
                temp = "richsnitchbed";
                ECS.Get<EntityManager::Money>(0).price-=1800; 
            }
        }if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";    
        }
        //--------------------------Gamerbed-----------------------
         if (ImGui::Button("Gamer-Bed 1400$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1150){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed";   
                printf("changed to gamer bed");
                temp = "gamerbed";
                ECS.Get<EntityManager::Money>(0).price-=1150; 
             }
        }if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed";    
        }
        //----------------------Gamerbed2------------------------
         if (ImGui::Button("Gamer-Bed-II 1150$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1250){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed2";   
                printf("changed to gamer bed2");
                temp = "gamerbed2";
                ECS.Get<EntityManager::Money>(0).price-=1250; 
             }

         }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gamerbed2";    
        }
        //-----------------rainbowbed------------------------------
        if (ImGui::Button("Rainbow-Bed 1500$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1500){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "pridebed";   
                printf("changed to pride bed");
                temp =  "pridebed";
                ECS.Get<EntityManager::Money>(0).price-=1500; 
             }
           // printf("temp:  ",temp);
        }
        if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "pridebed";    
        }
        //---------------Gothbed-----------------------------
        if (ImGui::Button("Goth-Bed 1550$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1300){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed";   
                printf("changed to goth bed");
                temp = "gothbed";
                ECS.Get<EntityManager::Money>(0).price-=1300; 
             }
        }
        if(ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed";    
        }
        //-----------------Gothbed2---------------------------
         if (ImGui::Button("Goth-Bed-II 1300$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1500){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed2";   
                printf("changed to goth bed2");
                temp = "gothbed2";
                ECS.Get<EntityManager::Money>(0).price-=1500; 
             }
        }
        if (ImGui::IsItemHovered()) {
            //printf("hovered");
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "gothbed2";    
        }
      
        //----------------------Halloween-------------------
        if (ImGui::Button("Halloween-Bed 1900$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1900){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "halloweenbed";  
                printf("changed to Halloween bed"); 
                temp = "halloweenbed";
                ECS.Get<EntityManager::Money>(0).price-=1900; 
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
        if (ImGui::Button("Patriot-Lamp 500$")) { //shadai 
            if(ECS.Get<EntityManager::Money>(0).price >=500){
                ECS.Get<GraphicsManager::Sprite>(3).image_name = "patriotlamp";  
                printf("changed to patriot lamp"); 
                temp2 = "patriotlamp";
                ECS.Get<EntityManager::Money>(0).price-=500; 
            }
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(3).image_name = "patriotlamp"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(3).image_name = temp2; // Revert to temp when not hovered.
        }

       //-------------richlamp-----------------
        if (ImGui::Button("Rich-B**** Lamp 590$")) {//shadai 
           if(ECS.Get<EntityManager::Money>(0).price >=590){
                /*
                printf("changed to rich snitch lamp"); 
                ECS.Get<GraphicsManager::Sprite>(3).image_name = "richlamp";  
                temp2 = "richlamp";
                ECS.Get<EntityManager::Money>(0).price-=590; 
                */
           }      
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "richlamp"; 
        }

        //-------------Gamer lamp--------------------
        if (ImGui::Button("Gaming-Lamp 550$")) {//shadai 
         if(ECS.Get<EntityManager::Money>(0).price >=550){
            printf("changed to gaming lamp"); 
            ECS.Get<GraphicsManager::Sprite>(3).image_name = "gamerlamp";  
            temp2 = "gamerlamp";
            ECS.Get<EntityManager::Money>(0).price-=550; 
         }

        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(3).image_name = "gamerlamp"; 
        }

        //------------------Rainbow lamp------------------
        if (ImGui::Button("Rainbow-Lamp 560$")) { //sahdai 
        
            //rainbow cloud lamp 
            /*if(ECS.Get<EntityManager::Money>(0).price >=560){
                printf("changed to rainbow lamp"); 
                ECS.Get<GraphicsManager::Sprite>(3).image_name = "pridelamp";  
                temp2 = "pridelamp";
                ECS.Get<EntityManager::Money>(0).price-=560; 
             } 
             */
              
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "pridelamp"; 
        }
          
        //------------Goth Lamp------------
         if (ImGui::Button("Goth-Lamp 575$")) {//shadai 
            //candles
            /*if(ECS.Get<EntityManager::Money>(0).price >=565){
                printf("changed to goth lamp"); 
                ECS.Get<GraphicsManager::Sprite>(3).image_name = "gothlamp";  
                temp2 = "gothlamp";
                ECS.Get<EntityManager::Money>(0).price-=565; 
             } 
             */
              
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(3).image_name = "gothlamp"; 
        }


        //--------------Halloween Lamp --------------
         if (ImGui::Button("Halloween-Lamp 600$")) { //shadai 
            //jackolantern
            /*if(ECS.Get<EntityManager::Money>(0).price >=600){
                printf("changed to halloween lamp"); 
                ECS.Get<GraphicsManager::Sprite>(3).image_name = "hallowlamp";  
                temp2 = "hallowlamp";
                ECS.Get<EntityManager::Money>(0).price-=600; 
             } 
             */
              
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

        if (ImGui::Button("Patriot-Desk 800$")) { // shadai 
            if(ECS.Get<EntityManager::Money>(0).price >=800){
                printf("changed to patriot Desk"); 
                ECS.Get<GraphicsManager::Sprite>(4).image_name = "desktv"; 
                temp3 = "desktv";
                ECS.Get<EntityManager::Money>(0).price-=800; 
            }        
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(4).image_name = "desktv"; 
        }else{ //DO NOT PUT THIS else MULTIPLE TIMES, JUST THIS RIGHT HERE IS ENOUGH
            ECS.Get<GraphicsManager::Sprite>(4).image_name = temp3; // Revert to temp when not hovered.
        }     
        //--------------richdesk-------
        if (ImGui::Button("Rich-B*****-Desk 850$")) {//shadai 
            /*
            if(ECS.Get<EntityManager::Money>(0).price >=850){
                printf("changed to gaming desk 2"); 
                ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk";  
                temp3 = "richdesk";
                ECS.Get<EntityManager::Money>(0).price-=850; 
             } 
             */
              
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk"; 
        }
        
        //--------Gaming desk 1 ------------
        if (ImGui::Button("Gaming-Desk-I 950$")) {//shadai 
            if(ECS.Get<EntityManager::Money>(0).price >=950){
                printf("changed to gaming Desk 1"); 
                ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskpc"; 
                temp3 = "deskpc";
                ECS.Get<EntityManager::Money>(0).price-=950; 
             }        
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskpc";
        }

        //-----------Gaming desk 2------------
        if (ImGui::Button("Gaming-Desk-II 875$")) { //shadai 
            //Console
            if(ECS.Get<EntityManager::Money>(0).price >=875){
                printf("changed to gaming desk2"); 
                ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskconsole"; 
                temp3 = "deskconsole";
                ECS.Get<EntityManager::Money>(0).price-=875; 
            }        
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskconsole"; 
        }
        
        //------------Rainbow desk----------------------

        if (ImGui::Button("Rainbow-Desk 860$")) { //shadai 
             /*
            if(ECS.Get<EntityManager::Money>(0).price >=850){
                printf("changed to rainbow desk"); 
                ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk";  
                temp3 = "richdesk";
                ECS.Get<EntityManager::Money>(0).price-=850; 
             } 
             */
              
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(4).image_name = "richdesk"; 
        }
        //------------goth desk---------------
         if (ImGui::Button("Goth-Desk 870$")) { // shadai 
            //maybe - ritual - skull withe pencils paper, a thick book 
            /*
            if(ECS.Get<EntityManager::Money>(0).price >=870){
                printf("changed to goth desk"); 
                ECS.Get<GraphicsManager::Sprite>(4).image_name = "gothdesk";  
                temp3 = "gothdesk";
                ECS.Get<EntityManager::Money>(0).price-=870; 
             } 
             */
              
        }
        if(ImGui::IsItemHovered()){
             //ECS.Get<GraphicsManager::Sprite>(4).image_name = "gothdesk"; 
        }
        //----------Halloween desk--------------
         if (ImGui::Button("Halloween-Desk")) { //shadai 
            //potion brew
           /*
            if(ECS.Get<EntityManager::Money>(0).price >=1000){
                printf("changed to halloween desk"); 
                ECS.Get<GraphicsManager::Sprite>(4).image_name = "hallowdesk";  
                temp3 = "hallowdesk";
                ECS.Get<EntityManager::Money>(0).price-=1000; 
             } 
             */
              
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
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("DRESSER")) {
        
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Dresser-SubMenu");
    }

    
    if (ImGui::BeginPopup("Dresser-SubMenu")) {
        if (ImGui::Button("Boring-Dresser")) {
            //books stacked
            ECS.Get<GraphicsManager::Sprite>(5).image_name = "boringdresser";  
            printf("changed to default dresser"); 
        } 

        if (ImGui::Button("Patriot-Dresser")) { // shadai 
          
            
            if(ECS.Get<EntityManager::Money>(0).price >=1000){
                printf("changed to patriot dresser"); 
                ECS.Get<GraphicsManager::Sprite>(5).image_name = "patriotdresser";
                temp4 = "patriotdesk";
                ECS.Get<EntityManager::Money>(0).price-=1000; 
            } 
             
              
        }
        if(ImGui::IsItemHovered()){
             ECS.Get<GraphicsManager::Sprite>(5).image_name = "patriotdresser"; 
        }
        //--------rich snitch--------
        if (ImGui::Button("Rich-Snitch-Dresser")) {//shadai 
            // jewlery 
            printf("changed to rich Desk"); 
        }
        if (ImGui::Button("Gaming-Dresser-I")) {//shadai 
            ECS.Get<GraphicsManager::Sprite>(5).image_name = "gamingdresser";
            printf("changed to gaming Desk"); 
        }
        if (ImGui::Button("Rainbow-Dresser")) {//shadai 
            //pride flag with some flasks
            printf("changed to rainbow dresser"); 
        }
         if (ImGui::Button("Goth-Dresser")) { //shadai 
            //maybe - ritual - skull withe pencils paper, a thick book 
            printf("changed to gothlamp"); 
        }
         if (ImGui::Button("Halloween-Dresser")) { //shadai 
            //potion brew
            printf("changed to Halloween lamp"); 
        }
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
            ECS.Get<GraphicsManager::Sprite>(5).image_name = "animedresser";
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
            //action figure
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
        if (ImGui::Button("Paper-Towel")) {
            ECS.Get<GraphicsManager::Sprite>(6).image_name = "fridgetowel";  
            printf("changed to papertowel"); 
        } 
        if (ImGui::Button("Food")) {
            //ramen cups
            ECS.Get<GraphicsManager::Sprite>(6).image_name = "fridgefood";  
            printf("changed to food"); 
        } 
        if (ImGui::Button("Microwave")) {
             ECS.Get<GraphicsManager::Sprite>(6).image_name = "fridgemicrowave";  
            printf("changed to microwave"); 
        }
        if (ImGui::Button("Mini-Crockpot")) { //jenn
            // minicrockpot plus seasonings
            printf("changed to crockpot"); 
        }
        if (ImGui::Button("Coffee-Maker")) { //jen
            //coffee maker
            printf("changed to coffeemaker"); 
        }
        if (ImGui::Button("Sweet-Tooth")) { //jen
            //basket of snacks
            printf("changed to sweettooth"); 
        }
        ImGui::EndPopup();
    }
    //------------FLOOR--------------
    if (ImGui::BeginMenuBar()){ImGui::EndMenuBar();}
    if(ImGui::Button("FLOOR")) {
        
         ImGui::OpenPopup("Floor-SubMenu");
    }
    if (ImGui::BeginPopup("Floor-SubMenu")) {
        if (ImGui::Button("Boring-Floor")) {
            ECS.Get<GraphicsManager::Sprite>(7).image_name = "boringfloor";  
            //open moving boxes
        } 
        if (ImGui::Button("Patriot-Floor")) { //Shadai 
            ECS.Get<GraphicsManager::Sprite>(7).image_name = "patriotfloor"; 
            //green rug 
        }
        if (ImGui::Button("Rich-Snitch-Floor")) { //Shadai 
            // Fluffy white rug 
        }
        if (ImGui::Button("Gaming-Floor")) {//Shadai 
            //Bean bag + circle rug
             ECS.Get<GraphicsManager::Sprite>(7).image_name = "gamefloor";  
        }
        if (ImGui::Button("Rainbow-Floor")) {//Shadai 
            //cloud rug
        }
         if (ImGui::Button("Goth-Floor")) {//Shadai 
            //ouija rug
        }
         if (ImGui::Button("Halloween-Floor")) {//Shadai 
            //bat rug or web rug design 
        }
        if (ImGui::Button("Christmas-Floor")) {
            //tiny tree + presents  
        }
        if (ImGui::Button("Gym-Rat-Floor")) {
            //yoga mat with exercise ball
        }
        if (ImGui::Button("Anime-Floor")) {
            //akatsuki rug
        }
        if (ImGui::Button("Cat-Lady-Floor")) {
            //fluffy Rug + Cat???
        
            
        }
        if (ImGui::Button("Nerdy-Floor")) {
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
            //currentStamina -= 5;
        }
        //5 energy
        //100 crumbs

     }
     if (ImGui::Button("Work-Study - 10 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=10){
            ECS.Get<EntityManager::Money>(0).price+=400;
            ECS.Get<EntityManager::Health>(0).percent-=10;
            //currentStamina -= 10;
        }
        //10 energy
        //400 crumbs
     }
     if (ImGui::Button("Part-Time-Job - 15 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=15){
            ECS.Get<EntityManager::Money>(0).price+=700;
            ECS.Get<EntityManager::Health>(0).percent-=15;
            //currentStamina -= 15;
        }
        //15 energy
        //700 crumbs
     }
     if (ImGui::Button("Internship - 35 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=35){
            ECS.Get<EntityManager::Money>(0).price+=1200;
            ECS.Get<EntityManager::Health>(0).percent-=35;
            //currentStamina -= 35;
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
                    }
                }
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("DESK")) {      
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
    std::cout << "checking: " <<  ECS.Get<GraphicsManager::Sprite>(2).image_name << std::endl;
    temp =  ECS.Get<GraphicsManager::Sprite>(2).image_name ; // bed
    temp2 =  ECS.Get<GraphicsManager::Sprite>(3).image_name ; //lamp
    temp3 = ECS.Get<GraphicsManager::Sprite>(4).image_name; //desk
    temp4 =  ECS.Get<GraphicsManager::Sprite>(5).image_name;  //dresser
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
