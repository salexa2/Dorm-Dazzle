#include <iostream>
#include "Engine.h"

//xmakEngine engine;
    
 
void printMessage(Engine& engine){

     if(GLOBAL_ENGINE.inputManager.KeyisPressed(GLFW_KEY_Q)== true || GLOBAL_ENGINE.inputManager.KeyisPressed(GLFW_KEY_ESCAPE)== true){
        glfwSetWindowShouldClose(GLOBAL_ENGINE.graphicsManager.window, true); 
        printf("Escape Key Clicked!\n");
     }
    
   
}
int main( int argc, const char* argv[] ) {
    std::cout << "Hello, World!\n";

     GLOBAL_ENGINE.scriptManager.LoadScript("startup", "assets/scripts/startup.lua");

    GLOBAL_ENGINE.Start();
    // GLOBAL_ENGINE.graphicsManager.guiManager.Start(GLOBAL_ENGINE.graphicsManager.window,GLOBAL_ENGINE.graphicsManager.device,GLOBAL_ENGINE.graphicsManager.swap_chain_format);
   
    GLOBAL_ENGINE.scriptManager.scriptResults["startup"]();
    GLOBAL_ENGINE.graphicsManager.guiManager.Start(GLOBAL_ENGINE.graphicsManager.window,GLOBAL_ENGINE.graphicsManager.device,GLOBAL_ENGINE.graphicsManager.swap_chain_format);


  


    GLOBAL_ENGINE.graphicsManager.LoadSprite("sprites.json");
   

    GLOBAL_ENGINE.graphicsManager.guiManager.SetTemp(); // sets the default items temp variables 
    GLOBAL_ENGINE.graphicsManager.guiManager.loadPurchasedItems("inventory.json");
    GLOBAL_ENGINE.graphicsManager.guiManager.loadMoney("money.json");
    GLOBAL_ENGINE.RunGameLoop( [&]( Engine& ) {

       

        GLOBAL_ENGINE.scriptManager.LoadScript("checkpress", "assets/scripts/checkpress.lua");
        GLOBAL_ENGINE.scriptManager.scriptResults["checkpress"]();
     
       
      //   printMessage(GLOBAL_ENGINE);
        
         GLOBAL_ENGINE.scriptManager.UpdateEngine();

       // GLOBAL_ENGINE.scriptManager.LoadScript("fox", "assets/scripts/fox.lua");
      //  GLOBAL_ENGINE.scriptManager.scriptResults["fox"](); 
       
         
    } );
    
    GLOBAL_ENGINE.Shutdown(); 
    
    return 0;
}

