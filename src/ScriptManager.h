#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <string>
#include "EntityManager.h"
#include <unordered_map>
class ScriptManager{

    private:

      //  std::filesystem::path rootPath;
    public: //= "assets"
     
        ScriptManager(); 
        sol::state lua;
        //sol::load_result script;
        std::unordered_map<std::string, sol::load_result> scriptResults;

        struct Script{
             std::string name;

        };

        void Start();
        void Shutdown();
        bool LoadScript(std::string name,std::string path );
        void UpdateEngine();


};

#endif