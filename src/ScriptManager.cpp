#include "ScriptManager.h"
#include "Engine.h"

ScriptManager::ScriptManager()
{
}

void ScriptManager::Start()
{
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);
    lua.script("math.randomseed(0)");
    
    //----------set function-----------------
    lua.set_function( "KeyisPressed", [&]( const int keycode ) { return GLOBAL_ENGINE.inputManager.KeyisPressed( keycode ); } );
    lua.set_function("KeyisReleased",[&]( const int keycode ) { return GLOBAL_ENGINE.inputManager.KeyisReleased( keycode ); } );

    lua.set_function("ExitGame", [&](){return  GLOBAL_ENGINE.ExitGame(); });
  

    lua.new_enum("KEYBOARD", 
                 "R", GLFW_KEY_R,
                 "SPACE", GLFW_KEY_SPACE,
                 "Q", GLFW_KEY_Q,
                 "ESC", GLFW_KEY_ESCAPE
                 // Add more keycodes here
    );


    lua.set_function("CreateEntity",[&](){return ECS.CreateEntity();});
      
    lua.set_function("LoadTexture", [&](const std::string& name, const std::string& path) {
    return GLOBAL_ENGINE.graphicsManager.LoadTexture(name, path);
    }); 
    lua.set_function("GetSprite", [&](EntityID e) -> GraphicsManager::Sprite& {
    return ECS.Get<GraphicsManager::Sprite>(e);
   });

    lua.set_function("GetScript", [&](EntityID e) -> EntityManager::Script& {
    return ECS.Get<EntityManager::Script>(e);
   });

    lua.set_function("GetPosition", [&](EntityID e) -> EntityManager::Position& {
    return ECS.Get<EntityManager::Position>(e);
   });
   lua.set_function("GetHealth", [&](EntityID e) -> EntityManager::Health& {
    return ECS.Get<EntityManager::Health>(e);
   });

   lua.set_function("GetGravity", [&](EntityID e) -> EntityManager::Gravity& {
    return ECS.Get<EntityManager::Gravity>(e);
   });

   lua.set_function("GetVelocity", [&](EntityID e) -> EntityManager::Velocity& {
    return ECS.Get<EntityManager::Velocity>(e);
   });

   /* JENNNIFER EDITS */
   //Sound Manager Function
   lua.set_function("LoadSound", [&](const std::string& name, const std::string& path) {GLOBAL_ENGINE.soundManager.LoadSound(name, path);});
   lua.set_function("PlaySound", [&](const std::string& name){GLOBAL_ENGINE.soundManager.PlaySound(name);});

// ---------------------usertype sets------------------
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

    lua.new_usertype<EntityManager::Position>(
    "Position",
    sol::constructors<EntityManager::Position()>(), 
    "x", &EntityManager::Position::x,
    "y", &EntityManager::Position::y
    );
      lua.new_usertype<EntityManager::Script>(
    "Script",
    sol::constructors<EntityManager::Script()>(), 
    "name", &EntityManager::Script::name
    );
    lua.new_usertype<EntityManager::Gravity>(
    "Gravity",
    sol::constructors<EntityManager::Gravity()>(), 
    "meters_per_second", &EntityManager::Gravity::meters_per_second
    );
    
    lua.new_usertype<EntityManager::Velocity>(
    "Velocity",
    sol::constructors<EntityManager::Velocity()>(), 
    "x", &EntityManager::Velocity::x,
    "y", &EntityManager::Velocity::y
    );


    lua.new_usertype<glm::vec3>("vec3",
    sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
    "x", &glm::vec3::x,
    "y", &glm::vec3::y,
    "z", &glm::vec3::z,
    // optional and fancy: operator overloading. see: https://github.com/ThePhD/sol2/issues/547
    sol::meta_function::addition, sol::overload( [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1+v2; } ),
    sol::meta_function::subtraction, sol::overload( [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1-v2; } ),
    sol::meta_function::multiplication, sol::overload(
        [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1*v2; },
        [](const glm::vec3& v1, float f) -> glm::vec3 { return v1*f; },
        [](float f, const glm::vec3& v1) -> glm::vec3 { return f*v1; }
    )
);

lua.new_usertype<glm::vec2>("vec2",
    sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
    "x", &glm::vec2::x,
    "y", &glm::vec2::y
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
