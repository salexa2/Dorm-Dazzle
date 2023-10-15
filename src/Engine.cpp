
#include "Engine.h"
#include <thread> 
 #include <chrono>
 

        
    //Constructor
        Engine::Engine(){
    
        }
    
    //Other managers 
        

    //Methods
        void Engine::Start(){
            
          graphicsManager.Start();
          scriptManager.Start(); 
           
        }
           
        
        void Engine::Shutdown(){
            
            graphicsManager.Shutdown();
            //inputManager.Shutdown();
        }
        
        //----------HELP HELP HELP 
        void Engine::RunGameLoop(const UpdateCallBack& callback){
          
          /*
          graphicsManager.LoadTexture("imageName","assets/fox.png");
          GraphicsManager::Sprite fox; 
          fox.image_name = "imageName";
          fox.position = {0,0,0};
          fox.scale = {100,100};
          fox.z = 0;
          //fox.d = graphicsManager.image_map.at("imageName");
          std::vector<GraphicsManager::Sprite> spriteVector;
          spriteVector.push_back(fox);
        */  
          while(true){
             inputManager.Update();
             UpdateCallBack();
            // 
            callback(*this);
            if(glfwWindowShouldClose(graphicsManager.window)== true){
              break;
            }
            graphicsManager.Draw();


             
            
            const auto t1 = std::chrono::steady_clock::now();
            const auto t2 = std::chrono::steady_clock::now();
            const auto sub = t1-t2;
            const auto sixty = std::chrono::duration<double>( sub );
            std::this_thread::sleep_for(sixty);
          }
        }

        //Engine GLOBAL_ENGINE;
        
                // Manage timestep
