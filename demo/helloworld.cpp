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
   
       // Create a Lua state
    sol::state lua;



    // Expose the KeyIsDown function to Lua

    lua.set_function( "KeyisPressed", [&]( const int keycode ) { return GLOBAL_ENGINE.inputManager.KeyisPressed( keycode ); } );
    lua.set_function("KeyisReleased", &InputManager::KeyisReleased, &GLOBAL_ENGINE.inputManager);
    lua.set_function("CreateEntity", &EntityManager::CreateEntity, &ECS);
  
    //lua.set_function("GetSprite",&GraphicsManager:: GetSprite, &GLOBAL_ENGINE.graphicsManager);
   // lua.set_function("Health",&EntityManager:: GetHealth, &ECS);




    // Expose keycodes to Lua using lua.new_enum
    lua.new_enum("KEYBOARD",
                 "R", GLFW_KEY_R,
                 "SPACE", GLFW_KEY_SPACE
                 // Add more keycodes here
    );


    lua.new_usertype<GraphicsManager::Sprite>(
    "Sprite",
    sol::constructors<GraphicsManager::Sprite()>(), 
    "image_name", &GraphicsManager::Sprite::image_name,
    "position", &GraphicsManager::Sprite::position,
    "scale", &GraphicsManager::Sprite::scale,
    "z",&GraphicsManager::Sprite::z
    );

    lua.new_usertype<EntityManager::Health>(
    "Health",
    sol::constructors<EntityManager::Health()>(), 
    "percent", &EntityManager::Health::percent
    );

  
  
   // Engine engine;
    GLOBAL_ENGINE.Start();

//SET IMAGE
 //  lua.set_function("LoadTexture", [&](const std::string name, const std::string path) {return GLOBAL_ENGINE.graphicsManager.LoadTexture(name,path);});
    
    lua.set_function("LoadTexture", [&](const std::string& name, const std::string& path) {
    return GLOBAL_ENGINE.graphicsManager.LoadTexture(name, path);
});
     GLOBAL_ENGINE.scriptManager.LoadScript("startup", "assets/scripts/startup.lua");
    GLOBAL_ENGINE.scriptManager.scriptResults["startup"]();
    GLOBAL_ENGINE.scriptManager.LoadScript("checkpress", "assets/scripts/checkpress.lua");
    GLOBAL_ENGINE.scriptManager.scriptResults["checkpress"]();

 //  GLOBAL_ENGINE. graphicsManager.LoadTexture("fox","assets/fox.png");
   
   auto e1 = ECS.CreateEntity();
   // auto e2 = ECS.CreateEntity();
/*
    ECS.Get<GraphicsManager::Sprite>( e2 ).image_name = "dorm";
    ECS.Get<GraphicsManager::Sprite>( e2 ).position ={0,0,0};
    ECS.Get<GraphicsManager::Sprite>( e2 ).scale ={100,100};
    ECS.Get<GraphicsManager::Sprite>( e2 ).z =0;
*/
    ECS.Get<GraphicsManager::Sprite>( e1 ).image_name = "fox";
    ECS.Get<GraphicsManager::Sprite>( e1 ).position ={0,0,0};
    ECS.Get<GraphicsManager::Sprite>( e1 ).scale ={50,50};
    ECS.Get<GraphicsManager::Sprite>( e1 ).z =0;
    ECS.Get<EntityManager::Health>(e1 ) = EntityManager::Health{100};
    bool spaceKeyPressed = false; 

  /*  GLOBAL_ENGINE.scriptManager.LoadScript("startup", "assets/scripts/startup.lua");
    GLOBAL_ENGINE.scriptManager.scriptResults["startup"]();
    GLOBAL_ENGINE.scriptManager.LoadScript("checkpress", "assets/scripts/checkpress.lua");
    GLOBAL_ENGINE.scriptManager.scriptResults["checkpress"]();
*/

    GLOBAL_ENGINE.RunGameLoop( [&]( Engine& ) {

         printMessage(GLOBAL_ENGINE);
         GLOBAL_ENGINE.scriptManager.UpdateEngine();
         //if space clicked deduct health 
         if( GLOBAL_ENGINE.inputManager.KeyisPressed( GLFW_KEY_SPACE)&& spaceKeyPressed == false) {

               if(ECS.Get<EntityManager::Health>( e1 ).percent!= 0){
                  ECS.Get<EntityManager::Health>( e1 ).percent -= 25;
                  printf( "Health is: %lf\n",ECS.Get<EntityManager::Health>( e1 ).percent);
                  spaceKeyPressed = true;
                }
         }
         if(GLOBAL_ENGINE.inputManager.KeyisReleased( GLFW_KEY_SPACE)){
            spaceKeyPressed = false;
         }

         //move fox right
         if( GLOBAL_ENGINE.inputManager.KeyisPressed( GLFW_KEY_R)) {
            ECS.Get<GraphicsManager::Sprite>( e1 ).position.x+= .1;
            
         }


   
     } );
    
    GLOBAL_ENGINE.Shutdown(); 
/*
    try {
        // Load and execute your Lua scripts
        std::unordered_map<std::string, sol::load_result> loadedScripts;

        // Load Lua scripts and store them in the map
        loadedScripts["startup"] = lua.load_file("startup.lua");
        loadedScripts["checkpress"] = lua.load_file("checkpress.lua");
        
        loadedScripts["startup"]();
        loadedScripts["checkpress"]();

    } catch (const sol::error& e) {
        std::cerr << "Error running Lua script: " << e.what() << std::endl;
    }*/
    /*
     GLOBAL_ENGINE. graphicsManager.LoadTexture("dorm","assets/dormbase.jpg");

    GLOBAL_ENGINE. graphicsManager.LoadTexture("imageName","assets/fox.png");
   
    auto e1 = ECS.CreateEntity();
    auto e2 = ECS.CreateEntity();

   ECS.Get<GraphicsManager::Sprite>( e2 ).image_name = "dorm";
    ECS.Get<GraphicsManager::Sprite>( e2 ).position ={0,0,0};
    ECS.Get<GraphicsManager::Sprite>( e2 ).scale ={100,100};
    ECS.Get<GraphicsManager::Sprite>( e2 ).z =0;

    ECS.Get<GraphicsManager::Sprite>( e1 ).image_name = "imageName";
    ECS.Get<GraphicsManager::Sprite>( e1 ).position ={0,0,0};
    ECS.Get<GraphicsManager::Sprite>( e1 ).scale ={50,50};
    ECS.Get<GraphicsManager::Sprite>( e1 ).z =0;
    ECS.Get<EntityManager::Health>(e1 ) = EntityManager::Health{100};
    */
  //  bool spaceKeyPressed = false;
 //   GLOBAL_ENGINE.RunGameLoop( [&]( Engine& ) {

    //    printMessage(GLOBAL_ENGINE);

        // GLOBAL_ENGINE.scriptManager.UpdateEngine();
         /*
        try {
        // Assuming 'onGameLoop' is a global function in the loaded checkpress.lua script
        lua["onGameLoop"]();
      } catch (const sol::error& e) {
        std::cerr << "Error running onGameLoop: " << e.what() << std::endl;
         }*/

         //if space clicked deduct health 
         /*
         if( GLOBAL_ENGINE.inputManager.KeyisPressed( GLFW_KEY_SPACE)&& spaceKeyPressed == false) {

               if(ECS.Get<EntityManager::Health>( e1 ).percent!= 0){
                  ECS.Get<EntityManager::Health>( e1 ).percent -= 25;
                  printf( "Health is: %lf\n",ECS.Get<EntityManager::Health>( e1 ).percent);
                  spaceKeyPressed = true;
                }
         }
         if(GLOBAL_ENGINE.inputManager.KeyisReleased( GLFW_KEY_SPACE)){
            spaceKeyPressed = false;
         }

         //move fox right
         if (GLOBAL_ENGINE.inputManager.KeyisPressed(GLFW_KEY_R)) {
             ECS.Get<GraphicsManager::Sprite>( e1 ).position.x+= .1;
         }*/
            
    
           


   
   //  } );
    
   // GLOBAL_ENGINE.Shutdown(); 
    
    return 0;
}

