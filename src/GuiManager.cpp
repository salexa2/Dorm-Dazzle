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

std::chrono::system_clock::time_point start_time;
std::chrono::system_clock::time_point end_time;

//include this in the graphics manager, 
GuiManager::GuiManager()
{
    temp = "";
  ///  std::cout << "help: " << ECS.Get<GraphicsManager::Sprite>(2).image_name << std::endl;

}

void GuiManager::Start(GLFWwindow *window, WGPUDevice device, WGPUTextureFormat swapchainformat)
{
    time_t lastLoad = LoadTime();
    
    //initialize time things...
    maxStamina = 40.0f;
    replenish_rate = 1.0f;
    start_time = std::chrono::system_clock::now(); //set init start time to when gui manager starts

    time_t now = std::chrono::system_clock::to_time_t(start_time);

    double diff = difftime(now, lastLoad);

    std::cout << "Time between last shut down and now = " << diff << std::endl;

    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); 
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOther(window, true);
    WGPUTextureFormat ob = WGPUTextureFormat_Undefined; 
    ImGui_ImplWGPU_Init(device,3,swapchainformat, ob);    
}

time_t GuiManager::LoadTime() {
    //read time saved to file into 

    std::ifstream timeFile("time.txt");
    if (timeFile.is_open()) {
        std::string timeString;
        std::getline(timeFile, timeString);
        timeFile.close();
        //std::cout << "The time at last shut down was " << timeString << std::endl;

        // convert string to time_t
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
    ImGui::ProgressBar(ECS.Get<EntityManager::Health>(0).percent / maxStamina, ImVec2(-1, 0), "Max: 40");

    //calculate current time
    end_time = std::chrono::system_clock::now();

    //if time is >= 2 minutes -> update energy and reset start time
    auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    //replenish over time
    //if (time_elapsed.count() >= 120) { //2 minutes
    if (time_elapsed.count() >= 30) { //30 seconds
        ECS.Get<EntityManager::Health>(0).percent += replenish_rate;
        //currentStamina = currentStamina + replenish_rate; //update energy
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
             ECS.Get<GraphicsManager::Sprite>(2).image_name = temp; // Revert to temp when not hovered.
        }
        //-------------------patriotbed-----------------
        if (ImGui::Button("Patriot-Bed  1000$")) {
            if(ECS.Get<EntityManager::Money>(0).price >=1000){
                 ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";
                 temp = "patriotbed";
                 printf("purchased patriot bed\n"); 
                 ECS.Get<EntityManager::Money>(0).price-=1000; 

            }else{
                printf("Can't afford patriot bed!\n");
            }
           
        
        } if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed";    
        }
        //-------------------Patriotbed2------------------------
        if (ImGui::Button("Patriot-Bed-II 1050$")) {
             if(ECS.Get<EntityManager::Money>(0).price >=1050){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";   
                printf("changed to patriot bed2");
                temp = "patriotbed2";
                ECS.Get<EntityManager::Money>(0).price-=1050; 
             }else{
                printf("Can't afford patriotbed2");
             }

        }if (ImGui::IsItemHovered()) {
            ECS.Get<GraphicsManager::Sprite>(2).image_name = "patriotbed2";    
        }
        //--------------------richsnitch---------------------
        if (ImGui::Button("Rich-Snitch 1100$")) {
            if(ECS.Get<EntityManager::Money>(0).price >=1100){
                ECS.Get<GraphicsManager::Sprite>(2).image_name = "richsnitchbed";   
                printf("changed to rich snitch");
                temp = "richsnitchbed";
                ECS.Get<EntityManager::Money>(0).price-=1100; 
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
        if (ImGui::Button("Boring-Lamp")) {
            ECS.Get<GraphicsManager::Sprite>(3).image_name = "boringlamp";  
            printf("changed to default lamp"); 
        } 
        if (ImGui::Button("Patriot-Lamp")) { //shadai 
            ECS.Get<GraphicsManager::Sprite>(3).image_name = "patriotlamp";  
            printf("changed to patriot lamp"); 
        }


        if (ImGui::Button("Rich-Snitch-Lamp")) {//shadai 
          
            printf("changed to rich snitch lamp"); 
        }
        if (ImGui::Button("Gaming-Lamp")) {//shadai 
            ECS.Get<GraphicsManager::Sprite>(3).image_name = "gamerlamp";  
            printf("changed to gaming lamp"); 
        }
        if (ImGui::Button("Rainbow-Lamp")) { //sahdai 
        
            printf("changed to rainbow lamp"); 
        }
         if (ImGui::Button("Goth-Lamp")) {//shadai 
            //candles
            printf("changed to gothlamp"); 
        }
         if (ImGui::Button("Halloween-Lamp")) { //shadai 
            //jackolantern
            printf("changed to Halloween lamp"); 
        }
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
    if(ImGui::Button("DESKS")) {
        
         ImGui::SetNextWindowSize(ImVec2(400, 100));
         ImGui::OpenPopup("Desk-SubMenu");
    }

    
    if (ImGui::BeginPopup("Desk-SubMenu")) {
        if (ImGui::Button("Boring-Desk")) {
            ECS.Get<GraphicsManager::Sprite>(4).image_name = "boringdesk";  
            printf("changed to default desk"); 
        } 
        if (ImGui::Button("Patriot-Desk")) { // shadai 
            printf("changed to patriot Desk"); 
            ECS.Get<GraphicsManager::Sprite>(4).image_name = "desktv";  
        }


        if (ImGui::Button("Rich-Snitch-Desk")) {//shadai 
          
            printf("changed to rich Desk"); 
        }
        if (ImGui::Button("Gaming-Desk-I")) {//shadai 
            //PC
            ECS.Get<GraphicsManager::Sprite>(4).image_name = "pcdesk"; 
            printf("changed to gaming Desk"); 
        }
        if (ImGui::Button("Gaming-Desk-II")) { //shadai 
            //Console
            ECS.Get<GraphicsManager::Sprite>(4).image_name = "deskconsole"; 
            printf("changed to gaming Desk"); 
        }
        if (ImGui::Button("Rainbow-Desk")) { //shadai 
            //laptop with rainbow mouse mat and a cup of coffee
            printf("changed to rainbow Desk"); 
        }
         if (ImGui::Button("Goth-Desk")) { // shadai 
            //maybe - ritual - skull withe pencils paper, a thick book 
            printf("changed to gothdesk"); 
        }
         if (ImGui::Button("Halloween-Desk")) { //shadai 
            //potion brew
            printf("changed to Halloween desk"); 
        }
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
            //patriot merch
             ECS.Get<GraphicsManager::Sprite>(5).image_name = "patriotdresser";
            printf("changed to patriot dresser"); 
        }
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
            ECS.Get<GraphicsManager::Sprite>(6).image_name = "towel";  
            printf("changed to papertowel"); 
        } 
        if (ImGui::Button("Food")) {
            //ramen cups
            ECS.Get<GraphicsManager::Sprite>(6).image_name = "food";  
            printf("changed to food"); 
        } 
        if (ImGui::Button("Microwave")) {
             ECS.Get<GraphicsManager::Sprite>(6).image_name = "microwave";  
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
            currentStamina -= 5;
        }
        //5 energy
        //100 crumbs

     }
     if (ImGui::Button("Work-Study - 10 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=10){
            ECS.Get<EntityManager::Money>(0).price+=400;
            ECS.Get<EntityManager::Health>(0).percent-=10;
            currentStamina -= 10;
        }
        //10 energy
        //400 crumbs
     }
     if (ImGui::Button("Part-Time-Job - 15 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=15){
            ECS.Get<EntityManager::Money>(0).price+=700;
            ECS.Get<EntityManager::Health>(0).percent-=15;
            currentStamina -= 15;
        }
        //15 energy
        //700 crumbs
     }
     if (ImGui::Button("Internship - 35 Energy")) {
        if(ECS.Get<EntityManager::Health>(0).percent>=35){
            ECS.Get<EntityManager::Money>(0).price+=1200;
            ECS.Get<EntityManager::Health>(0).percent-=35;
            currentStamina -= 35;
        }
        //35 energy
        //1200 crumbs
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
