#include "ScriptManager.h"

ScriptManager::ScriptManager()
{
}

void ScriptManager::Start()
{
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);
    lua.script("math.randomseed(0)");
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
