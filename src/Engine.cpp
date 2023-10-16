
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

        void Engine::ExitGame()
        {
            glfwSetWindowShouldClose(GLOBAL_ENGINE.graphicsManager.window, true); 
        }

        //----------HELP HELP HELP 
        void Engine::RunGameLoop(const UpdateCallBack& callback){
          
       
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
