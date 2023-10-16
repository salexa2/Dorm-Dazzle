#include "ScriptManager.h"
#include "Engine.h"

ScriptManager::ScriptManager()
{
}

void ScriptManager::Start()
{
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);
    lua.script("math.randomseed(0)");
    
    lua.set_function( "KeyisPressed", [&]( const int keycode ) { return GLOBAL_ENGINE.inputManager.KeyisPressed( keycode ); } );
    lua.new_enum("KEYBOARD",
                 "R", GLFW_KEY_R,
                 "SPACE", GLFW_KEY_SPACE
                 // Add more keycodes here
    );



    lua.set_function("KeyisReleased", &InputManager::KeyisReleased, &GLOBAL_ENGINE.inputManager);
    lua.set_function("CreateEntity", &EntityManager::CreateEntity, &ECS);
      
    lua.set_function("LoadTexture", [&](const std::string& name, const std::string& path) {
    return GLOBAL_ENGINE.graphicsManager.LoadTexture(name, path);
    });

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

  
}

void ScriptManager::Shutdown()
{
}

bool ScriptManager::LoadScript(std::string name,std::string path)
{
   

   scriptResults[name] = lua.load_file( path );

    if (!scriptResults[name].valid()) {
        sol::error error = scriptResults[name];
        std::cerr << "Error loading Lua script: " << error.what() << std::endl;
        return false;
    }

   
   
    return true;
}


//hard
void ScriptManager::UpdateEngine()
{
     ECS.ForEach<Script>( [&]( EntityID entity ) {
        ECS.Get<Script>(entity);
    } );
}
